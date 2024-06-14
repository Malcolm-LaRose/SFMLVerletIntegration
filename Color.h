#pragma once

#ifndef COLOR_H
#define COLOR_H

#include <SFML/Graphics.hpp>

struct Color {
    static const sf::Color WHITE;
    static const sf::Color BLACK;
    static const sf::Color RED;
    static const sf::Color GREEN;
    static const sf::Color BLUE;
    static const sf::Color PHSORNG;
    static const sf::Color MAGENTA;
    static const sf::Color DRKGRY;
    static const sf::Color EIGENGRAU;
    static const sf::Color LANDLORDWHITE;
    static const sf::Color BLUEPRINT;
    static const sf::Color TRANSL50;
    static const sf::Color TRANSP;
};

// Define the static constants
const sf::Color Color::WHITE(255, 255, 255, 255);
const sf::Color Color::BLACK(0, 0, 0, 255);
const sf::Color Color::RED(255, 0, 0, 255);
const sf::Color Color::GREEN(0, 255, 0, 255);
const sf::Color Color::BLUE(0, 0, 255, 255);
const sf::Color Color::PHSORNG(255, 204, 0, 255);
const sf::Color Color::MAGENTA(255, 0, 255, 255);
const sf::Color Color::DRKGRY(55, 55, 55, 255);
const sf::Color Color::EIGENGRAU(22, 22, 29, 255);
const sf::Color Color::LANDLORDWHITE(250, 249, 246, 255);
const sf::Color Color::BLUEPRINT(96, 130, 166, 255);
const sf::Color Color::TRANSL50(10, 10, 10, 128);
const sf::Color Color::TRANSP(0, 0, 0, 0);

#endif // COLOR_H