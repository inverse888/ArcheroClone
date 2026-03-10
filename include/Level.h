#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

struct Obstacle {
    sf::Vector2f position;
    std::shared_ptr<sf::Sprite> sprite;
    sf::FloatRect bounds;
};

struct HeartBonus {
    sf::CircleShape shape;
    sf::Vector2f position;
    bool active;
    float lifetime;
    static constexpr float MAX_LIFETIME = 3.0f;
};

class Level {
private:
    int m_levelNumber;
    bool m_completed;
    bool m_gameOver;
    
    Player m_player;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    std::vector<Obstacle> m_obstacles;
    std::vector<HeartBonus> m_heartBonuses;
    
    int m_enemiesKilled;
    int m_totalEnemies;
    
    bool m_heartDropped;
    int m_heartDropAtKill;
    
    sf::Font m_font;
    sf::Texture m_heartTexture;
    sf::Sprite m_heartSprite;
    bool m_hasHeartTexture;
    
    sf::Texture m_bushTexture;
    sf::Texture m_cactusTexture;
    sf::Texture m_treeTexture;
    sf::Texture m_floorTexture;
    sf::Sprite m_floorSprite;
    
    std::unique_ptr<sf::Text> m_levelText;
    std::unique_ptr<sf::Text> m_healthText;
    std::unique_ptr<sf::Text> m_enemiesText;
    
    bool m_mousePressed;
    float m_contactDamageCooldown;
    
public:
    Level();
    ~Level() = default;
    
    bool init(int levelNumber);
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    
    bool isCompleted() const { return m_completed; }
    bool isGameOver() const { return m_gameOver; }
    int getLevelNumber() const { return m_levelNumber; }
    
private:
    void checkCollisions();
    void updateBonuses(float deltaTime);
    void resolveEnemyOverlap();
    void spawnHeartBonus(const sf::Vector2f& position);
    
    bool checkObstacleCollision(const sf::FloatRect& bounds) const;
};

#endif
