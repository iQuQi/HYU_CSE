#pragma once
#include <string>
using namespace std;
class Animal
{
public:
	string name;
	int age;
	Animal();
	Animal(string _name,int _age);
	~Animal();
	virtual	void printInfo();
};

class Zebra : public Animal
{
public:
	int numStripes;
	Zebra();
	Zebra(string _name, int _age ,int num);
	~Zebra();
	virtual	void printInfo();
};

class Cat : public Animal
{
public:
	string favoriteToy;
	Cat();
	Cat(string _name, int _age , string favor);
	~Cat();
	virtual	void printInfo();
};


