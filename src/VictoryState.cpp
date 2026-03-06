#include "VictoryState.h"
#include "StateStack.h"
#include "GameState.h"

VictoryState::VictoryState(State::Context context)
: State(context)
, m_victoryText(*context.font, "VICTORY!", 60)
, m_instructionText(*context.font, "", 30)
{
    m_victoryText.setFillColor(sf::Color::Green);
    sf::FloatRect bounds = m_victoryText.getLocalBounds();
    m_victoryText.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    m_victoryText.setPosition({400, 250});
    
    std::string instruction = (GameState::selectedLevel < 3) ? 
        "Level Complete! Press Enter for next level" : 
        "All levels complete! Press Enter to menu";
        
    m_instructionText.setString(instruction);
    m_instructionText.setFillColor(sf::Color::White);
    bounds = m_instructionText.getLocalBounds();
    m_instructionText.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    m_instructionText.setPosition({400, 350});
}

void VictoryState::draw() {
    sf::RenderWindow& window = *getContext().window;
    
    sf::RectangleShape overlay({800, 600});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    window.draw(m_victoryText);
    window.draw(m_instructionText);
}

bool VictoryState::update(float dt) {
    return false;
}

bool VictoryState::handleEvent(const sf::Event& event) {
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Enter || 
            keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            
            if (GameState::selectedLevel < 3 && keyEvent->scancode == sf::Keyboard::Scancode::Enter) {
                GameState::selectedLevel++;
                requestStackPop(); // Убираем VictoryState
                requestStackPop(); // Убираем текущий GameState
                requestStackPush(States::Game); // Пушим новый GameState
            } else {
                requestStateClear();
                requestStackPush(States::MainMenu);
            }
        }
    }
    return false;
}
