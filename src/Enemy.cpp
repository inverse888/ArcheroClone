#include "Enemy.h"
#include "Level.h"
#include "Config.h"
#include <cmath>
#include <iostream>

Enemy::Enemy(EnemyType type) : m_type(type), m_hasTexture(false), m_health(5), m_maxHealth(5), m_speed(80.0f),
                               m_currentDirection(EnemyDirection::Down), m_currentState(EnemyState::Idle) {
    switch (type) {
        case EnemyType::SIMPLE:
            m_speed = 80.0f;
            m_health = 5;
            m_maxHealth = 5;
            break;
        case EnemyType::FAST:
            m_speed = 150.0f;
            m_health = 5;
            m_maxHealth = 5;
            break;
        case EnemyType::TANK:
            m_speed = 40.0f;
            m_health = 15;
            m_maxHealth = 15;
            break;
    }
}

void Enemy::init(const sf::Vector2f& startPos) {
    m_position = startPos;
    
    std::string idlePath, runPath;
    switch (m_type) {
        case EnemyType::SIMPLE: 
            idlePath = Config::ENEMY_SIMPLE_IDLE; 
            runPath = Config::ENEMY_SIMPLE_RUN;
            break;
        case EnemyType::FAST:   
            idlePath = Config::ENEMY_FAST_IDLE;
            runPath = Config::ENEMY_FAST_RUN;
            break;
        case EnemyType::TANK:   
            idlePath = Config::ENEMY_TANK_IDLE;
            runPath = Config::ENEMY_TANK_RUN;
            break;
    }

    if (m_idleTexture.loadFromFile(idlePath) && m_runTexture.loadFromFile(runPath)) {
        const int FW = 64;
        const int FH = 64;
        
        m_sprite = std::make_unique<sf::Sprite>(m_idleTexture);
        // SET TEXTURE RECT TO ONE FRAME *BEFORE* COMPUTING ORIGIN
        m_sprite->setTextureRect(sf::IntRect({0, 0}, {FW, FH}));
        m_sprite->setOrigin({FW / 2.0f, FH / 2.0f});
        m_sprite->setPosition(m_position);
        
        // Rows: 0=Down, 1=Up, 2=Right, 3=Left (same as player pack)
        for (int row = 0; row < 4; ++row) {
            EnemyDirection eDir;
            if (row == 0)      eDir = EnemyDirection::Down;
            else if (row == 1) eDir = EnemyDirection::Up;
            else if (row == 2) eDir = EnemyDirection::Right;
            else               eDir = EnemyDirection::Left;
            
            Animation idleAnim;
            idleAnim.setFrameTime(0.15f);
            for (int f = 0; f < 4; ++f)
                idleAnim.addFrame(sf::IntRect({f * FW, row * FH}, {FW, FH}));
            m_animations[EnemyState::Idle][eDir] = idleAnim;
            
            Animation runAnim;
            runAnim.setFrameTime(0.1f);
            for (int f = 0; f < 8; ++f)
                runAnim.addFrame(sf::IntRect({f * FW, row * FH}, {FW, FH}));
            m_animations[EnemyState::Running][eDir] = runAnim;
        }

        m_hasTexture = true;
    } else {
        switch (m_type) {
            case EnemyType::SIMPLE:
                m_shape.setRadius(15.0f);
                m_shape.setFillColor(sf::Color::Red);
                break;
            case EnemyType::FAST:
                m_shape.setRadius(12.0f);
                m_shape.setFillColor(sf::Color::Magenta);
                break;
            case EnemyType::TANK:
                m_shape.setRadius(25.0f);
                m_shape.setFillColor(sf::Color(139, 0, 0));
                break;
        }
        m_shape.setOrigin({m_shape.getRadius(), m_shape.getRadius()});
        m_shape.setPosition(m_position);
        m_hasTexture = false;
    }
}

void Enemy::update(float deltaTime, const sf::Vector2f& playerPos, const std::vector<Obstacle>& obstacles) {
    moveTowards(deltaTime, playerPos, obstacles);
    
    if (m_hasTexture && m_sprite) {
        auto& anim = m_animations[m_currentState][m_currentDirection];
        anim.update(deltaTime);
        m_sprite->setTexture(m_currentState == EnemyState::Idle ? m_idleTexture : m_runTexture);
        m_sprite->setTextureRect(anim.getCurrentFrame());
        m_sprite->setPosition(m_position);
    } else {
        m_shape.setPosition(m_position);
    }
}

void Enemy::moveTowards(float deltaTime, const sf::Vector2f& target, const std::vector<Obstacle>& obstacles) {
    sf::Vector2f direction = target - m_position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (length > 10.0f) {
        direction /= length;
        m_currentState = EnemyState::Running;
        
        if (std::abs(direction.x) > std::abs(direction.y))
            m_currentDirection = (direction.x > 0) ? EnemyDirection::Right : EnemyDirection::Left;
        else
            m_currentDirection = (direction.y > 0) ? EnemyDirection::Down : EnemyDirection::Up;
        
        const float HALF = 15.0f;
        sf::Vector2f nextPos = m_position + direction * m_speed * deltaTime;
        sf::FloatRect moveBounds(nextPos - sf::Vector2f(HALF, HALF), {HALF * 2, HALF * 2});
        
        bool collision = false;
        for (const auto& obs : obstacles)
            if (moveBounds.findIntersection(obs.bounds)) { collision = true; break; }
        
        if (!collision) {
            m_position = nextPos;
        } else {
            float nx = m_position.x + direction.x * m_speed * deltaTime;
            sf::FloatRect bx(sf::Vector2f(nx - HALF, m_position.y - HALF), {HALF * 2, HALF * 2});
            bool collX = false;
            for (const auto& obs : obstacles) if (bx.findIntersection(obs.bounds)) { collX = true; break; }
            if (!collX) m_position.x = nx;

            float ny = m_position.y + direction.y * m_speed * deltaTime;
            sf::FloatRect by(sf::Vector2f(m_position.x - HALF, ny - HALF), {HALF * 2, HALF * 2});
            bool collY = false;
            for (const auto& obs : obstacles) if (by.findIntersection(obs.bounds)) { collY = true; break; }
            if (!collY) m_position.y = ny;
        }
    } else {
        m_currentState = EnemyState::Idle;
    }
}

void Enemy::render(sf::RenderWindow& window) {
    if (m_hasTexture && m_sprite) window.draw(*m_sprite);
    else window.draw(m_shape);
}

void Enemy::takeDamage(int damage) {
    m_health -= damage;
    if (m_hasTexture && m_sprite) m_sprite->setColor(sf::Color(255, 100, 100));
    else m_shape.setFillColor(sf::Color::White);
}

sf::FloatRect Enemy::getBounds() const {
    return sf::FloatRect(m_position - sf::Vector2f(15, 15), {30, 30});
}
