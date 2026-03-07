#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class Menu {
private:
    enum class Mode {
        Main,
        LevelSelect
    };

    sf::Font m_font;
    std::vector<sf::Text> m_buttons;
    std::vector<std::function<void()>> m_actions;
    int m_selectedButton;
    Mode m_mode;
    bool m_soundEnabled;
    
public:
    Menu();
    ~Menu() = default;
    
    bool init();
    void update();
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    
    void createMainMenu(std::function<void()> onPlay,
                        std::function<void()> onToggleSound,
                        std::function<void()> onExit,
                        bool soundEnabled);
    void createLevelSelectMenu(std::function<void(int)> onLevelSelected, std::function<void()> onBack);
    
private:
    void updateMainMenuSoundLabel();
    void moveUp();
    void moveDown();
    void activate();
};

#endif
