#include "animal.h"
#include <iostream>
using namespace std;

Animal::Animal()
{
}

Animal::Animal(string _name, int _age)
{
	name = _name;
	age = _age;
}

Animal::~Animal()
{
}

void Animal::printInfo()
{
}

Zebra::Zebra()
{
}

Zebra::Zebra(string _name, int _age, int num):Animal(_name,_age)
{
	numStripes = num;
}


Zebra::~Zebra()
{
}

void Zebra::printInfo()
{
	cout << "Zebra, Name: " << name << ", Age: " << age << ", Number of stripes: " << numStripes << endl;
}

Cat::Cat()
{
}

Cat::Cat(string _name, int _age, string favor) : Animal(_name, _age)
{
	favoriteToy = favor;
}

Cat::~Cat()
{
}

void Cat::printInfo()
{
	cout << "Cat, Name: " << name << ", Age: " << age << ", Favorite toy: " << favoriteToy << endl;
}
