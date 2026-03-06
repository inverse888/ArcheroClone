#include "LevelSelectState.h"
#include "StateStack.h"
#include "GameState.h"

LevelSelectState::LevelSelectState(State::Context context)
: State(context)
{
    m_menu.init();
    m_menu.createLevelSelectMenu(
        [this](int level) { 
            GameState::selectedLevel = level;
            requestStackPop();
            requestStackPush(States::Game);
        },
        [this]() { 
            requestStackPop();
        }
    );
}

void LevelSelectState::draw() {
    sf::RenderWindow& window = *getContext().window;
    m_menu.render(window);
}

bool LevelSelectState::update(float dt) {
    m_menu.update();
    return true;
}

bool LevelSelectState::handleEvent(const sf::Event& event) {
    m_menu.handleEvent(event, *getContext().window);
    
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            requestStackPop();
        }
    }
    
    return false;
}
