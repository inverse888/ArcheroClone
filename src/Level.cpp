#include "Level.h"
#include "LevelGenerator.h"
#include "SoundManager.h"
#include "Config.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

Level::Level() : m_levelNumber(1), m_completed(false), m_gameOver(false), 
                 m_enemiesKilled(0), m_totalEnemies(0), m_mousePressed(false),
                 m_heartSprite(m_heartTexture), m_hasHeartTexture(false),
                 m_heartDropped(false), m_heartDropAtKill(1), m_contactDamageCooldown(0.0f),
                 m_floorSprite(m_floorTexture) {
}

bool Level::init(int levelNumber) {
    m_levelNumber = levelNumber;
    m_completed = false;
    m_gameOver = false;
    m_enemiesKilled = 0;
    m_mousePressed = false;
    m_heartDropped = false;
    m_contactDamageCooldown = 0.0f;
    
    if (!m_font.openFromFile(Config::FONT_MAIN)) {
        if (!m_font.openFromFile("assets/fonts/Helvetica.ttc")) {
            std::cout << "Warning: Font not found in Level" << std::endl;
        }
    }

    if (m_heartTexture.loadFromFile(Config::HEART_SPRITE) || m_heartTexture.loadFromFile("assets/images/heart.png")) {
        m_heartSprite.setTexture(m_heartTexture, true);
        sf::FloatRect bounds = m_heartSprite.getLocalBounds();
        m_heartSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        m_heartSprite.setScale({3.0f, 3.0f});
        m_hasHeartTexture = true;
    } else {
        m_hasHeartTexture = false;
    }
    
    std::string bushPath = Config::BUSH_SPRITE;
    std::string cactusPath = Config::CACTUS_SPRITE;
    std::string treePath = Config::TREE_SPRITE;
    if (levelNumber == 2) {
        bushPath = Config::ICE_BUSH_SPRITE;
        cactusPath = Config::ICE_DECOR_SPRITE;
        treePath = Config::ICE_TREE_SPRITE;
    } else if (levelNumber >= 3) {
        bushPath = Config::LAVA_BUSH_SPRITE;
        cactusPath = Config::LAVA_DECOR_SPRITE;
        treePath = Config::LAVA_TREE_SPRITE;
    }

    (void)m_bushTexture.loadFromFile(bushPath);
    (void)m_cactusTexture.loadFromFile(cactusPath);
    (void)m_treeTexture.loadFromFile(treePath);

    std::string bgPath = "assets/images/background_first.png";
    if (levelNumber == 2) bgPath = "assets/images/background_second.png";
    if (levelNumber >= 3) bgPath = "assets/images/background_third.png";
    bool bgLoaded = m_floorTexture.loadFromFile(bgPath);
    if (!bgLoaded && levelNumber == 2) bgLoaded = m_floorTexture.loadFromFile("assets/images/background_second.avif");
    if (!bgLoaded) bgLoaded = m_floorTexture.loadFromFile("assets/images/background_first.jpg");
    if (!bgLoaded) bgLoaded = m_floorTexture.loadFromFile("assets/images/background_third.jpg");
    if (bgLoaded) {
        m_floorTexture.setSmooth(true);
        m_floorSprite.setTexture(m_floorTexture, true);
        sf::Vector2u sz = m_floorTexture.getSize();
        if (sz.x > 0 && sz.y > 0) {
            m_floorSprite.setScale({800.0f / static_cast<float>(sz.x), 600.0f / static_cast<float>(sz.y)});
        }
    }
    m_floorSprite.setPosition({0.0f, 0.0f});
    
    m_player.init();
    m_player.setPosition(400.0f, 300.0f);
    
    LevelGenerator generator;
    generator.generateLevel(levelNumber, m_enemies, m_obstacles, m_bushTexture, m_cactusTexture, m_treeTexture);
    
    m_totalEnemies = static_cast<int>(m_enemies.size());
    
    
    std::mt19937 rng(std::random_device{}());
    m_heartDropAtKill = (m_totalEnemies >= 3) ? 3 : m_totalEnemies;
    
    m_bullets.clear();
    m_heartBonuses.clear();
    
    m_levelText = std::make_unique<sf::Text>(m_font, "LEVEL " + std::to_string(levelNumber), 30);
    m_levelText->setFillColor(sf::Color::Yellow);
    m_levelText->setOutlineColor(sf::Color::Black);
    m_levelText->setOutlineThickness(2);
    m_levelText->setPosition({20, 20});
    
    m_healthText = std::make_unique<sf::Text>(m_font, 
        "HP: " + std::to_string(m_player.getHealth()) + "/" + std::to_string(m_player.getMaxHealth()), 24);
    m_healthText->setFillColor(sf::Color::Red);
    m_healthText->setOutlineColor(sf::Color::Black);
    m_healthText->setOutlineThickness(1);
    m_healthText->setPosition({20, 60});
    
    m_enemiesText = std::make_unique<sf::Text>(m_font, 
        "Killed: 0/" + std::to_string(m_totalEnemies), 24);
    m_enemiesText->setFillColor(sf::Color::White);
    m_enemiesText->setOutlineColor(sf::Color::Black);
    m_enemiesText->setOutlineThickness(1);
    m_enemiesText->setPosition({20, 90});
    
    return true;
}

