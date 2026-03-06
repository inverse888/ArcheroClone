#include "Level.h"
#include "LevelGenerator.h"
#include "SoundManager.h"
#include "Config.h"
#include <cmath>
#include <iostream>
#include <limits>

Level::Level() : m_levelNumber(1), m_completed(false), m_gameOver(false), 
                 m_enemiesKilled(0), m_totalEnemies(0), m_mousePressed(false),
                 m_heartSprite(m_heartTexture), m_hasHeartTexture(false) {
}

bool Level::init(int levelNumber) {
    std::cout << "Level::init " << levelNumber << std::endl;
    m_levelNumber = levelNumber;
    m_completed = false;
    m_gameOver = false;
    m_enemiesKilled = 0;
    m_mousePressed = false;
    
    // Загружаем шрифт
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
    
    // Load obstacle textures once
    (void)m_bushTexture.loadFromFile(Config::BUSH_SPRITE);
    (void)m_cactusTexture.loadFromFile(Config::CACTUS_SPRITE);
    (void)m_treeTexture.loadFromFile(Config::TREE_SPRITE);
    
    // Инициализируем игрока
    std::cout << "Player::init" << std::endl;
    m_player.init();
    m_player.setPosition(400.0f, 300.0f);
    
    // Генерируем уровень
    std::cout << "Generator::generateLevel" << std::endl;
    LevelGenerator generator;
    generator.generateLevel(levelNumber, m_enemies, m_obstacles, m_bushTexture, m_cactusTexture, m_treeTexture);
    
    m_totalEnemies = m_enemies.size();
    
    m_bullets.clear();
    m_heartBonuses.clear();
    
    // Создаём текстовые элементы
    std::cout << "Creating UI" << std::endl;
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
        "ENEMIES: 0/" + std::to_string(m_totalEnemies), 24);
    m_enemiesText->setFillColor(sf::Color::White);
    m_enemiesText->setOutlineColor(sf::Color::Black);
    m_enemiesText->setOutlineThickness(1);
    m_enemiesText->setPosition({20, 90});
    
    return true;
}

void Level::update(float deltaTime) {
    if (m_completed || m_gameOver) return;
    
    // Обновляем игрока
    m_player.update(deltaTime, m_obstacles);
    
    // Обновляем врагов
    for (auto& enemy : m_enemies) {
        enemy->update(deltaTime, m_player.getPosition(), m_obstacles);
    }
    
    // Обновляем пули
    for (auto& bullet : m_bullets) {
        bullet->update(deltaTime);
    }
    
    // Удаляем неактивные пули
    m_bullets.erase(
        std::remove_if(m_bullets.begin(), m_bullets.end(),
            [](const auto& bullet) { return !bullet->isActive(); }),
        m_bullets.end()
    );
    
    // Обновляем бонусы
    updateBonuses(deltaTime);
    
    // АВТОМАТИЧЕСКАЯ СТРЕЛЬБА ПО БЛИЖАЙШЕМУ ВРАГУ
    if (!m_enemies.empty()) {
        sf::Vector2f playerPos = m_player.getPosition();
        float minDist = std::numeric_limits<float>::max();
        sf::Vector2f closestEnemyPos;
        bool foundEnemy = false;
        
        // Ищем ближайшего живого врага
        for (auto& enemy : m_enemies) {
            if (!enemy->isAlive()) continue;
            
            sf::Vector2f enemyPos = enemy->getPosition();
            float dx = enemyPos.x - playerPos.x;
            float dy = enemyPos.y - playerPos.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist < minDist) {
                minDist = dist;
                closestEnemyPos = enemyPos;
                foundEnemy = true;
            }
        }
        
        // Стреляем в ближайшего врага, если он найден
        if (foundEnemy) {
            auto bullet = m_player.shoot(closestEnemyPos);
            if (bullet) {
                m_bullets.push_back(std::move(bullet));
            }
        }
    }
    
    // Проверяем коллизии
    checkCollisions();
    
    // Проверяем, убиты ли все враги (не сразу, а когда вектор действительно пуст)
    if (m_enemies.empty() && m_totalEnemies > 0 && !m_completed) {
        m_completed = true;
    }
    
    // Обновляем текстовые элементы
    if (m_healthText) {
        m_healthText->setString("Health: " + std::to_string(m_player.getHealth()) + 
                               "/" + std::to_string(m_player.getMaxHealth()));
    }
    
    if (m_enemiesText) {
        m_enemiesText->setString("Enemies: " + std::to_string(m_enemiesKilled) + 
                                "/" + std::to_string(m_totalEnemies));
    }
}

void Level::render(sf::RenderWindow& window) {
    // Draw background (greenish floor) to see the bounds
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
                    spawnHeartBonus(enemy->getPosition());
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
    
    for (auto& enemy : m_enemies) {
        if (!enemy->isAlive()) continue;
        
        if (m_player.getBounds().findIntersection(enemy->getBounds())) {
            m_player.takeDamage(1);
            enemy->takeDamage(999);
            SoundManager::getInstance().playSound(SoundID::Damage);
            
            if (m_player.getHealth() <= 0) {
                m_gameOver = true;
                SoundManager::getInstance().playSound(SoundID::PlayerDeath);
            }
        }
    }
    
    for (auto& bonus : m_heartBonuses) {
        if (!bonus.active) continue;
        
        if (m_player.getBounds().findIntersection(bonus.shape.getGlobalBounds())) {
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
            if (bonus.lifetime <= 0.0f) {
                bonus.active = false;
            }
            
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
    
    bonus.shape.setRadius(10.0f);
    bonus.shape.setFillColor(sf::Color::Red);
    bonus.shape.setOrigin({10.0f, 10.0f});
    bonus.shape.setPosition(position);
    
    m_heartBonuses.push_back(std::move(bonus));
}

bool Level::checkObstacleCollision(const sf::FloatRect& bounds) const {
    for (const auto& obstacle : m_obstacles) {
        if (bounds.findIntersection(obstacle.bounds)) {
            return true;
        }
    }
    return false;
}
