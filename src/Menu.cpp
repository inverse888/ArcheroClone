#include "Menu.h"
#include <iostream>

Menu::Menu() : m_selectedButton(0) {
}

bool Menu::init() {
    if (!m_font.openFromFile("assets/fonts/Helvetica.ttc")) {
        if (!m_font.openFromFile("/Library/Fonts/Arial.ttf")) {
            std::cout << "Warning: Font not found in Menu" << std::endl;
        }
    }
    return true;
}

void Menu::createMainMenu(std::function<void()> onPlay, std::function<void()> onExit) {
    m_buttons.clear();
    m_actions.clear();
    
    sf::Text playButton(m_font, "PLAY GAME", 50);
    playButton.setFillColor(sf::Color::White);
    sf::FloatRect b = playButton.getLocalBounds();
    playButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    playButton.setPosition({400.0f, 250.0f});
    m_buttons.push_back(playButton);
    m_actions.push_back(onPlay);
    
    sf::Text exitButton(m_font, "EXIT", 50);
    exitButton.setFillColor(sf::Color::White);
    b = exitButton.getLocalBounds();
    exitButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    exitButton.setPosition({400.0f, 350.0f});
    m_buttons.push_back(exitButton);
    m_actions.push_back(onExit);
    
    m_selectedButton = 0;
    m_buttons[0].setFillColor(sf::Color::Yellow);
    m_buttons[0].setScale({1.2f, 1.2f});
}

void Menu::createLevelSelectMenu(std::function<void(int)> onLevelSelected, std::function<void()> onBack) {
    m_buttons.clear();
    m_actions.clear();
    
    for (int i = 1; i <= 3; i++) {
        sf::Text levelButton(m_font, "LEVEL " + std::to_string(i), 40);
        levelButton.setFillColor(sf::Color::White);
        sf::FloatRect b = levelButton.getLocalBounds();
        levelButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
        levelButton.setPosition({400.0f, 150.0f + static_cast<float>(i) * 80.0f});
        m_buttons.push_back(levelButton);
        m_actions.push_back([onLevelSelected, i]() { onLevelSelected(i); });
    }
    
    sf::Text backButton(m_font, "BACK", 40);
    backButton.setFillColor(sf::Color::White);
    sf::FloatRect b = backButton.getLocalBounds();
    backButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    backButton.setPosition({400.0f, 500.0f});
    m_buttons.push_back(backButton);
    m_actions.push_back(onBack);
    
    m_selectedButton = 0;
    m_buttons[0].setFillColor(sf::Color::Yellow);
    m_buttons[0].setScale({1.2f, 1.2f});
}

void Menu::setSoundToggleCallback(std::function<void()> callback) {
}

void Menu::update() {
}

void Menu::render(sf::RenderWindow& window) {
    // Рисуем заголовок или фон если нужно
    sf::Text title(m_font, "ARCHERO CLONE", 70);
    title.setFillColor(sf::Color::Red);
    sf::FloatRect b = title.getLocalBounds();
    title.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    title.setPosition({400, 100});
    window.draw(title);

    for (auto& button : m_buttons) {
        window.draw(button);
    }
}

void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (auto keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyEvent->scancode) {
            case sf::Keyboard::Scan::W:
            case sf::Keyboard::Scan::Up:
                moveUp();
                break;
                
            case sf::Keyboard::Scan::S:
            case sf::Keyboard::Scan::Down:
                moveDown();
                break;
                
            case sf::Keyboard::Scan::Enter:
            case sf::Keyboard::Scan::Space:
                activate();
                break;
                
            default:
                break;
        }
    }
}

void Menu::moveUp() {
    if (m_buttons.empty()) return;
    
    m_buttons[m_selectedButton].setFillColor(sf::Color::White);
    m_buttons[m_selectedButton].setScale({1.0f, 1.0f});
    m_selectedButton = (m_selectedButton - 1 + m_buttons.size()) % m_buttons.size();
    m_buttons[m_selectedButton].setFillColor(sf::Color::Yellow);
    m_buttons[m_selectedButton].setScale({1.2f, 1.2f});
}

void Menu::moveDown() {
    if (m_buttons.empty()) return;
    
    m_buttons[m_selectedButton].setFillColor(sf::Color::White);
    m_buttons[m_selectedButton].setScale({1.0f, 1.0f});
    m_selectedButton = (m_selectedButton + 1) % m_buttons.size();
    m_buttons[m_selectedButton].setFillColor(sf::Color::Yellow);
    m_buttons[m_selectedButton].setScale({1.2f, 1.2f});
}

void Menu::activate() {
    if (!m_actions.empty() && m_selectedButton < m_actions.size()) {
        m_actions[m_selectedButton]();
    }
}
