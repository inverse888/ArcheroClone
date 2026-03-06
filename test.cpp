#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({200, 200}), "Test");
    
    while (window.isOpen()) {
        // В SFML 3 pollEvent() возвращает optional
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        window.display();
    }
    
    return 0;
}