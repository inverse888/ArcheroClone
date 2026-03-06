#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class Menu {
private:
    sf::Font m_font;
    std::vector<sf::Text> m_buttons;
    std::vector<std::function<void()>> m_actions;
    int m_selectedButton;
    
public:
    Menu();
    ~Menu() = default;
    
    bool init();
    void update();
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    
    void createMainMenu(std::function<void()> onPlay, std::function<void()> onExit);
    void createLevelSelectMenu(std::function<void(int)> onLevelSelected, std::function<void()> onBack);
    
    void setSoundToggleCallback(std::function<void()> callback);
    
private:
    void moveUp();
    void moveDown();
    void activate();
};

#endif