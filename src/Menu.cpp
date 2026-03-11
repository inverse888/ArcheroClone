#include "Menu.h"
#include "Config.h"
#include <iostream>

Menu::Menu() : m_selectedButton(0), m_mode(Mode::Main), m_soundEnabled(true) {
}

bool Menu::init() {
    if (!m_font.openFromFile(Config::FONT_MAIN)) {
        if (!m_font.openFromFile("assets/fonts/Helvetica.ttc")) {
            std::cout << "Warning: Font not found in Menu" << std::endl;
        }
    }

    if (m_bgTexture.loadFromFile(Config::MAIN_MENU_BG)) {
        m_bgSprite = std::make_unique<sf::Sprite>(m_bgTexture);
        sf::Vector2u sz = m_bgTexture.getSize();
        if (sz.x > 0 && sz.y > 0) {
            m_bgSprite->setScale({800.0f / static_cast<float>(sz.x), 600.0f / static_cast<float>(sz.y)});
        }
        m_hasBg = true;
    }
    return true;
}

void Menu::styleButton(sf::Text& btn, bool selected) {
    btn.setOutlineColor(sf::Color::Black);
    btn.setOutlineThickness(2);
    if (selected) {
        btn.setFillColor(sf::Color(255, 165, 0));
        btn.setScale({1.2f, 1.2f});
    } else {
        btn.setFillColor(sf::Color::White);
        btn.setScale({1.0f, 1.0f});
    }
}

void Menu::createMainMenu(std::function<void()> onPlay,
                          std::function<void()> onToggleSound,
                          std::function<void()> onExit,
                          bool soundEnabled) {
    m_buttons.clear();
    m_actions.clear();
    m_mode = Mode::Main;
    m_soundEnabled = soundEnabled;
    
    sf::Text playButton(m_font, "PLAY GAME", 50);
    sf::FloatRect b = playButton.getLocalBounds();
    playButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    playButton.setPosition({400.0f, 340.0f});
    styleButton(playButton, false);
    m_buttons.push_back(playButton);
    m_actions.push_back(onPlay);
    
    sf::Text soundButton(m_font, "", 44);
    b = soundButton.getLocalBounds();
    soundButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    soundButton.setPosition({400.0f, 430.0f});
    styleButton(soundButton, false);
    m_buttons.push_back(soundButton);
    m_actions.push_back([this, onToggleSound]() {
        m_soundEnabled = !m_soundEnabled;
        if (onToggleSound) onToggleSound();
        updateMainMenuSoundLabel();
    });

    sf::Text exitButton(m_font, "EXIT", 50);
    b = exitButton.getLocalBounds();
    exitButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    exitButton.setPosition({400.0f, 520.0f});
    styleButton(exitButton, false);
    m_buttons.push_back(exitButton);
    m_actions.push_back(onExit);

    updateMainMenuSoundLabel();
    
    m_selectedButton = 0;
    styleButton(m_buttons[0], true);
}

void Menu::createLevelSelectMenu(std::function<void(int)> onLevelSelected, std::function<void()> onBack) {
    m_buttons.clear();
    m_actions.clear();
    m_mode = Mode::LevelSelect;
    
    for (int i = 1; i <= 3; i++) {
        sf::Text levelButton(m_font, "LEVEL " + std::to_string(i), 40);
        sf::FloatRect b = levelButton.getLocalBounds();
        levelButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
        levelButton.setPosition({400.0f, 150.0f + static_cast<float>(i) * 80.0f});
        styleButton(levelButton, false);
        m_buttons.push_back(levelButton);
        m_actions.push_back([onLevelSelected, i]() { onLevelSelected(i); });
    }
    
    sf::Text backButton(m_font, "BACK", 40);
    sf::FloatRect b = backButton.getLocalBounds();
    backButton.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    backButton.setPosition({400.0f, 500.0f});
    styleButton(backButton, false);
    m_buttons.push_back(backButton);
    m_actions.push_back(onBack);
    
    m_selectedButton = 0;
    styleButton(m_buttons[0], true);
}

void Menu::updateMainMenuSoundLabel() {
    if (m_mode != Mode::Main || m_buttons.size() < 2) return;

    m_buttons[1].setString(m_soundEnabled ? "SOUND: ON" : "SOUND: OFF");
    sf::FloatRect b = m_buttons[1].getLocalBounds();
    m_buttons[1].setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
    m_buttons[1].setPosition({400.0f, 430.0f});
}

void Menu::update() {
}

void Menu::render(sf::RenderWindow& window) {
    if (m_mode == Mode::Main && m_hasBg && m_bgSprite) {
        window.draw(*m_bgSprite);
    } else if (m_mode == Mode::LevelSelect) {
        sf::RectangleShape bg({800, 600});
        bg.setFillColor(sf::Color(20, 15, 30));
        window.draw(bg);

        sf::Text title(m_font, "SELECT LEVEL", 55);
        title.setFillColor(sf::Color::White);
        title.setOutlineColor(sf::Color::Black);
        title.setOutlineThickness(2);
        sf::FloatRect b = title.getLocalBounds();
        title.setOrigin({b.size.x / 2.0f, b.size.y / 2.0f});
        title.setPosition({400, 80});
        window.draw(title);
    }

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
    
    styleButton(m_buttons[m_selectedButton], false);
    m_selectedButton = (m_selectedButton - 1 + m_buttons.size()) % m_buttons.size();
    styleButton(m_buttons[m_selectedButton], true);
}

void Menu::moveDown() {
    if (m_buttons.empty()) return;
    
    styleButton(m_buttons[m_selectedButton], false);
    m_selectedButton = (m_selectedButton + 1) % m_buttons.size();
    styleButton(m_buttons[m_selectedButton], true);
}

void Menu::activate() {
    if (!m_actions.empty() && m_selectedButton < m_actions.size()) {
        m_actions[m_selectedButton]();
    }
}
