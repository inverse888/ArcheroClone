#include "StateStack.h"
#include <cassert>

StateStack::StateStack(State::Context context)
: m_stack()
, m_pendingList()
, m_context(context)
, m_factories()
{
}

void StateStack::update(float dt)
{
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
    {
        if (!(*itr)->update(dt))
            break;
    }

    applyPendingChanges();
}

void StateStack::draw()
{
    // Draw from the bottom up to the top, but skip if states above are not transparent
    // To do this properly, we find the first non-transparent state from top to bottom
    int firstVisibleState = m_stack.size() - 1;
    for (; firstVisibleState > 0; --firstVisibleState) {
        if (!m_stack[firstVisibleState]->isTransparent()) {
            break;
        }
    }

    for (int i = firstVisibleState; i < m_stack.size(); ++i) {
        m_stack[i]->draw();
    }
}

void StateStack::handleEvent(const sf::Event& event)
{
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
    {
        if (!(*itr)->handleEvent(event))
            break;
    }

    applyPendingChanges();
}

void StateStack::pushState(States::ID stateID)
{
    m_pendingList.push_back(PendingChange(Push, stateID));
}

void StateStack::popState()
{
    m_pendingList.push_back(PendingChange(Pop));
}

void StateStack::clearStates()
{
    m_pendingList.push_back(PendingChange(Clear));
}

bool StateStack::isEmpty() const
{
    return m_stack.empty();
}

State::Ptr StateStack::createState(States::ID stateID)
{
    auto found = m_factories.find(stateID);
    assert(found != m_factories.end());

    return found->second();
}

void StateStack::applyPendingChanges()
{
    for (PendingChange change : m_pendingList)
    {
        switch (change.action)
        {
            case Push:
                m_stack.push_back(createState(change.stateID));
                break;

            case Pop:
                m_stack.pop_back();
                break;

            case Clear:
                m_stack.clear();
                break;
        }
    }

    m_pendingList.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
: action(action)
, stateID(stateID)
{
}