void Level::update(float deltaTime) {
    if (m_completed || m_gameOver) return;
    
    m_player.update(deltaTime, m_obstacles);
    
    for (auto& enemy : m_enemies) {
        enemy->update(deltaTime, m_player.getPosition(), m_obstacles);
    }
    resolveEnemyOverlap();
    
    for (auto& bullet : m_bullets) {
        bullet->update(deltaTime);
    }
    
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const auto& bullet) { return !bullet->isActive(); }),
        m_bullets.end()
    );
    
    updateBonuses(deltaTime);
    if (m_contactDamageCooldown > 0.0f) {
        m_contactDamageCooldown -= deltaTime;
    }
    
    
    if (!m_enemies.empty()) {
        sf::Vector2f playerPos = m_player.getPosition();
        float minDist = std::numeric_limits<float>::max();
        sf::Vector2f closestPos;
        bool found = false;
        
        for (auto& enemy : m_enemies) {
            if (!enemy->isAlive()) continue;
            sf::Vector2f ep = enemy->getPosition();
            float d = std::sqrt((ep.x - playerPos.x) * (ep.x - playerPos.x) + 
                               (ep.y - playerPos.y) * (ep.y - playerPos.y));
            if (d < minDist) { minDist = d; closestPos = ep; found = true; }
        }
        
        if (found) {
            auto bullet = m_player.shoot(closestPos);
            if (bullet) m_bullets.push_back(std::move(bullet));
        }
    }
    
    checkCollisions();
    
    if (m_enemies.empty() && m_totalEnemies > 0 && !m_completed) {
        m_completed = true;
    }
    
    
    if (m_healthText) {
        m_healthText->setString("HP: " + std::to_string(m_player.getHealth()) + 
                               "/" + std::to_string(m_player.getMaxHealth()));
    }
    if (m_enemiesText) {
        m_enemiesText->setString("Killed: " + std::to_string(m_enemiesKilled) + 
                                "/" + std::to_string(m_totalEnemies));
    }
}

void Level::render(sf::RenderWindow& window) {
    window.draw(m_floorSprite);

    for (const auto& obstacle : m_obstacles) {
        if (obstacle.sprite) window.draw(*obstacle.sprite);
    }
    
    for (const auto& bonus : m_heartBonuses) {
        if (bonus.active) {
            if (m_hasHeartTexture) {
                m_heartSprite.setPosition(bonus.position);
                m_heartSprite.setScale(bonus.shape.getScale() * 3.0f);
                window.draw(m_heartSprite);
            } else {
                window.draw(bonus.shape);
            }
        }
    }
    
    m_player.render(window);
    
    for (const auto& enemy : m_enemies) {
        enemy->render(window);
    }
    
    for (const auto& bullet : m_bullets) {
        bullet->render(window);
    }
    
    if (m_levelText) window.draw(*m_levelText);
    if (m_healthText) window.draw(*m_healthText);
    if (m_enemiesText) window.draw(*m_enemiesText);
}

void Level::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    m_player.handleEvent(event);
}

void Level::checkCollisions() {
    
    for (auto& bullet : m_bullets) {
        if (!bullet->isActive()) continue;
        
        for (auto& enemy : m_enemies) {
            if (!enemy->isAlive()) continue;
            
            if (bullet->getBounds().findIntersection(enemy->getBounds())) {
                enemy->takeDamage(bullet->getDamage());
                bullet->deactivate();
                SoundManager::getInstance().playSound(SoundID::Hit);
                
                if (!enemy->isAlive()) {
                    m_enemiesKilled++;
                    
                    
                    if (!m_heartDropped && m_enemiesKilled == m_heartDropAtKill) {
                        m_heartDropped = true;
                        spawnHeartBonus(enemy->getPosition());
                    }
                    SoundManager::getInstance().playSound(SoundID::EnemyDeath);
                }
                break;
            }
        }
        
        
        for (const auto& obstacle : m_obstacles) {
            if (bullet->isActive() && bullet->getBounds().findIntersection(obstacle.bounds)) {
                bullet->deactivate();
                break;
            }
        }
    }
    
    
    bool touchingEnemy = false;
    for (const auto& enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        if (m_player.getBounds().findIntersection(enemy->getBounds())) {
            touchingEnemy = true;
            break;
        }
    }
    if (touchingEnemy && m_contactDamageCooldown <= 0.0f) {
        m_player.takeDamage(1);
        m_contactDamageCooldown = 0.6f;
        SoundManager::getInstance().playSound(SoundID::Damage);

        if (m_player.getHealth() <= 0) {
            m_gameOver = true;
            SoundManager::getInstance().playSound(SoundID::PlayerDeath);
        }
    }
    
    
    for (auto& bonus : m_heartBonuses) {
        if (!bonus.active) continue;
        sf::FloatRect bonusBounds(bonus.position - sf::Vector2f(18, 18), {36, 36});
        if (m_player.getBounds().findIntersection(bonusBounds)) {
            m_player.heal(1);
            bonus.active = false;
            SoundManager::getInstance().playSound(SoundID::BonusPickUp);
        }
    }
    
    
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const auto& enemy) { return !enemy->isAlive(); }),
        m_enemies.end()
    );


}

