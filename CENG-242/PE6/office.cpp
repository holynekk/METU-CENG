#include <iostream>
#include "office.h"
#include "owner.h"

using namespace std;

Office::Office(const string &property_name, int area, Owner *owner, bool having_wifi, bool having_reception)
{
    this->property_name = property_name;
    this->area = area;
    this->owner = owner;
    this->having_wifi = having_wifi;
    this->having_reception = having_reception;
    if (owner) this->owner->add_property(this);
}

float Office::valuate()
{
    float result = area * 5;
    if (having_wifi) result *= 1.3;
    if (having_reception) result *= 1.5;
    return result;
}
