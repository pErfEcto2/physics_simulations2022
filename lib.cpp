#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "iostream"
#include "chrono"


//---------------------------OPERATORS-----------------------------------------
sf::Vector2f operator*(sf::Vector2f const &v, double const &n) {
    return sf::Vector2f(v.x * n, v.y * n);
}

sf::Vector2f operator/(sf::Vector2f const &v, double const &n) {
    return sf::Vector2f(v.x / n, v.y / n);
}

//------------------------------------FUNCTIONS------------------------------------
float clamp(float const &n, float const &min, float const &max) {
    return std::max(min, std::min(n, max));
}

float getDistance(sf::Vector2f const &a, sf::Vector2f const &b) { // distance between two points
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float getVectorLength(sf::Vector2f const &v) { // length of a vector
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

sf::Vector2f abs(sf::Vector2f v) {
    return sf::Vector2f(abs(v.x), abs(v.y));
}

sf::Vector2f clamp(sf::Vector2f v, float min, float max) {
    return sf::Vector2f(clamp(v.x, min, max), clamp(v.y, min, max));
}

double clamp(double n, double min, double max) {
    return std::max(min, std::min(n, max));
}

sf::Text textInit(sf::Text text, sf::Font &font, int size, sf::Vector2f pos) {
    text.setFont(font);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(pos);
    return text;
}

sf::Vector2i const getScreenSize() {
    return sf::Vector2i(sf::VideoMode::getDesktopMode().width,
                        sf::VideoMode::getDesktopMode().height);
}

//----------------------------------OBJECT CLASS----------------------------------
Object::Object(sf::Vector2f p, sf::Color c, int m) {
    pos = p;
    color = c;
    size = m;
    vel = sf::Vector2f(0, 0);
    rect.setSize(sf::Vector2f(size, size));
    rect.setPosition(pos);
    rect.setFillColor(color);
    rect.setOrigin(size / 2, size / 2);
}

Object::~Object() {}

void Object::draw(sf::RenderWindow &w) {
    w.draw(rect);
}

sf::Vector2f Object::getPos() {
    return pos;
}

sf::Vector2f Object::getVel() {
    return vel;
}

void Object::isMovable(bool f) {
    movable = f;
}

bool Object::isMovable() {
    return movable;
}

void Object::setPos(sf::Vector2f p) {
    pos = p;
}

void Object::move() {
    if (!stop) {
        pos += vel;
        rect.setPosition(pos);
    }
}

void Object::switchStop() {
    stop = !stop;
}

void Object::setVel(sf::Vector2f v) {
    if (abs(v.x) > 0.001 && abs(v.y) > 0.001) {
        vel = v;
    }
    else {
        vel = sf::Vector2f(0, 0);
    }
}

void Object::setAcc(sf::Vector2f a) {
    acc = a;
    vel += acc;
}

bool Object::contains(sf::Vector2f p) {
    return rect.getGlobalBounds().contains(p);
}

int Object::getSize() {
    return size;
}

//-------------------------------------------RECT CLASS-------------------------------------------
Rect::Rect(sf::Vector2i i, sf::Vector2f p1, sf::Vector2f p2, sf::Color c) {
    color = c;
    vert[0].position = p1;
    vert[1].position = p2;
    vert[0].color = color;
    vert[1].color = color;
    id = i;
}

Rect::~Rect() {}

void Rect::setPos(sf::Vector2f p1, sf::Vector2f p2) {
    vert[0].position = p1;
    vert[1].position = p2;
}

void Rect::draw(sf::RenderWindow &w) {
    w.draw(vert, 2, sf::Lines);
}

sf::Vector2i Rect::getId() {
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
