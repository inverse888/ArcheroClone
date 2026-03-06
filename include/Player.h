#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>
#include "Bullet.h"
#include "Animation.h"

enum class PlayerDirection {
    Down,
    Up,
    Left,
    Right
};

enum class PlayerState {
    Idle,
    Running,
    Attacking,
    Hurt,
    Dead
};

class Player {
private:
    std::unique_ptr<sf::Sprite> m_sprite;
    sf::Texture m_idleTexture;
    sf::Texture m_runTexture;
    sf::Texture m_attackTexture;
    sf::Texture m_hurtTexture;
    sf::Texture m_deathTexture;
    
    sf::Vector2f m_position;
    float m_speed;
    int m_health;
    int m_maxHealth;
    float m_shootCooldown;
    float m_shootTimer;
    
    std::map<PlayerState, std::map<PlayerDirection, Animation>> m_animations;
    PlayerDirection m_currentDirection;
    PlayerState m_currentState;
    bool m_isMoving;

    bool m_upPressed;
    bool m_downPressed;
    bool m_leftPressed;
    bool m_rightPressed;
    
    float m_stateTimer; // Для временных состояний (Attack, Hurt)
    
public:
    Player();
    ~Player() = default;
    
    bool init();
    void update(float deltaTime, const std::vector<struct Obstacle>& obstacles);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    
    void takeDamage(int damage);
    void heal(int amount);
    
    sf::FloatRect getBounds() const;
    const sf::Vector2f& getPosition() const { return m_position; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    bool isAlive() const { return m_health > 0; }
    
    std::unique_ptr<Bullet> shoot(const sf::Vector2f& targetPos);
    void setPosition(float x, float y);
    
private:
    sf::Vector2f getDirectionFromInput() const;
    void updateAnimation(float deltaTime);
    void setState(PlayerState state);
};

#endif
