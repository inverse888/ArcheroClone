#include "LevelGenerator.h"
#include "Config.h"
#include <random>
#include <iostream>
#include <cmath>

LevelGenerator::LevelGenerator() : m_rng(std::random_device{}()) {
}

void LevelGenerator::generateLevel(int levelNumber, 
                                 std::vector<std::unique_ptr<Enemy>>& enemies,
                                 std::vector<Obstacle>& obstacles,
                                 const sf::Texture& bush, const sf::Texture& cactus, const sf::Texture& tree) {
    enemies.clear();
    obstacles.clear();
    
    RoomConfig config = getConfigForLevel(levelNumber);
    const sf::Texture* textures[3] = { &bush, &cactus, &tree };
    
    // --- Generate LINE formations (2-4 lines per level) ---
    std::uniform_int_distribution<int> numLinesDist(2, 3 + levelNumber);
    int numLines = numLinesDist(m_rng);
    
    for (int line = 0; line < numLines; ++line) {
        std::uniform_int_distribution<int> lengthDist(5, 9);
        int lineLength = lengthDist(m_rng);
        
        std::uniform_int_distribution<int> dirDist(0, 1); // 0=horizontal, 1=vertical
        bool horizontal = dirDist(m_rng) == 0;
        
        std::uniform_int_distribution<int> texIdx(0, 2);
        int tex = texIdx(m_rng);
        
        // Pick a start position that keeps the line on screen
        float startX, startY;
        float spacing = 40.0f;
        if (horizontal) {
            float maxStartX = 780.0f - lineLength * spacing;
            if (maxStartX < 30.0f) maxStartX = 30.0f;
            std::uniform_real_distribution<float> xd(30.0f, maxStartX);
            std::uniform_real_distribution<float> yd(40.0f, 560.0f);
            startX = xd(m_rng);
            startY = yd(m_rng);
        } else {
            float maxStartY = 580.0f - lineLength * spacing;
            if (maxStartY < 30.0f) maxStartY = 30.0f;
            std::uniform_real_distribution<float> xd(40.0f, 760.0f);
            std::uniform_real_distribution<float> yd(30.0f, maxStartY);
            startX = xd(m_rng);
            startY = yd(m_rng);
        }
        
        // Don't place lines too close to player spawn
        float distToPlayer = std::sqrt((startX - 400.0f) * (startX - 400.0f) + (startY - 300.0f) * (startY - 300.0f));
        if (distToPlayer < 80.0f) continue;
        
        for (int i = 0; i < lineLength; ++i) {
            sf::Vector2f pos;
            if (horizontal) pos = {startX + i * spacing, startY};
            else pos = {startX, startY + i * spacing};
            
            // Skip if too close to player spawn
            float dp = std::sqrt((pos.x - 400.0f) * (pos.x - 400.0f) + (pos.y - 300.0f) * (pos.y - 300.0f));
            if (dp < 70.0f) continue;
            
            // Skip if off screen
            if (pos.x < 20 || pos.x > 780 || pos.y < 20 || pos.y > 580) continue;
            
            Obstacle obs;
            obs.position = pos;
            obs.sprite = std::make_shared<sf::Sprite>(*textures[tex]);
            sf::FloatRect lb = obs.sprite->getLocalBounds();
            obs.sprite->setOrigin({lb.size.x / 2.0f, lb.size.y / 2.0f});
            obs.sprite->setPosition(pos);
            obs.bounds = obs.sprite->getGlobalBounds();
            obs.bounds = sf::FloatRect(obs.bounds.position + sf::Vector2f(8, 8), obs.bounds.size - sf::Vector2f(16, 16));
            obstacles.push_back(obs);
        }
    }
    
    // --- Generate scattered obstacles ---
    std::uniform_int_distribution<int> numScatteredDist(config.minObstacles, config.maxObstacles);
    int numScattered = numScatteredDist(m_rng);
    std::uniform_int_distribution<int> texRand(0, 2);
    
    for (int i = 0; i < numScattered; ++i) {
        sf::Vector2f pos;
        bool valid = false;
        int attempts = 0;
        while (!valid && attempts < 50) {
            pos = getRandomPosition(40, 760, 40, 560);
            valid = isPositionValid(pos, obstacles);
            attempts++;
        }
        if (!valid) continue;
        
        Obstacle obs;
        obs.position = pos;
        obs.sprite = std::make_shared<sf::Sprite>(*textures[texRand(m_rng)]);
        sf::FloatRect lb = obs.sprite->getLocalBounds();
        obs.sprite->setOrigin({lb.size.x / 2.0f, lb.size.y / 2.0f});
        obs.sprite->setPosition(pos);
        obs.bounds = obs.sprite->getGlobalBounds();
        obs.bounds = sf::FloatRect(obs.bounds.position + sf::Vector2f(8, 8), obs.bounds.size - sf::Vector2f(16, 16));
        obstacles.push_back(obs);
    }
    
    // --- Generate enemies with level scaling ---
    std::uniform_int_distribution<int> numEnemiesDist(config.minEnemies, config.maxEnemies);
    int numEnemies = numEnemiesDist(m_rng);
    std::uniform_int_distribution<int> enemyTypeIdx(0, static_cast<int>(config.possibleEnemies.size()) - 1);
    
    for (int i = 0; i < numEnemies; ++i) {
        sf::Vector2f pos;
        bool valid = false;
        int attempts = 0;
        while (!valid && attempts < 100) {
            pos = getRandomPosition(50, 750, 50, 550);
            float dp = std::sqrt((pos.x - 400.0f) * (pos.x - 400.0f) + (pos.y - 300.0f) * (pos.y - 300.0f));
            if (dp > 150.0f && isPositionValid(pos, obstacles)) valid = true;
            attempts++;
        }
        
        if (valid) {
            EnemyType type = config.possibleEnemies[enemyTypeIdx(m_rng)];
            auto enemy = std::make_unique<Enemy>(type, levelNumber);
            enemy->init(pos);
            enemies.push_back(std::move(enemy));
        }
    }
}

RoomConfig LevelGenerator::getConfigForLevel(int levelNumber) {
    RoomConfig config;
    config.minEnemies = 4 + levelNumber * 2;
    config.maxEnemies = 6 + levelNumber * 2;
    config.minObstacles = 6 + levelNumber;
    config.maxObstacles = 10 + levelNumber * 2;
    
    // Level 1: only SIMPLE (Vampires1)
    // Level 2: SIMPLE + FAST (Vampires2)  
    // Level 3: SIMPLE + FAST + TANK (Vampires3)
    config.possibleEnemies.push_back(EnemyType::SIMPLE);
    if (levelNumber >= 2) config.possibleEnemies.push_back(EnemyType::FAST);
    if (levelNumber >= 3) config.possibleEnemies.push_back(EnemyType::TANK);
    
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
    for (const auto& obs : obstacles) {
        float dx = obs.position.x - pos.x;
        float dy = obs.position.y - pos.y;
        if (std::sqrt(dx*dx + dy*dy) < 35.0f) return false;
    }
    
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    if (std::sqrt(dx*dx + dy*dy) < 80.0f) return false;
    
    return true;
}
