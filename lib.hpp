#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "chrono"

//---------------------------FUNCTIONS-----------------------------------------
float getDistance(sf::Vector2f const &a, sf::Vector2f const &b);

float getVectorLength(sf::Vector2f const &v);

sf::Text textInit(sf::Text text, sf::Font &font, int size, sf::Vector2f pos);

sf::Vector2f abs(sf::Vector2f v);

sf::Vector2f clamp(sf::Vector2f v, float min, float max);

double clamp(double n, double min, double max);

sf::Vector2f const getScreenSize();


//----------------------------------Knot CLASS----------------------------------
class Knot {
    /*
    This class is used th show knots on the screen.
    */
    private:
        sf::Vector2f pos;
        sf::Vector2f vel;
        sf::Vector2f acc;
        sf::RectangleShape rect;
        int size;
        sf::Color color;
        sf::Vector2f screenSize = getScreenSize();
        int t = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        bool movable = false;
        bool stop = false;
    
    public:
        Knot(sf::Vector2f p, sf::Color c, int s);
        ~Knot();
        void draw(sf::RenderWindow &w);
        sf::Vector2f getPos();
        sf::Vector2f getVel();
        void setPos(sf::Vector2f p);
        void setAcc(sf::Vector2f a);
        void setVel(sf::Vector2f v);
        void isMovable(bool f);
        bool isMovable();
        void move();
        int getSize();
        bool contains(sf::Vector2f p);
        void switchStop();
};


//----------------------------------Connection CLASS----------------------------------
class Connection {
    /*
    This class is used to show connections between Knots.
    */
    private:
        sf::Vector2f pos;
        sf::Vertex vert[2];
        sf::Color color;
        sf::Vector2f screenSize = getScreenSize();
        sf::Vector2i id;
    
    public:
        Connection(sf::Vector2i i, sf::Vector2f p1, sf::Vector2f p2, sf::Color c);
        ~Connection();
        void draw(sf::RenderWindow &w);
        void setPos(sf::Vector2f p1, sf::Vector2f p2);
        sf::Vector2i getId();
};


//----------------------------------FPS CLASS----------------------------------
class FPS {
    /*
    This class is used to calculate the FPS in the programm.
    Every 10 frames the FPS is calculated.
    */
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
