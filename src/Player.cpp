#include "Player.h"
#include "Level.h"
#include "Config.h"
#include <cmath>
#include <iostream>

Player::Player() : m_sprite(nullptr), 
                   m_speed(200.0f), 
                   m_health(5), 
                   m_maxHealth(5), 
                   m_shootCooldown(1.0f), 
                   m_shootTimer(0.0f),
                   m_currentDirection(PlayerDirection::Down),
                   m_currentState(PlayerState::Idle),
                   m_isMoving(false),
                   m_upPressed(false),
                   m_downPressed(false),
                   m_leftPressed(false),
                   m_rightPressed(false) {
}

bool Player::init() {
    if (!m_idleTexture.loadFromFile(Config::PLAYER_IDLE) || 
        !m_runTexture.loadFromFile(Config::PLAYER_RUN) ||
        !m_attackTexture.loadFromFile(Config::PLAYER_ATTACK) ||
        !m_hurtTexture.loadFromFile(Config::PLAYER_HURT) ||
        !m_deathTexture.loadFromFile(Config::PLAYER_DEATH)) {
        std::cout << "Failed to load player textures" << std::endl;
        return false;
    }
    
    const int FW = 64;
    const int FH = 64;
    
    m_sprite = std::make_unique<sf::Sprite>(m_idleTexture);
    // SET TEXTURE RECT TO ONE FRAME *BEFORE* COMPUTING ORIGIN
    m_sprite->setTextureRect(sf::IntRect({0, 0}, {FW, FH}));
    m_sprite->setOrigin({FW / 2.0f, FH / 2.0f});
    
    // Sprite sheet rows: 0=Down, 1=Up, 2=Right, 3=Left
    for (int row = 0; row < 4; ++row) {
        PlayerDirection pDir;
        if (row == 0)      pDir = PlayerDirection::Down;
        else if (row == 1) pDir = PlayerDirection::Up;
        else if (row == 2) pDir = PlayerDirection::Right;
        else               pDir = PlayerDirection::Left;
        
        auto makeAnim = [&](int numFrames, float frameTime) {
            Animation a;
            a.setFrameTime(frameTime);
            for (int f = 0; f < numFrames; ++f)
                a.addFrame(sf::IntRect({f * FW, row * FH}, {FW, FH}));
            return a;
        };
        
        m_animations[PlayerState::Idle][pDir]      = makeAnim(12, 0.1f);
        m_animations[PlayerState::Running][pDir]    = makeAnim(8,  0.08f);
        m_animations[PlayerState::Attacking][pDir]  = makeAnim(8,  0.06f);
        m_animations[PlayerState::Hurt][pDir]       = makeAnim(5,  0.1f);
        m_animations[PlayerState::Dead][pDir]       = makeAnim(7,  0.15f);
    }
    
    m_position = {400.0f, 300.0f};
    m_sprite->setPosition(m_position);
    
    return true;
}

void Player::handleEvent(const sf::Event& event) {
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyEvent->scancode) {
            case sf::Keyboard::Scan::W: case sf::Keyboard::Scan::Up:    m_upPressed = true; break;
            case sf::Keyboard::Scan::S: case sf::Keyboard::Scan::Down:  m_downPressed = true; break;
            case sf::Keyboard::Scan::A: case sf::Keyboard::Scan::Left:  m_leftPressed = true; break;
            case sf::Keyboard::Scan::D: case sf::Keyboard::Scan::Right: m_rightPressed = true; break;
            default: break;
        }
    }
    
    if (auto keyEvent = event.getIf<sf::Event::KeyReleased>()) {
        switch (keyEvent->scancode) {
            case sf::Keyboard::Scan::W: case sf::Keyboard::Scan::Up:    m_upPressed = false; break;
            case sf::Keyboard::Scan::S: case sf::Keyboard::Scan::Down:  m_downPressed = false; break;
            case sf::Keyboard::Scan::A: case sf::Keyboard::Scan::Left:  m_leftPressed = false; break;
            case sf::Keyboard::Scan::D: case sf::Keyboard::Scan::Right: m_rightPressed = false; break;
            default: break;
        }
    }
}

