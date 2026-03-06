#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "State.h"
#include "Level.h"

class GameState : public State {
public:
    static int selectedLevel;

public:
    GameState(State::Context context);
    virtual void draw() override;
    virtual bool update(float dt) override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    Level m_level;
    bool m_stateChanged;
};

#endif
