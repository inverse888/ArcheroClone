#include "LevelGenerator.h"
#include "Config.h"
#include <random>
#include <iostream>

LevelGenerator::LevelGenerator() : m_rng(std::random_device{}()) {
}

void LevelGenerator::generateLevel(int levelNumber, 
                                 std::vector<std::unique_ptr<Enemy>>& enemies,
                                 std::vector<Obstacle>& obstacles,
                                 const sf::Texture& bush, const sf::Texture& cactus, const sf::Texture& tree) {
    enemies.clear();
    obstacles.clear();
    
    RoomConfig config = getConfigForLevel(levelNumber);
    
    std::uniform_int_distribution<int> numObsDist(config.minObstacles, config.maxObstacles);
    int numObstacles = numObsDist(m_rng);
    
    std::uniform_int_distribution<int> typeDist(0, 2);

    for (int i = 0; i < numObstacles; ++i) {
        sf::Vector2f pos;
        bool valid = false;
        int attempts = 0;
        while (!valid && attempts < 100) {
            pos = getRandomPosition(50, 750, 50, 550);
            valid = isPositionValid(pos, obstacles);
            attempts++;
        }
        
        if (valid) {
            Obstacle obs;
            obs.position = pos;
            
            int type = typeDist(m_rng);
            const sf::Texture* tex = nullptr;
            if (type == 0) tex = &bush;
            else if (type == 1) tex = &cactus;
            else tex = &tree;

            obs.sprite = std::make_shared<sf::Sprite>(*tex);
            sf::FloatRect localBounds = obs.sprite->getLocalBounds();
            obs.sprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
            obs.sprite->setPosition(obs.position);
            obs.bounds = obs.sprite->getGlobalBounds();
            
            // Shrink collision box slightly
            obs.bounds = sf::FloatRect(obs.bounds.position + sf::Vector2f(10, 10), obs.bounds.size - sf::Vector2f(20, 20));
            
            obstacles.push_back(obs);
        }
    }
    
    std::uniform_int_distribution<int> numEnemiesDist(config.minEnemies, config.maxEnemies);
    int numEnemies = numEnemiesDist(m_rng);
    
    std::uniform_int_distribution<int> enemyTypeIdxDist(0, config.possibleEnemies.size() - 1);
    
    for (int i = 0; i < numEnemies; ++i) {
        sf::Vector2f pos;
        bool valid = false;
        int attempts = 0;
        while (!valid && attempts < 100) {
            pos = getRandomPosition(50, 750, 50, 550);
            float distToPlayer = std::sqrt(std::pow(pos.x - 400.0f, 2) + std::pow(pos.y - 300.0f, 2));
            if (distToPlayer > 150.0f && isPositionValid(pos, obstacles)) {
                valid = true;
            }
            attempts++;
        }
        
        if (valid) {
            EnemyType type = config.possibleEnemies[enemyTypeIdxDist(m_rng)];
            auto enemy = std::make_unique<Enemy>(type);
            enemy->init(pos);
            enemies.push_back(std::move(enemy));
        }
    }
}

RoomConfig LevelGenerator::getConfigForLevel(int levelNumber) {
    RoomConfig config;
    config.minEnemies = 3 + levelNumber;
    config.maxEnemies = 5 + levelNumber;
    config.minObstacles = 5 + levelNumber / 2;
    config.maxObstacles = 8 + levelNumber;
    
    config.possibleEnemies.push_back(EnemyType::SIMPLE);
    if (levelNumber >= 3) config.possibleEnemies.push_back(EnemyType::FAST);
    if (levelNumber >= 5) config.possibleEnemies.push_back(EnemyType::TANK);
    
    return config;
}

sf::Vector2f LevelGenerator::getRandomPosition(float minX, float maxX, float minY, float maxY) {
    std::uniform_real_distribution<float> xDist(minX, maxX);
    std::uniform_real_distribution<float> yDist(minY, maxY);
    return {xDist(m_rng), yDist(m_rng)};
}

bool LevelGenerator::isPositionValid(const sf::Vector2f& pos, 
                                   const std::vector<Obstacle>& obstacles,
                                   const sf::Vector2f& playerPos) {
    // Check distance to other obstacles
    for (const auto& obs : obstacles) {
        float dx = obs.position.x - pos.x;
        float dy = obs.position.y - pos.y;
        if (std::sqrt(dx*dx + dy*dy) < 60.0f) return false;
    }
    
    // Check distance to player
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    if (std::sqrt(dx*dx + dy*dy) < 100.0f) return false;
    
    return true;
}
