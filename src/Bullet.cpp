#include "Bullet.h"
#include "Config.h"
#include <cmath>

Bullet::Bullet(const sf::Vector2f& startPos, const sf::Vector2f& dir) 
    : m_position(startPos), m_direction(dir), m_speed(500.0f), m_damage(1), m_active(true), m_hasTexture(false) {
    
    if (m_texture.loadFromFile(Config::BULLET_SPRITE)) {
        m_sprite = std::make_unique<sf::Sprite>(m_texture);
        sf::FloatRect bounds = m_sprite->getLocalBounds();
        m_sprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        m_sprite->setPosition(m_position);
        m_hasTexture = true;
    } else {
        m_shape.setRadius(5.0f);
        m_shape.setFillColor(sf::Color::Yellow);
        m_shape.setOrigin({5.0f, 5.0f});
        m_shape.setPosition(m_position);
        m_hasTexture = false;
    }

    // Нормализуем направление, если оно не нулевое
    float length = std::sqrt(m_direction.x * m_direction.x + m_direction.y * m_direction.y);
    if (length > 0.0f) {
        m_direction /= length;
    } else {
        // Если направление нулевое, стреляем вверх по умолчанию
        m_direction = {0.0f, -1.0f};
    }
}

void Bullet::update(float deltaTime) {
    if (!m_active) return;
    
    m_position += m_direction * m_speed * deltaTime;
    if (m_hasTexture && m_sprite) m_sprite->setPosition(m_position);
    else m_shape.setPosition(m_position);
    
    // Деактивируем, если вылетела за пределы экрана
    if (m_position.x < 0 || m_position.x > 800 || 
        m_position.y < 0 || m_position.y > 600) {
        m_active = false;
    }
}

void Bullet::render(sf::RenderWindow& window) {
    if (m_active) {
        if (m_hasTexture && m_sprite) window.draw(*m_sprite);
        else window.draw(m_shape);
    }
}

sf::FloatRect Bullet::getBounds() const {
    if (m_hasTexture && m_sprite) return m_sprite->getGlobalBounds();
    return m_shape.getGlobalBounds();
}
