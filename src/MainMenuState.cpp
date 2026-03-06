#include "MainMenuState.h"
#include "StateStack.h"

MainMenuState::MainMenuState(State::Context context)
: State(context)
{
    m_menu.init();
    m_menu.createMainMenu(
        [this]() { 
            requestStackPush(States::LevelSelect);
        },
        [this]() { 
            requestStateClear();
        }
    );
}

void MainMenuState::draw() {
    sf::RenderWindow& window = *getContext().window;
    m_menu.render(window);
}

bool MainMenuState::update(float dt) {
    m_menu.update();
    return true;
}

bool MainMenuState::handleEvent(const sf::Event& event) {
    m_menu.handleEvent(event, *getContext().window);
    return false;
}
