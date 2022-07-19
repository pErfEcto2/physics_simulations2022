#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "vector"
#include "iostream"
#include "chrono"


sf::Vector2f operator-(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return sf::Vector2f(v1.x - v2.x, v1.y - v2.y);
}

sf::Vector2f operator*(sf::Vector2f const &v1, int const &n) {
    return sf::Vector2f(v1.x * n, v1.y * n);
}

sf::Vector2f operator*(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return sf::Vector2f(v1.x * v2.x, v1.y * v2.y);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(200, 200), "Net", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Event event;
    
    std::srand(time(0));

    int WIDTH = 10; // width of the net
    int HEIGHT = 10; // height of the net
    int number = WIDTH * HEIGHT; // number of objects in the net
    int offset = 50; // offset of the net from the screen
    int size = 20; // size of the objects

    float K = 40; // spring constant
    float airDencity = 80;
    float dTime = 0.00001; // time step

    sf::Vector2i screenSize = sf::Vector2i(window.getSize().x, window.getSize().y);
    
    std::vector<Object> objects; // vector of objects
    std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            /*
            fill the vector of objects with points
            */
            Object obj(sf::Vector2f((sf::VideoMode::getDesktopMode().width / WIDTH) * j + offset,
                                     (sf::VideoMode::getDesktopMode().height / HEIGHT) * i + offset),
                                     sf::Color(rand() % 255, rand() % 255, rand() % 255),
                                     size,
                                     sf::Vector2f(0, 0));
            obj.isMovable(false);
            objects.push_back(obj);
        }
    }

    float const_dx = sf::VideoMode::getDesktopMode().width / WIDTH;
    float const_dy = sf::VideoMode::getDesktopMode().height / HEIGHT;

    // hotkeys
    std::map<std::string, bool> keys = {
        {"leftMouse", false},
        {"stop", false},
        {"clear", true},
        {"air", false}
    };

    sf::Font font;

    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font" << std::endl;
        exit(1);
    }

    std::vector<sf::Text> texts;
    texts.push_back(textInit(sf::Text(), font, "Air Dencity: ", 20, sf::Vector2f(10, 10)));
    texts.push_back(textInit(sf::Text(), font, "Clear: ", 20, sf::Vector2f(200, 10)));
    texts.push_back(textInit(sf::Text(), font, "Left mouse ", 20, sf::Vector2f(300, 10)));
    texts.push_back(textInit(sf::Text(), font, "Energy ", 20, sf::Vector2f(500, 10)));

    while (window.isOpen()) {
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::KeyPressed) {
                if (event.type == sf::Event::Closed or event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    exit(0);
                }

                if (event.key.code == sf::Keyboard::Space) {
                    keys["stop"] = !keys["stop"];
                }

                if (event.key.code == sf::Keyboard::C) {
                    keys["clear"] = !keys["clear"];
                }

                if (event.key.code == sf::Keyboard::A) {
                    keys["air"] = !keys["air"];
                }

                if (event.key.code == sf::Keyboard::R) {
                    for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            objects[i * WIDTH + j].setPos(sf::Vector2f(sf::VideoMode::getDesktopMode().width  / WIDTH  * j + offset,
                                                                       sf::VideoMode::getDesktopMode().height / HEIGHT * i + offset),
                                                                       true);
                            objects[i * WIDTH + j].setVel(sf::Vector2f(0, 0));
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /*
                    if left mouse button is pressed, set the point, which contains cursor, to the mouse position
                    */
                    keys["leftMouse"] = true;
                    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition());
                    for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            if (objects[i * WIDTH + j].contains(mousePos) &&
                                i != 0 && j != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                                objects[i * WIDTH + j].isMovable(true);
                                break;
                            }
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    keys["leftMouse"] = false;
                    /*
                    if left mouse is released stop all points
                    */
                    for (Object &obj: objects) {
                        obj.isMovable(false);
                    }
                }
            }
        }

        for (int i = 1; i < HEIGHT - 1; i++) {
            for (int j = 1; j < WIDTH - 1; j++) {
                if (objects[i * WIDTH + j].isMovable()) {
                    /*
                    move the point, which contains cursor
                    */
                    objects[i * WIDTH + j].setPos(sf::Vector2f(sf::Mouse::getPosition())); // 2 dimensions
                }
            }
        }

        if (!keys["stop"]) {
            for (Object &obj: objects) {
                obj.update();
            }
        }

        std::chrono::milliseconds finish = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );

        if (finish - start > std::chrono::milliseconds(10) ) {
        //if (!keys["leftMouse"]) {
            /*
            move objects if left mouse is not pressed and find new distances
            */
            std::vector<sf::Vector2f> forces;
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                        forces.push_back(sf::Vector2f(0, 0));
                    }

                    else {

                        float dX1 = const_dx - (objects[i * WIDTH + j].getPos().x - objects[i * WIDTH + j - 1].getPos().x);
                        float dX2 = (objects[i * WIDTH + j + 1].getPos().x - objects[i * WIDTH + j].getPos().x) - const_dx;
                        float dX3 = const_dx - (objects[i * WIDTH + j].getPos().x - objects[(i - 1) * WIDTH + j].getPos().x);
                        float dX4 = (objects[(i + 1) * WIDTH + j].getPos().x - objects[i * WIDTH + j].getPos().x) - const_dx;

                        float dY1 = const_dy - (objects[i * WIDTH + j].getPos().y - objects[i * WIDTH + j - 1].getPos().y);
                        float dY2 = (objects[i * WIDTH + j + 1].getPos().y - objects[i * WIDTH + j].getPos().y) - const_dy;
                        float dY3 = const_dy - (objects[i * WIDTH + j].getPos().y - objects[(i - 1) * WIDTH + j].getPos().y);
                        float dY4 = (objects[(i + 1) * WIDTH + j].getPos().y - objects[i * WIDTH + j].getPos().y) - const_dy;

                        sf::Vector2f force = sf::Vector2f(dX1 + dX2 + dX3 + dX4, dY1 + dY2 + dY3 + dY4) * K * dTime;
                        
                        if (keys["air"] &&
                        abs(objects[i * WIDTH + j].getVel().x) > 0 &&
                        abs(objects[i * WIDTH + j].getVel().y) > 0) {
                            sf::Vector2f air = {objects[i * WIDTH + j].getVel().x / abs(objects[i * WIDTH + j].getVel().x),
                                                objects[i * WIDTH + j].getVel().y / abs(objects[i * WIDTH + j].getVel().y)};
                            
                            force -= objects[i * WIDTH + j].getVel() * objects[i * WIDTH + j].getVel() * objects[i * WIDTH + j].getSize() * airDencity * air * dTime;
                        }

                        forces.push_back(force);
                    }
                }
            }

            for (int i = 0; i < number; i++) {
                objects[i].setAcc(forces[i]);
                objects[i].move();
            }

            start = finish;
        }

        if (keys["clear"]) window.clear();

        double energy = 0;
        for (Object &obj: objects) {
            energy += obj.getMass() * pow(getVectorLength(obj.getVel()), 2) / 2 +
                      K * getVectorLength(obj.getLastPos() - obj.getPos()) +
                      obj.getMass() * 1 * (screenSize.y - obj.getPos().y);
            obj.draw(window);
        }

        texts[0].setString("Air resistance: " + std::to_string(keys["air"] ? 1 : 0));
        texts[1].setString("Clear: " + std::to_string(keys["clear"] ? 1 : 0));
        texts[2].setString("Left mouse: " + std::to_string(keys["leftMouse"] ? 1 : 0));
        texts[3].setString("Energy: " + std::to_string(energy / 1e4));

        for (sf::Text &text: texts) {
            window.draw(text);
        }

        window.display();
    }
    return 0;
}