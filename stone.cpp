#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "vector"
#include "iostream"
#include "string"
#include "map"


int main() {
    sf::RenderWindow window(sf::VideoMode(200, 200), "Stone", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Event event;
    
    std::srand(time(0));

    sf::Vector2f g = {0, 0.00000003};

    Object obj(sf::Vector2f(10, sf::VideoMode::getDesktopMode().height - 10), sf::Color::Red, 20, g);

    std::vector<sf::Text> texts;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return 1;
    }

    std::map<std::string, bool> keys = {
        {"leftMouse", false},
        {"stop", false}
    };

    texts.push_back(textInit(sf::Text(), font, "Position ", 20, sf::Vector2f(10, 10)));
    texts.push_back(textInit(sf::Text(), font, "Velocity ", 20, sf::Vector2f(10, 30)));
    texts.push_back(textInit(sf::Text(), font, "Acceleration ", 20, sf::Vector2f(10, 50)));
    texts.push_back(textInit(sf::Text(), font, "Last position ", 20, sf::Vector2f(10, 70)));
    texts.push_back(textInit(sf::Text(), font, "Time ", 20, sf::Vector2f(10, 90)));
    texts.push_back(textInit(sf::Text(), font, "Last time ", 20, sf::Vector2f(10, 110)));

    while (window.isOpen()) {
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::KeyPressed) {
                if (event.type == sf::Event::Closed or event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }

                if (event.key.code == sf::Keyboard::Space) {
                    keys["stop"] = !keys["stop"];
                }

                if (event.key.code == sf::Keyboard::R) {
                    window.clear();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    keys["leftMouse"] = true;
                }
            };

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    keys["leftMouse"] = false;
                    obj.calculateAcc();
                }
            };
        }

        if (keys["leftMouse"]) {
            sf::Vector2i mousePos = sf::Mouse::getPosition();
            obj.setPos({mousePos.x, mousePos.y});
            obj.setAcc({0, 0});
            obj.setVel({0, 0});
        }

        if (!keys["stop"]) {
            obj.update();
            obj.setAcc(g);
        }

        texts[0].setString("Position "     + std::to_string(obj.getPos().x) + " " + std::to_string(obj.getPos().y));
        texts[1].setString("Velocity "     + std::to_string(obj.getVel().x) + " " + std::to_string(obj.getVel().y));
        texts[2].setString("Acceleration " + std::to_string(obj.getAcc().x) + " " + std::to_string(obj.getAcc().y));
        texts[3].setString("Last position " + std::to_string(obj.getLastPos().x) + " " + std::to_string(obj.getLastPos().y));
        texts[4].setString("Time " + std::to_string(obj.getTime()));
        texts[5].setString("Last time " + std::to_string(obj.getLastTime()));

        window.clear();

        for (sf::Text text: texts) {
            window.draw(text);
        }

        obj.draw(window);
        window.display();
    }
    return 0;
}