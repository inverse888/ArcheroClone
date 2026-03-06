#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#include <vector>
#include <random>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Enemy.h"
#include "Level.h"

struct RoomConfig {
    int minEnemies;
    int maxEnemies;
    int minObstacles;
    int maxObstacles;
    std::vector<EnemyType> possibleEnemies;
};

class LevelGenerator {
private:
    std::mt19937 m_rng;
    
public:
    LevelGenerator();
    ~LevelGenerator() = default;
    
    void generateLevel(int levelNumber, 
                      std::vector<std::unique_ptr<Enemy>>& enemies,
                      std::vector<Obstacle>& obstacles,
                      const sf::Texture& bush, const sf::Texture& cactus, const sf::Texture& tree);
    
private:
    RoomConfig getConfigForLevel(int levelNumber);
    sf::Vector2f getRandomPosition(float minX, float maxX, float minY, float maxY);
    bool isPositionValid(const sf::Vector2f& pos, 
                        const std::vector<Obstacle>& obstacles,
                        const sf::Vector2f& playerPos = {400, 300});
};

#endif