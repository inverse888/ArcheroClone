#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
private:
    std::unique_ptr<sf::Sprite> m_sprite;
    sf::Texture m_texture;
    sf::CircleShape m_shape;
    bool m_hasTexture;
    sf::Vector2f m_position;
    sf::Vector2f m_direction;
    float m_speed;
    int m_damage;
    bool m_active;
    
public:
    Bullet(const sf::Vector2f& startPos, const sf::Vector2f& dir);
    ~Bullet() = default;
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    sf::FloatRect getBounds() const;
    const sf::Vector2f& getPosition() const { return m_position; }
    int getDamage() const { return m_damage; }
    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }
};

#endif