#ifndef LEVEL_SELECT_STATE_H
#define LEVEL_SELECT_STATE_H

#include "State.h"
#include "Menu.h"

class LevelSelectState : public State {
public:
    LevelSelectState(State::Context context);
    virtual void draw() override;
    virtual bool update(float dt) override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    Menu m_menu;
};

#endif
