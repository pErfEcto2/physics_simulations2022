#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "math.h"
#include "lib.hpp"
#include "iostream"
#include "chrono"


sf::Vector2f operator*(sf::Vector2f const &v, double const &n) {
    return sf::Vector2f(v.x * n, v.y * n);
}

sf::Vector2f operator/(sf::Vector2f const &v, double const &n) {
    return sf::Vector2f(v.x / n, v.y / n);
}

float clamp(float const &n, float const &min, float const &max) {
    return std::max(min, std::min(n, max));
}

float getDistance(sf::Vector2f const &a, sf::Vector2f const &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

float getVectorLength(sf::Vector2f const &v) {
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

sf::Vector2f abs(sf::Vector2f v) {
    return sf::Vector2f(abs(v.x), abs(v.y));
}

sf::Vector2f clamp(sf::Vector2f v, float min, float max) {
    return sf::Vector2f(clamp(v.x, min, max), clamp(v.y, min, max));
}

sf::Vector2f defineLine(sf::Vector2f p1, sf::Vector2f p2) {
    float dist = getDistance(p1, p2);
    float angle = atan2(p2.y - p1.y, p2.x - p1.x);
    return sf::Vector2f(dist, angle);
}

Object::Object(sf::Vector2f p, sf::Color c, int m, sf::Vector2f g) {
    pos = p;
    color = c;
    mass = m;
    size = m;
    g = g;
    vel = sf::Vector2f(0, 0);
    lastPos = pos;
    rect.setSize(sf::Vector2f(size, size));
    rect.setPosition(pos);
    rect.setFillColor(color);
    rect.setOrigin(size / 2, size / 2);
}

Object::~Object() {}

void Object::update() {
    if (movable) {
        pos += vel;

        if (pos.x - size / 2 < 0) {
            vel.x *= -1;
            pos.x = size / 2;
        }

        if (pos.x + size / 2 > screenSize.x) {
            vel.x *= -1;
            pos.x = screenSize.x - size / 2;
        }

        if (pos.y - size / 2 < 0) {
            vel.y *= -1;
            if (abs(vel.x) < 0.1) {
                vel.x = 0;
            }
            pos.y = size / 2;
        }

        if (pos.y + size / 2 > screenSize.y) {
            vel.y *= -1;
            if (abs(vel.x) < 0.1) {
                vel.x = 0;
            }
            pos.y = screenSize.y - size / 2;
            if (abs(vel.y) < 0.1) {
                vel.y = 0;
            }
        }
    }
    rect.setPosition(pos);
}

sf::Text textInit(sf::Text text, sf::Font &font, std::string s, int size, sf::Vector2f pos) {
    text.setFont(font);
    text.setString(s);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(pos);
    return text;
}

void Object::draw(sf::RenderWindow &w) {
    w.draw(rect);
}

sf::Vector2f Object::getAcc() {
    return acc;
}

sf::Vector2f Object::getPos() {
    return pos;
}

sf::Vector2f Object::getVel() {
    return vel;
}

sf::Color Object::getColor() {
    return color;
}

int Object::getMass() {
    return mass;
}

void Object::isMovable(bool f) {
    movable = f;
}

bool Object::isMovable() {
    return movable;
}

void Object::setPos(sf::Vector2f p, bool f) {
    pos = p;
    if (f) {
        lastPos = pos;
    }
}

void Object::setPos(sf::Vector2f p) {
    lastPos = pos;
    pos = p;
    
    if (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t > 100000) {
        lastTime = t;
        t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}

void Object::move(sf::Vector2f f) {
    if (!stop) {
        pos += f / mass;
        rect.setPosition(pos);
    }
}

void Object::move() {
    if (!stop) {
        pos += vel;
        rect.setPosition(pos);
    }
}

void Object::calculateAcc() {
    acc = (pos - lastPos) / ((t - lastTime)) * 17000;
    vel += acc;
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

int Object::getTime() {
    return t;
}

int Object::getLastTime() {
    return lastTime;
}

sf::Vector2f Object::getLastPos() {
    return lastPos;
}

void Object::switchStop() {
    stop = !stop;
}

void Object::setStop(bool f) {
    stop = f;
}

bool Object::contains(sf::Vector2f p) {
    return rect.getGlobalBounds().contains(p);
}

void Object::setColor(sf::Color c) {
    color = c;
    rect.setFillColor(color);
}

void Object::setMass(double m) {
    mass = m;
}

int Object::getSize() {
    return size;
}

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

void Rect::setColor(sf::Color c) {
    color = c;
    vert[0].color = color;
    vert[1].color = color;
}

FPS::FPS() {
    lastTime = 0;
}

int FPS::getFPS() {
    currentTime = clock.restart().asSeconds();
    return 1.f / (currentTime - lastTime);
}

void FPS::update() {
    lastTime = currentTime;
}
