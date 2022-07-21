#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "chrono"

//---------------------------FUNCTIONS-----------------------------------------
float getDistance(sf::Vector2f const &a, sf::Vector2f const &b); // distance between two points

float getVectorLength(sf::Vector2f const &v); // length of a vector

sf::Text textInit(sf::Text text, sf::Font &font, int size, sf::Vector2f pos);

sf::Vector2f abs(sf::Vector2f v);

sf::Vector2f clamp(sf::Vector2f v, float min, float max);

double clamp(double n, double min, double max);

sf::Vector2i const getScreenSize();

//----------------------------------OBJECT CLASS----------------------------------
class Object {
    private:
        sf::Vector2f pos;
        sf::Vector2f vel;
        sf::Vector2f acc;
        sf::RectangleShape rect;
        int size;
        sf::Color color;
        int mass;
        sf::Vector2f screenSize = sf::Vector2f(getScreenSize());
        sf::Vector2f lastPos;
        int t = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        int lastTime;
        sf::Vector2f g;
        bool movable = true;
        bool stop = false;
    
    public:
        Object(sf::Vector2f p, sf::Color c, int m, sf::Vector2f g);
        ~Object();
        void update();
        void draw(sf::RenderWindow &w);
        sf::Vector2f getPos();
        sf::Vector2f getVel();
        sf::Vector2f getAcc();
        sf::Color getColor();
        int getMass();
        void setPos(sf::Vector2f p);
        void setPos(sf::Vector2f p, bool f);
        void setAcc(sf::Vector2f a);
        void setColor(sf::Color c);
        void setMass(double m);
        void setVel(sf::Vector2f v);
        sf::Vector2f getLastPos();
        int getTime();
        int getLastTime();
        void calculateAcc();
        void isMovable(bool f);
        bool isMovable();
        void move(sf::Vector2f f);
        void move();
        int getSize();
        bool contains(sf::Vector2f p);
        void switchStop();
        void setStop(bool f);
};

//----------------------------------RECT CLASS----------------------------------
class Rect {
    private:
        sf::Vector2f pos;
        sf::Vertex vert[2];
        sf::Vector2i size;
        sf::Color color;
        sf::Vector2f screenSize = sf::Vector2f(getScreenSize());
        sf::Vector2f distAndAngle;
        sf::Vector2i id;
    
    public:
        Rect(sf::Vector2i i, sf::Vector2f p1, sf::Vector2f p2, sf::Color c);
        ~Rect();
        void draw(sf::RenderWindow &w);
        void setColor(sf::Color c);
        void setPos(sf::Vector2f p1, sf::Vector2f p2);
        sf::Vector2i getId();
};

//----------------------------------FPS CLASS----------------------------------
class FPS {
    private:
        sf::Clock clock;
        float lastTime;
        float currentTime;
        int fps;
        int cnt;

    public:
        FPS();
        ~FPS();
        int getFPS();
};
