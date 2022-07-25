#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "vector"
#include "iostream"
#include "chrono"


sf::Vector2f operator-(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    /*
    Subtracts two vectors.
    */
    return sf::Vector2f(v1.x - v2.x, v1.y - v2.y);
}

sf::Vector2f operator+(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    /*
    Adds two vectors.
    */
    return sf::Vector2f(v1.x + v2.x, v1.y + v2.y);
}

sf::Vector2f operator*(sf::Vector2f const &v1, int const &n) {
    /*
    Multiplies a vector by a number.
    */
    return sf::Vector2f(v1.x * n, v1.y * n);
}

sf::Vector2f operator*(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    /*
    Multiplies two vectors.
    */
    return sf::Vector2f(v1.x * v2.x, v1.y * v2.y);
}

bool operator>(sf::Vector2f const &v1, double const &n) {
    /*
    Checks if a vector's X and Y is greater than a number.
    */
    return v1.x > n && v1.y > n;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(200, 200), "Net", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Event event;
    
    std::srand(time(0));

    int WIDTH = 10; // width of the net
    int HEIGHT = 10; // height of the net
    int number = WIDTH * HEIGHT; // number of knots in the net
    int offset = 30; // offset of the net from the top and left sides on the screen
    int size = 20; // size of the knots

    float K = 40; // spring constant
    float airDencity = 20;
    float dTime = 1e-4; // time step

    sf::Vector2f screenSize = getScreenSize();
    
    std::vector<Knot> knots;
    std::vector<Connection> connections;
    sf::Vector2f forces[number]; // forces on each knot

    std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            /*
            fill the vector of knots
            */
            Knot knot(sf::Vector2f((sf::VideoMode::getDesktopMode().width / WIDTH) * j + offset,
                                     (sf::VideoMode::getDesktopMode().height / HEIGHT) * i + offset),
                                     sf::Color(15 + rand() % 240, 15 + rand() % 240, 15 + rand() % 240),
                                     size);
            knot.isMovable(false);
            knots.push_back(knot);
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            /*
            fill the vector of connections between knots
            */
            if (i != HEIGHT - 1 && j != 0 && j != WIDTH - 1) {
                /*
                Vertical connection
                */
                Connection rect(sf::Vector2i(i * WIDTH + j, (i + 1) * WIDTH + j), knots[i * WIDTH + j].getPos(), knots[(i + 1) * WIDTH + j].getPos(),
                       sf::Color::White);
                connections.push_back(rect);
            }

            if (i != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                /*
                Horizontal connection
                */
                Connection rect(sf::Vector2i(i * WIDTH + j, i * WIDTH + j + 1), knots[i * WIDTH + j].getPos(), knots[i * WIDTH + j + 1].getPos(),
                       sf::Color::White);
                connections.push_back(rect);
            }
        }
    }

    // distances between horizontal and vertical knots
    float const_dx = sf::VideoMode::getDesktopMode().width / WIDTH;
    float const_dy = sf::VideoMode::getDesktopMode().height / HEIGHT;

    // hotkeys
    std::map<std::string, bool> hotKeys = {
        {"leftMouse", false},
        {"clear", true},
        {"air", false},
        {"rightMouse", false}
    };

    sf::Font font;

    if (!font.loadFromFile("src/fonts/arial.ttf")) {
        std::cout << "Error loading font" << std::endl;
        window.close();
        exit(1);
    }

    // initialize all texts
    std::vector<sf::Text> texts;
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(10, 10)));
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(200, 10)));
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(300, 10)));
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(450, 10)));
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(1200, 10)));

    FPS fps;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.type == sf::Event::Closed or event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    exit(0);
                }

                if (event.key.code == sf::Keyboard::C) {
                    hotKeys["clear"] = !hotKeys["clear"];
                }

                if (event.key.code == sf::Keyboard::A) {
                    hotKeys["air"] = !hotKeys["air"];
                }

                if (event.key.code == sf::Keyboard::R) {
                    for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            /*
                            Set base position for all knots
                            */
                            knots[i * WIDTH + j].setPos(sf::Vector2f(sf::VideoMode::getDesktopMode().width  / WIDTH  * j + offset,
                                                                       sf::VideoMode::getDesktopMode().height / HEIGHT * i + offset));
                            knots[i * WIDTH + j].setVel(sf::Vector2f(0, 0));
                            knots[i * WIDTH + j].switchStop(false);
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition());
                    for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                if (knots[i * WIDTH + j].contains(mousePos) &&
                                    i != 0 && j != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                                        /*
                                        If left mouse button is pressed,
                                        set the point, which contains cursor, to the mouse position
                                        */
                                    knots[i * WIDTH + j].isMovable(true);
                                    hotKeys["leftMouse"] = true;
                                    break;
                                }
                            }
                            else if(event.mouseButton.button == sf::Mouse::Right) {
                                if (knots[i * WIDTH + j].contains(mousePos) &&
                                    i != 0 && j != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                                    /*
                                    if right mouse button is pressed,
                                    stop the point, which contains cursor
                                    */
                                    knots[i * WIDTH + j].switchStop();
                                    hotKeys["rightMouse"] = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    /*
                    If left mouse button is released,
                    release all knots
                    */
                    hotKeys["leftMouse"] = false;
                    for (Knot &knot: knots) {
                        if(knot.isMovable()) knot.isMovable(false);
                    }
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                /*
                Increase or decrease the dTime variable via mouse wheel
                */
                dTime += event.mouseWheelScroll.delta * 2 / 1e5;
                dTime = clamp(dTime, 1e-4, 1e-3);
            }
        }

        for (int i = 1; i < HEIGHT - 1; i++) {
            for (int j = 1; j < WIDTH - 1; j++) {
                if (knots[i * WIDTH + j].isMovable()) {
                    /*
                    Move the point, which contains cursor
                    */
                    knots[i * WIDTH + j].setPos(sf::Vector2f(sf::Mouse::getPosition())); // 2 dimensions
                }
            }
        }

        std::chrono::milliseconds finish = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );

        if (finish - start > std::chrono::milliseconds(10) ) {
        /*
        Calculate new forces and move all knots
        */
        //if (!hotKeys["leftMouse"]) {
            /*
            move knots if left mouse is not pressed and find new distances
            */
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                        /*
                        If point is on the border,
                        dont move it
                        */
                        forces[i * WIDTH + j] = sf::Vector2f(0, 0);
                    }
                    else {
                        /*
                        New distances between knots
                        */
                        sf::Vector2f dXY1 = {const_dx - (knots[i * WIDTH + j].getPos().x - knots[i * WIDTH + j - 1].getPos().x),
                                             const_dy - (knots[i * WIDTH + j].getPos().y - knots[i * WIDTH + j - 1].getPos().y)};
                        sf::Vector2f dXY2 = {(knots[i * WIDTH + j + 1].getPos().x - knots[i * WIDTH + j].getPos().x) - const_dx,
                                             (knots[i * WIDTH + j + 1].getPos().y - knots[i * WIDTH + j].getPos().y) - const_dy};
                        sf::Vector2f dXY3 = {const_dx - (knots[i * WIDTH + j].getPos().x - knots[(i - 1) * WIDTH + j].getPos().x),
                                             const_dy - (knots[i * WIDTH + j].getPos().y - knots[(i - 1) * WIDTH + j].getPos().y)};
                        sf::Vector2f dXY4 = {(knots[(i + 1) * WIDTH + j].getPos().x - knots[i * WIDTH + j].getPos().x) - const_dx,
                                             (knots[(i + 1) * WIDTH + j].getPos().y - knots[i * WIDTH + j].getPos().y) - const_dy};

                        /*
                        Calculate force for a knot
                        */
                        sf::Vector2f force = (dXY1 + dXY2 + dXY3 + dXY4) * K * dTime;
                        
                        if (hotKeys["air"] && abs(knots[i * WIDTH + j].getVel().x) > 0 && abs(knots[i * WIDTH + j].getVel().y) > 0) {
                            /*
                            If dencity of the air is enabled,
                            add air resistance to the force
                            */
                            sf::Vector2f air = {knots[i * WIDTH + j].getVel().x / abs(knots[i * WIDTH + j].getVel().x),
                                                knots[i * WIDTH + j].getVel().y / abs(knots[i * WIDTH + j].getVel().y)};
                            
                            force -= knots[i * WIDTH + j].getVel() * knots[i * WIDTH + j].getVel() * knots[i * WIDTH + j].getSize() * airDencity * air * dTime;
                        }
                        forces[i * WIDTH + j] = clamp(force, -10.0f, 10.0f);
                    }
                }
            }

            for (int i = 0; i < number; i++) {
                /*
                Move all knots
                */
                knots[i].setAcc(forces[i]);
                knots[i].move();
            }

            for (Connection &rect: connections) {
                /*
                Move all connections
                */
                rect.setPos(knots[rect.getId().x].getPos(), knots[rect.getId().y].getPos());
            }
            start = finish;
        }

        if (hotKeys["clear"]) window.clear();

        for (Connection &rect: connections) {
            rect.draw(window);
        }
        for (Knot &knot: knots) {
            knot.draw(window);
        }

        texts[0].setString("Air resistance: " + std::to_string(hotKeys["air"] ? 1 : 0));
        texts[1].setString("Clear: " + std::to_string(hotKeys["clear"] ? 1 : 0));
        texts[2].setString("Left mouse: " + std::to_string(hotKeys["leftMouse"] ? 1 : 0));
        texts[3].setString("Time delta: " + std::to_string(dTime));
        texts[4].setString("FPS: " + std::to_string(fps.getFPS()));

        for (sf::Text &text: texts) {
            window.draw(text);
        }

        window.display();
    }
    return 0;
}
