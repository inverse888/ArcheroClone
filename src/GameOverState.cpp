#include "GameOverState.h"
#include "StateStack.h"

GameOverState::GameOverState(State::Context context)
: State(context)
, m_gameOverText(*context.font, "GAME OVER", 60)
, m_instructionText(*context.font, "Press ESC to return to menu", 30)
{
    m_gameOverText.setFillColor(sf::Color::Red);
    sf::FloatRect bounds = m_gameOverText.getLocalBounds();
    m_gameOverText.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    m_gameOverText.setPosition({400, 250});
    
    m_instructionText.setFillColor(sf::Color::White);
    bounds = m_instructionText.getLocalBounds();
    m_instructionText.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    m_instructionText.setPosition({400, 350});
}

void GameOverState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    // Затемнение фона
    sf::RectangleShape overlay({800, 600});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    window.draw(m_gameOverText);
    window.draw(m_instructionText);
}

bool GameOverState::update(float dt) {
    return false; // Останавливаем обновление состояний под нами
}

bool GameOverState::handleEvent(const sf::Event& event) {
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape || 
            keyEvent->scancode == sf::Keyboard::Scancode::Enter) {
            requestStateClear();
            requestStackPush(States::MainMenu);
        }
    }
    return false;
}
