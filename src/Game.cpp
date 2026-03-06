#include "Game.h"
#include "MainMenuState.h"
#include "LevelSelectState.h"
#include "GameState.h"
#include "GameOverState.h"
#include "VictoryState.h"
#include <iostream>

Game::Game() 
: m_window(sf::VideoMode({800u, 600u}), "Archero Clone")
, m_stateStack(State::Context(m_window, m_font, m_stateStack))
{
    // Explicitly set view for Mac Retina displays
    sf::View view(sf::FloatRect({0, 0}, {800, 600}));
    m_window.setView(view);
    m_window.setFramerateLimit(60);
}

bool Game::init() {
    // Загружаем шрифт
    if (!m_font.openFromFile("assets/fonts/Helvetica.ttc")) {
        if (!m_font.openFromFile("/Library/Fonts/Arial.ttf")) {
            std::cout << "Error: Font not found" << std::endl;
            return false;
        }
    }
    
    registerStates();
    m_stateStack.pushState(States::MainMenu);
    
    return true;
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // Cap delta time
        
        processInput();
        update(dt);
        render();
        
        if (m_stateStack.isEmpty()) {
            m_window.close();
        }
    }
}

void Game::processInput() {
    while (auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        m_stateStack.handleEvent(*event);
    }
}

void Game::update(float dt) {
    m_stateStack.update(dt);
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    m_stateStack.draw();
    m_window.display();
}

void Game::registerStates() {
    m_stateStack.registerState<MainMenuState>(States::MainMenu);
    m_stateStack.registerState<LevelSelectState>(States::LevelSelect);
    m_stateStack.registerState<GameState>(States::Game);
    m_stateStack.registerState<GameOverState>(States::GameOver);
    m_stateStack.registerState<VictoryState>(States::Victory);
}
