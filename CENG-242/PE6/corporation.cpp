#include <iostream>
#include "corporation.h"

using namespace std;

Corporation::Corporation(const string &name, float balance, string address)
{
    this->name = name;
    this->balance = balance;
    this->address = address;
}

void Corporation::print_info()
{
    cout << "Name: " << name << " Address: " << address << endl;
}
