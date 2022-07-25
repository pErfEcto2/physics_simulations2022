#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "iostream"
#include "chrono"


//---------------------------OPERATORS-----------------------------------------
sf::Vector2f operator*(sf::Vector2f const &v, double const &n) {
    /*
    Multiplies a vector by a number.
    */
    return sf::Vector2f(v.x * n, v.y * n);
}

sf::Vector2f operator/(sf::Vector2f const &v, double const &n) {
    /*
    Divides a vector by a number.
    */
    return sf::Vector2f(v.x / n, v.y / n);
}


//------------------------------------FUNCTIONS------------------------------------
float getDistance(sf::Vector2f const &a, sf::Vector2f const &b) {
    /*
    distance between two points
    */
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float getVectorLength(sf::Vector2f const &v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

sf::Vector2f abs(sf::Vector2f v) {
    /*
    Overload of the abs function for sf::Vector2f
    */
    return sf::Vector2f(abs(v.x), abs(v.y));
}

sf::Vector2f clamp(sf::Vector2f v, float min, float max) {
    /*
    Clamps a vector between a minimum and maximum value.
    */
    return sf::Vector2f(clamp(v.x, min, max), clamp(v.y, min, max));
}

double clamp(double n, double min, double max) {
    /*
    Clamps a double between a minimum and maximum value.
    */
    return std::max(min, std::min(n, max));
}

sf::Text textInit(sf::Text text, sf::Font &font, int size, sf::Vector2f pos) {
    /*
    This finstions initializes a text Knot.
    */
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(pos);
    return text;
}

sf::Vector2f const getScreenSize() {
    return sf::Vector2f(sf::VideoMode::getDesktopMode().width,
                        sf::VideoMode::getDesktopMode().height);
}


//----------------------------------Knot CLASS----------------------------------
Knot::Knot(sf::Vector2f p, sf::Color c, int s) {
    pos = p;
    color = c;
    size = s;
    vel = sf::Vector2f(0, 0);
    rect.setSize(sf::Vector2f(size, size));
    rect.setPosition(pos);
    rect.setFillColor(color);
    rect.setOrigin(size / 2, size / 2);
}

Knot::~Knot() {}

void Knot::draw(sf::RenderWindow &w) {
    w.draw(rect);
}

sf::Vector2f Knot::getPos() {
    return pos;
}

sf::Vector2f Knot::getVel() {
    return vel;
}

void Knot::isMovable(bool f) {
    movable = f;
}

bool Knot::isMovable() {
    return movable;
}

void Knot::setPos(sf::Vector2f p) {
    pos = p;
    rect.setPosition(pos);
}

void Knot::move() {
    if (!stop) {
        pos += vel;
        rect.setPosition(pos);
    }
}

void Knot::switchStop() {
    stop = !stop;
}

bool Knot::isStop() {
    return stop;
}

void Knot::switchStop(bool f) {
    stop = f;
}

void Knot::setVel(sf::Vector2f v) {
    vel = v;
}

void Knot::setAcc(sf::Vector2f a) {
    acc = a;
    vel += acc;
}

bool Knot::contains(sf::Vector2f p) {
    return rect.getGlobalBounds().contains(p);
}

int Knot::getSize() {
    return size;
}


//-------------------------------------------Connection CLASS-------------------------------------------
Connection::Connection(sf::Vector2i i, sf::Vector2f p1, sf::Vector2f p2, sf::Color c) {
    color = c;
    vert[0].position = p1; // position of the first knot
    vert[1].position = p2; // position of the second knot
    vert[0].color = color;
    vert[1].color = color;
    id = i; // indexes in main vector of connected knots
}

Connection::~Connection() {}

void Connection::setPos(sf::Vector2f p1, sf::Vector2f p2) {
    vert[0].position = p1; // position of the first knot
    vert[1].position = p2; // position of the second knot
}

void Connection::draw(sf::RenderWindow &w) {
    w.draw(vert, 2, sf::Lines);
}

sf::Vector2i Connection::getId() {
    return id;
}


//-------------------------------FPS CLASS---------------------------------------
FPS::FPS() {
    lastTime = 0;
    cnt = 0;
    fps = 0;
}

FPS::~FPS() {}

int FPS::getFPS() {
    cnt += 1;
    if (cnt % 10 == 0) {
        fps = 10.f / (clock.restart().asSeconds() - lastTime);
        lastTime = clock.restart().asSeconds();
        cnt = 0;
    }
    return fps;
}
