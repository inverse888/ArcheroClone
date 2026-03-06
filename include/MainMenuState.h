#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "State.h"
#include "Menu.h"

class MainMenuState : public State {
public:
    MainMenuState(State::Context context);
    virtual void draw() override;
    virtual bool update(float dt) override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    Menu m_menu;
};

#endif