void Player::update(float deltaTime, const std::vector<Obstacle>& obstacles) {
    sf::Vector2f input = {0.0f, 0.0f};
    if (m_upPressed)    input.y -= 1.0f;
    if (m_downPressed)  input.y += 1.0f;
    if (m_leftPressed)  input.x -= 1.0f;
    if (m_rightPressed) input.x += 1.0f;
    
    m_isMoving = false;
    float length = std::sqrt(input.x * input.x + input.y * input.y);
    if (length > 0.0f) {
        input /= length;
        m_isMoving = true;
        m_currentState = PlayerState::Running;
        
        if (std::abs(input.x) > std::abs(input.y)) {
            m_currentDirection = (input.x > 0) ? PlayerDirection::Right : PlayerDirection::Left;
        } else {
            m_currentDirection = (input.y > 0) ? PlayerDirection::Down : PlayerDirection::Up;
        }
    } else {
        if (m_currentState != PlayerState::Attacking || m_shootTimer <= 0.0f)
            m_currentState = PlayerState::Idle;
    }
    
    updateAnimation(deltaTime);
    
    sf::Vector2f nextPos = m_position + input * m_speed * deltaTime;
    
    const float MARGIN = 25.0f;
    nextPos.x = std::max(MARGIN, std::min(nextPos.x, 800.0f - MARGIN));
    nextPos.y = std::max(MARGIN, std::min(nextPos.y, 600.0f - MARGIN));
    
    const float HALF = 8.0f;
    sf::FloatRect colBox(nextPos - sf::Vector2f(HALF, HALF), {HALF * 2, HALF * 2});
    bool blocked = false;
    for (const auto& obs : obstacles) {
        if (colBox.findIntersection(obs.bounds)) { blocked = true; break; }
    }
    
    if (!blocked) {
        m_position = nextPos;
    } else {
        // Slide along X
        float nx = m_position.x + input.x * m_speed * deltaTime;
        nx = std::max(MARGIN, std::min(nx, 800.0f - MARGIN));
        sf::FloatRect bx(sf::Vector2f(nx - HALF, m_position.y - HALF), {HALF * 2, HALF * 2});
        bool bxHit = false;
        for (const auto& obs : obstacles) if (bx.findIntersection(obs.bounds)) { bxHit = true; break; }
        if (!bxHit) m_position.x = nx;

        // Slide along Y
        float ny = m_position.y + input.y * m_speed * deltaTime;
        ny = std::max(MARGIN, std::min(ny, 600.0f - MARGIN));
        sf::FloatRect by(sf::Vector2f(m_position.x - HALF, ny - HALF), {HALF * 2, HALF * 2});
        bool byHit = false;
        for (const auto& obs : obstacles) if (by.findIntersection(obs.bounds)) { byHit = true; break; }
        if (!byHit) m_position.y = ny;
    }
    
    if (m_sprite) m_sprite->setPosition(m_position);
    
    if (m_shootTimer > 0.0f) {
        m_shootTimer -= deltaTime;
        if (m_currentState == PlayerState::Attacking && m_shootTimer <= 0.0f)
            m_currentState = PlayerState::Idle;
    }
}

void Player::updateAnimation(float deltaTime) {
    if (!m_sprite) return;
    
    auto& anim = m_animations[m_currentState][m_currentDirection];
    anim.update(deltaTime);
    
    switch (m_currentState) {
        case PlayerState::Idle:      m_sprite->setTexture(m_idleTexture);   break;
        case PlayerState::Running:   m_sprite->setTexture(m_runTexture);    break;
        case PlayerState::Attacking: m_sprite->setTexture(m_attackTexture); break;
        case PlayerState::Hurt:      m_sprite->setTexture(m_hurtTexture);   break;
        case PlayerState::Dead:      m_sprite->setTexture(m_deathTexture);  break;
    }
    m_sprite->setTextureRect(anim.getCurrentFrame());
}

void Player::render(sf::RenderWindow& window) {
    if (m_sprite) window.draw(*m_sprite);
}

void Player::takeDamage(int damage) {
    m_health -= damage;
    if (m_health < 0) m_health = 0;
    if (m_sprite) m_sprite->setColor(sf::Color(255, 100, 100));
}

void Player::heal(int amount) {
    m_health += amount;
    if (m_health > m_maxHealth) m_health = m_maxHealth;
    if (m_sprite) m_sprite->setColor(sf::Color::White);
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(m_position - sf::Vector2f(15, 15), {30, 30});
}

std::unique_ptr<Bullet> Player::shoot(const sf::Vector2f& targetPos) {
    if (m_upPressed || m_downPressed || m_leftPressed || m_rightPressed) return nullptr;
    if (m_shootTimer > 0.0f) return nullptr;
    
    m_shootTimer = m_shootCooldown;
    m_currentState = PlayerState::Attacking;
    
    sf::Vector2f direction = targetPos - m_position;
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (len > 0.0f) direction /= len;
    else direction = {0.0f, -1.0f};
    
    if (std::abs(direction.x) > std::abs(direction.y))
        m_currentDirection = (direction.x > 0) ? PlayerDirection::Right : PlayerDirection::Left;
    else
        m_currentDirection = (direction.y > 0) ? PlayerDirection::Down : PlayerDirection::Up;
    
    return std::make_unique<Bullet>(m_position, direction);
}

void Player::setPosition(float x, float y) {
    m_position = {x, y};
    if (m_sprite) m_sprite->setPosition(m_position);
}
