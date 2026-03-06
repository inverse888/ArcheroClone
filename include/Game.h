#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include "StateStack.h"

class Game {
private:
    sf::RenderWindow m_window;
    sf::Font m_font;
    StateStack m_stateStack;
    
public:
    Game();
    ~Game() = default;
    
    bool init();
    void run();
    
private:
    void processInput();
    void update(float dt);
    void render();
    
    void registerStates();
};

#endif
