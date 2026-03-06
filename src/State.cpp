#include "State.h"
#include "StateStack.h"

State::Context::Context(sf::RenderWindow& window, sf::Font& font, StateStack& stack)
: window(&window)
, font(&font)
, stack(&stack)
{
}

State::State(Context context)
: m_context(context)
{
}

State::~State()
{
}

void State::requestStackPush(States::ID stateID)
{
    m_context.stack->pushState(stateID);
}

void State::requestStackPop()
{
    m_context.stack->popState();
}

void State::requestStateClear()
{
    m_context.stack->clearStates();
}

State::Context State::getContext() const
{
    return m_context;
}
