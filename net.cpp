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

sf::Vector2f operator+(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return sf::Vector2f(v1.x + v2.x, v1.y + v2.y);
}

sf::Vector2f operator*(sf::Vector2f const &v1, int const &n) {
    return sf::Vector2f(v1.x * n, v1.y * n);
}

sf::Vector2f operator*(sf::Vector2f const &v1, sf::Vector2f const &v2) {
    return sf::Vector2f(v1.x * v2.x, v1.y * v2.y);
}

bool operator>(sf::Vector2f const &v1, double const &n) {
    return v1.x > n && v1.y > n;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(200, 200), "Net", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    sf::Event event;
    
    std::srand(time(0));

    int WIDTH = 150; // width of the net
    int HEIGHT = 150; // height of the net
    int number = WIDTH * HEIGHT; // number of objects in the net
    int offset = 30; // offset of the net from the screen
    int size = 10; // size of the objects

    float K = 40; // spring constant
    float airDencity = 10;
    float dTime = 1e-4; // time step

    sf::Vector2i screenSize = getScreenSize();
    
    std::vector<Object> objects;
    std::vector<Rect> rects;
    sf::Vector2f forces[number]; // forces on each object

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
                                     sf::Color(30 + rand() % 225, 30 + rand() % 225, 30 + rand() % 225),
                                     size);
            obj.isMovable(false);
            objects.push_back(obj);
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            /*
            fill the vector of rects with connections between points
            */
            if (i != HEIGHT - 1 && j != 0 && j != WIDTH - 1) {
                /*
                Vertical connection
                */
                Rect rect(sf::Vector2i(i * WIDTH + j, (i + 1) * WIDTH + j), objects[i * WIDTH + j].getPos(), objects[(i + 1) * WIDTH + j].getPos(),
                       sf::Color::White);
                rects.push_back(rect);
            }

            if (i != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                /*
                Horizontal connection
                */
                Rect rect(sf::Vector2i(i * WIDTH + j, i * WIDTH + j + 1), objects[i * WIDTH + j].getPos(), objects[i * WIDTH + j + 1].getPos(),
                       sf::Color::White);
                rects.push_back(rect);
            }
        }
    }

    // distances between horizontal and vertical connections
    float const_dx = sf::VideoMode::getDesktopMode().width / WIDTH;
    float const_dy = sf::VideoMode::getDesktopMode().height / HEIGHT;

    // hotkeys
    std::map<std::string, bool> hotKeys = {
        {"leftMouse", false},
        {"stop", false},
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
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(400, 10)));
    texts.push_back(textInit(sf::Text(), font, 20, sf::Vector2f(1200, 10)));

    FPS fps;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.type == sf::Event::Closed or event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    exit(0);
                }

                if (event.key.code == sf::Keyboard::Space) {
                    hotKeys["stop"] = !hotKeys["stop"];
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
                            Set base position for all points
                            */
                            objects[i * WIDTH + j].setPos(sf::Vector2f(sf::VideoMode::getDesktopMode().width  / WIDTH  * j + offset,
                                                                       sf::VideoMode::getDesktopMode().height / HEIGHT * i + offset));
                            objects[i * WIDTH + j].setVel(sf::Vector2f(0, 0));
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition());
                    for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                if (objects[i * WIDTH + j].contains(mousePos) &&
                                    i != 0 && j != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                                        /*
                                        if left mouse button is pressed,
                                        set the point, which contains cursor, to the mouse position
                                        */
                                    objects[i * WIDTH + j].isMovable(true);
                                    hotKeys["leftMouse"] = true;
                                    break;
                                }
                            }
                            else if(event.mouseButton.button == sf::Mouse::Right) {
                                if (objects[i * WIDTH + j].contains(mousePos) &&
                                    i != 0 && j != 0 && i != HEIGHT - 1 && j != WIDTH - 1) {
                                    /*
                                    if right mouse button is pressed,
                                    stop the point, which contains cursor
                                    */
                                    objects[i * WIDTH + j].switchStop();
                                    hotKeys["rightMouse"] = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    hotKeys["leftMouse"] = false;
                    for (Object &obj: objects) {
                        if(obj.isMovable()) obj.isMovable(false);
                    }
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                /*
                modify the dTime variable via mouse wheel
                */
                dTime += event.mouseWheelScroll.delta * 2 / 1e5;
                dTime = clamp(dTime, 1e-4, 1e-3);
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

        std::chrono::milliseconds finish = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        );

        if (finish - start > std::chrono::milliseconds(10) ) {
        /*
        Calculate new forces and move all objects
        */
        //if (!hotKeys["leftMouse"]) {
            /*
            move objects if left mouse is not pressed and find new distances
            */
            // start
            for (int i = 0; i < HEIGHT; i++) {
                for (int j = 0; j < WIDTH; j++) {
                    if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                        forces[i * WIDTH + j] = sf::Vector2f(0, 0);
                    }
                    else {
                        sf::Vector2f dXY1 = {const_dx - (objects[i * WIDTH + j].getPos().x - objects[i * WIDTH + j - 1].getPos().x),
                                             const_dy - (objects[i * WIDTH + j].getPos().y - objects[i * WIDTH + j - 1].getPos().y)};
                        sf::Vector2f dXY2 = {(objects[i * WIDTH + j + 1].getPos().x - objects[i * WIDTH + j].getPos().x) - const_dx,
                                             (objects[i * WIDTH + j + 1].getPos().y - objects[i * WIDTH + j].getPos().y) - const_dy};
                        sf::Vector2f dXY3 = {const_dx - (objects[i * WIDTH + j].getPos().x - objects[(i - 1) * WIDTH + j].getPos().x),
                                             const_dy - (objects[i * WIDTH + j].getPos().y - objects[(i - 1) * WIDTH + j].getPos().y)};
                        sf::Vector2f dXY4 = {(objects[(i + 1) * WIDTH + j].getPos().x - objects[i * WIDTH + j].getPos().x) - const_dx,
                                             (objects[(i + 1) * WIDTH + j].getPos().y - objects[i * WIDTH + j].getPos().y) - const_dy};


                        sf::Vector2f force = (dXY1 + dXY2 + dXY3 + dXY4) * K * dTime;
                        
                        if (hotKeys["air"] && abs(objects[i * WIDTH + j].getVel().x) > 0 && abs(objects[i * WIDTH + j].getVel().y) > 0) {
                            sf::Vector2f air = {objects[i * WIDTH + j].getVel().x / abs(objects[i * WIDTH + j].getVel().x),
                                                objects[i * WIDTH + j].getVel().y / abs(objects[i * WIDTH + j].getVel().y)};
                            
                            force -= objects[i * WIDTH + j].getVel() * objects[i * WIDTH + j].getVel() * objects[i * WIDTH + j].getSize() * airDencity * air * dTime;
                        }
                        forces[i * WIDTH + j] = clamp(force, -10.0f, 10.0f);
                    }
                }
            } // finish - start = 10 +-1 ms (150 * 150  objects)

            for (int i = 0; i < number; i++) {
                objects[i].setAcc(forces[i]);
                objects[i].move();
            }

            for (Rect &rect: rects) {
                rect.setPos(objects[rect.getId().x].getPos(), objects[rect.getId().y].getPos());
            }
            start = finish;
        }

        if (hotKeys["clear"]) window.clear();

        // start
        for (Rect &rect: rects) {
            rect.draw(window);
        }
        for (Object &obj: objects) {
            obj.draw(window);
        }
        // finish - start =  20 +-1 ms (150 * 150  objects)

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