void Level::updateBonuses(float deltaTime) {
    for (auto& bonus : m_heartBonuses) {
        if (bonus.active) {
            bonus.lifetime -= deltaTime;
            if (bonus.lifetime <= 0.0f) bonus.active = false;
            float scale = 1.0f + 0.2f * std::sin(bonus.lifetime * 5.0f);
            bonus.shape.setScale({scale, scale});
        }
    }
}

void Level::resolveEnemyOverlap() {
    constexpr float MIN_DISTANCE = 28.0f;
    const float minDistSq = MIN_DISTANCE * MIN_DISTANCE;
    auto canStandAt = [this](const sf::Vector2f& p) {
        sf::FloatRect enemyRect(p - sf::Vector2f(15.0f, 15.0f), {30.0f, 30.0f});
        return !checkObstacleCollision(enemyRect);
    };

    for (std::size_t i = 0; i < m_enemies.size(); ++i) {
        if (!m_enemies[i]->isAlive()) continue;

        for (std::size_t j = i + 1; j < m_enemies.size(); ++j) {
            if (!m_enemies[j]->isAlive()) continue;

            sf::Vector2f delta = m_enemies[j]->getPosition() - m_enemies[i]->getPosition();
            float distSq = delta.x * delta.x + delta.y * delta.y;
            if (distSq <= 0.0001f || distSq >= minDistSq) continue;

            float dist = std::sqrt(distSq);
            sf::Vector2f normal = delta / dist;
            float push = (MIN_DISTANCE - dist) * 0.5f;

            sf::Vector2f p1 = m_enemies[i]->getPosition() - normal * push;
            sf::Vector2f p2 = m_enemies[j]->getPosition() + normal * push;

            p1.x = std::clamp(p1.x, 20.0f, 780.0f);
            p1.y = std::clamp(p1.y, 20.0f, 580.0f);
            p2.x = std::clamp(p2.x, 20.0f, 780.0f);
            p2.y = std::clamp(p2.y, 20.0f, 580.0f);

            if (canStandAt(p1)) {
                m_enemies[i]->setPosition(p1);
            }
            if (canStandAt(p2)) {
                m_enemies[j]->setPosition(p2);
            }
        }
    }
}

void Level::spawnHeartBonus(const sf::Vector2f& position) {
    HeartBonus bonus;
    bonus.position = position;
    const std::array<sf::Vector2f, 9> offsets = {
        sf::Vector2f{0.0f, 0.0f},
        sf::Vector2f{24.0f, 0.0f},
        sf::Vector2f{-24.0f, 0.0f},
        sf::Vector2f{0.0f, 24.0f},
        sf::Vector2f{0.0f, -24.0f},
        sf::Vector2f{24.0f, 24.0f},
        sf::Vector2f{-24.0f, 24.0f},
        sf::Vector2f{24.0f, -24.0f},
        sf::Vector2f{-24.0f, -24.0f}
    };
    for (const auto& offset : offsets) {
        sf::Vector2f candidate = position + offset;
        candidate.x = std::clamp(candidate.x, 24.0f, 776.0f);
        candidate.y = std::clamp(candidate.y, 24.0f, 576.0f);
        sf::FloatRect candidateBounds(candidate - sf::Vector2f(16.0f, 16.0f), {32.0f, 32.0f});
        if (!checkObstacleCollision(candidateBounds)) {
            bonus.position = candidate;
            break;
        }
    }
    bonus.active = true;
    bonus.lifetime = HeartBonus::MAX_LIFETIME;
    
    bonus.shape.setRadius(18.0f);
    bonus.shape.setFillColor(sf::Color::Red);
    bonus.shape.setOrigin({18.0f, 18.0f});
    bonus.shape.setPosition(bonus.position);
    
    m_heartBonuses.push_back(std::move(bonus));
}

bool Level::checkObstacleCollision(const sf::FloatRect& bounds) const {
    for (const auto& obstacle : m_obstacles) {
        if (bounds.findIntersection(obstacle.bounds)) return true;
    }
    return false;
}
