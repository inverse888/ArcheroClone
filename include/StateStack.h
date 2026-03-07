#ifndef STATE_STACK_H
#define STATE_STACK_H

#include "State.h"
#include <vector>
#include <map>
#include <functional>

class StateStack {
public:
    enum Action {
        Push,
        Pop,
        Clear,
    };

public:
    explicit StateStack(State::Context context);

    template <typename T>
    void registerState(States::ID stateID);
    
    
    template <typename T, typename Param>
    void registerState(States::ID stateID, Param param);

    void update(float dt);
    void draw();
    void handleEvent(const sf::Event& event);

    void pushState(States::ID stateID);
    void popState();
    void clearStates();

    bool isEmpty() const;

private:
    State::Ptr createState(States::ID stateID);
    void applyPendingChanges();

private:
    struct PendingChange {
        explicit PendingChange(Action action, States::ID stateID = States::None);
        Action action;
        States::ID stateID;
    };

private:
    std::vector<State::Ptr> m_stack;
    std::vector<PendingChange> m_pendingList;
    State::Context m_context;
    std::map<States::ID, std::function<State::Ptr()>> m_factories;
};

template <typename T>
void StateStack::registerState(States::ID stateID) {
    m_factories[stateID] = [this]() {
        return State::Ptr(new T(m_context));
    };
}

#endif
