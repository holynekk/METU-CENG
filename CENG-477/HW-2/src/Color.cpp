#include "Color.h"
#include <iostream>
#include <iomanip>

using namespace std;

Color::Color() {}

Color::Color(double r, double g, double b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color(const Color &other)
{
    this->r = other.r;
    this->g = other.g;
    this->b = other.b;
}

// Round to closest int by trimming 
Color Color::round_to_closest_int() {
    Color c;
    c.r = (int) (r + 0.5);
    c.g = (int) (g + 0.5);
    c.b = (int) (b + 0.5);
    return c;
}

ostream& operator<<(ostream& os, const Color& c)
{
    os << fixed << setprecision(0) << "rgb(" << c.r << ", " << c.g << ", " << c.b << ")";
    return os;
}
