#ifndef VICTORY_STATE_H
#define VICTORY_STATE_H

#include "State.h"

class VictoryState : public State {
public:
    VictoryState(State::Context context);
    virtual void draw() override;
    virtual bool update(float dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    virtual bool isTransparent() const override { return true; }

private:
    sf::Text m_victoryText;
    sf::Text m_instructionText;
};

#endif
