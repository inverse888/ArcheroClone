#include "GameState.h"
#include "StateStack.h"
#include "SoundManager.h"

int GameState::selectedLevel = 1;

GameState::GameState(State::Context context)
: State(context)
, m_stateChanged(false)
{
    m_level.init(selectedLevel);
}

void GameState::draw() {
    m_level.render(*getContext().window);
}

bool GameState::update(float dt) {
    if (m_stateChanged) return true;
    
    m_level.update(dt);
    
    if (m_level.isCompleted()) {
        requestStackPush(States::Victory);
        m_stateChanged = true;
        SoundManager::getInstance().playSound(SoundID::Victory);
    } else if (m_level.isGameOver()) {
        requestStackPush(States::GameOver);
        m_stateChanged = true;
    }
    
    return true;
}

bool GameState::handleEvent(const sf::Event& event) {
    m_level.handleEvent(event, *getContext().window);
    
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
            requestStackPop();
        }
    }
    
    return false;
}
