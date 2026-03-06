#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "State.h"

class GameOverState : public State {
public:
    GameOverState(State::Context context);
    virtual void draw() override;
    virtual bool update(float dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    virtual bool isTransparent() const override { return true; }

private:
    sf::Text m_gameOverText;
    sf::Text m_instructionText;
};

#endif
