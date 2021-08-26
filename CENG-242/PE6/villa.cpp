#include <iostream>
#include "villa.h"
#include "owner.h"

using namespace std;

Villa::Villa(const string &property_name, int area, Owner *owner, int number_of_floors, bool having_garden)
{
    this->property_name = property_name;
    this->area = area;
    this->owner = owner;
    this->number_of_floors = number_of_floors;
    this->having_garden = having_garden;
    if (owner) this->owner->add_property(this);
}

float Villa::valuate()
{
    float result = area * 10 * number_of_floors;
    return having_garden ? 2*result : result;
}
