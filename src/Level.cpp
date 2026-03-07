#include "Level.h"
#include "LevelGenerator.h"
#include "SoundManager.h"
#include "Config.h"
#include <cmath>
#include <iostream>
#include <limits>

Level::Level() : m_levelNumber(1), m_completed(false), m_gameOver(false), 
                 m_enemiesKilled(0), m_totalEnemies(0), m_mousePressed(false),
                 m_heartSprite(m_heartTexture), m_hasHeartTexture(false),
                 m_heartDropped(false), m_heartDropAtKill(1) {
}

bool Level::init(int levelNumber) {
    m_levelNumber = levelNumber;
    m_completed = false;
    m_gameOver = false;
    m_enemiesKilled = 0;
    m_mousePressed = false;
    m_heartDropped = false;
    
    if (!m_font.openFromFile("assets/fonts/Helvetica.ttc")) {
        if (!m_font.openFromFile("/Library/Fonts/Arial.ttf")) {
            std::cout << "Warning: Font not found in Level" << std::endl;
        }
    }

    if (m_heartTexture.loadFromFile(Config::HEART_SPRITE)) {
        m_heartSprite.setTexture(m_heartTexture);
        sf::FloatRect bounds = m_heartSprite.getLocalBounds();
        m_heartSprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        m_hasHeartTexture = true;
    } else {
        m_hasHeartTexture = false;
    }
    
    (void)m_bushTexture.loadFromFile(Config::BUSH_SPRITE);
    (void)m_cactusTexture.loadFromFile(Config::CACTUS_SPRITE);
    (void)m_treeTexture.loadFromFile(Config::TREE_SPRITE);
    
    m_player.init();
    m_player.setPosition(400.0f, 300.0f);
    
    LevelGenerator generator;
    generator.generateLevel(levelNumber, m_enemies, m_obstacles, m_bushTexture, m_cactusTexture, m_treeTexture);
    
    m_totalEnemies = static_cast<int>(m_enemies.size());
    
    // Heart drops exactly once per level at the first enemy kill
    m_heartDropAtKill = 1;
    
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
    
    for (auto& bullet : m_bullets) {
        bullet->update(deltaTime);
    }
    
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const auto& bullet) { return !bullet->isActive(); }),
        m_bullets.end()
    );
    
    updateBonuses(deltaTime);
    
    // Auto-shoot at nearest enemy
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
    
    // Update HUD with correct values
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
    sf::RectangleShape floor({800, 600});
    floor.setFillColor(sf::Color(30, 50, 30));
    window.draw(floor);

    for (const auto& obstacle : m_obstacles) {
        if (obstacle.sprite) window.draw(*obstacle.sprite);
    }
    
    for (const auto& bonus : m_heartBonuses) {
        if (bonus.active) {
            if (m_hasHeartTexture) {
                m_heartSprite.setPosition(bonus.position);
                m_heartSprite.setScale(bonus.shape.getScale());
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
    // Bullet vs enemy
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
                    
                    // Drop heart only once per level, at the random kill
                    if (!m_heartDropped && m_enemiesKilled == m_heartDropAtKill) {
                        m_heartDropped = true;
                        spawnHeartBonus(enemy->getPosition());
                    }
                    SoundManager::getInstance().playSound(SoundID::EnemyDeath);
                }
                break;
            }
        }
        
        // Bullet vs obstacle
        for (const auto& obstacle : m_obstacles) {
            if (bullet->isActive() && bullet->getBounds().findIntersection(obstacle.bounds)) {
                bullet->deactivate();
                break;
            }
        }
    }
    
    // Enemy vs player (contact damage)
    for (auto& enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        
        if (m_player.getBounds().findIntersection(enemy->getBounds())) {
            m_player.takeDamage(1);
            enemy->takeDamage(999);
            m_enemiesKilled++;
            SoundManager::getInstance().playSound(SoundID::Damage);
            
            if (!m_heartDropped && m_enemiesKilled == m_heartDropAtKill) {
                m_heartDropped = true;
                spawnHeartBonus(enemy->getPosition());
            }
            
            if (m_player.getHealth() <= 0) {
                m_gameOver = true;
                SoundManager::getInstance().playSound(SoundID::PlayerDeath);
            }
        }
    }
    
    // Player vs heart bonus
    for (auto& bonus : m_heartBonuses) {
        if (!bonus.active) continue;
        sf::FloatRect bonusBounds(bonus.position - sf::Vector2f(12, 12), {24, 24});
        if (m_player.getBounds().findIntersection(bonusBounds)) {
            m_player.heal(1);
            bonus.active = false;
            SoundManager::getInstance().playSound(SoundID::BonusPickUp);
        }
    }
    
    // Remove dead enemies
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

void Level::spawnHeartBonus(const sf::Vector2f& position) {
    HeartBonus bonus;
    bonus.position = position;
    bonus.active = true;
    bonus.lifetime = HeartBonus::MAX_LIFETIME;
    
    bonus.shape.setRadius(12.0f);
    bonus.shape.setFillColor(sf::Color::Red);
    bonus.shape.setOrigin({12.0f, 12.0f});
    bonus.shape.setPosition(position);
    
    m_heartBonuses.push_back(std::move(bonus));
}

bool Level::checkObstacleCollision(const sf::FloatRect& bounds) const {
    for (const auto& obstacle : m_obstacles) {
        if (bounds.findIntersection(obstacle.bounds)) return true;
    }
    return false;
}
