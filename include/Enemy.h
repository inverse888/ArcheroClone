#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <vector>
#include "Bullet.h"
#include "Animation.h"

enum class EnemyType {
    SIMPLE,
    FAST,
    TANK
};

enum class EnemyDirection {
    Down,
    Up,
    Left,
    Right
};

enum class EnemyState {
    Idle,
    Running
};

class Enemy {
private:
    EnemyType m_type;
    std::unique_ptr<sf::Sprite> m_sprite;
    sf::Texture m_idleTexture;
    sf::Texture m_runTexture;
    sf::CircleShape m_shape; // Fallback
    bool m_hasTexture;
    
    sf::Vector2f m_position;
    float m_speed;
    int m_health;
    int m_maxHealth;
    
    std::map<EnemyState, std::map<EnemyDirection, Animation>> m_animations;
    EnemyDirection m_currentDirection;
    EnemyState m_currentState;
    
public:
    Enemy(EnemyType type, int levelNumber = 1);
    ~Enemy() = default;
    
    void init(const sf::Vector2f& startPos);
    void update(float deltaTime, const sf::Vector2f& playerPos, const std::vector<struct Obstacle>& obstacles);
    void render(sf::RenderWindow& window);
    
    void takeDamage(int damage);
    
    sf::FloatRect getBounds() const;
    const sf::Vector2f& getPosition() const { return m_position; }
    void setPosition(const sf::Vector2f& position);
    int getHealth() const { return m_health; }
    bool isAlive() const { return m_health > 0; }
    EnemyType getType() const { return m_type; }
    
private:
    void moveTowards(float deltaTime, const sf::Vector2f& target, const std::vector<struct Obstacle>& obstacles);
};

#endif
