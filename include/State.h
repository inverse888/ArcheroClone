#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>
#include <memory>

namespace States {
    enum ID {
        None,
        MainMenu,
        LevelSelect,
        Game,
        GameOver,
        Victory
    };
}

class StateStack;

class State {
public:
    typedef std::unique_ptr<State> Ptr;

    struct Context {
        Context(sf::RenderWindow& window, sf::Font& font, StateStack& stack);
        sf::RenderWindow* window;
        sf::Font* font;
        StateStack* stack;
    };

public:
    State(Context context);
    virtual ~State();

    virtual void draw() = 0;
    virtual bool update(float dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;
    virtual bool isTransparent() const { return false; }

protected:
    void requestStackPush(States::ID stateID);
    void requestStackPop();
    void requestStateClear();

    Context getContext() const;

private:
    Context m_context;
};

#endif
