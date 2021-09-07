#pragma once
#define PI 3.14

class Circle
{
public:
	int x;
	int y;
	int rad;
public:
	float areaCir();
	float periCir();
	Circle();
	~Circle();
};

class Rectangle
{
public:
	int x_top;
	int y_top;
	int x_bot;
	int y_bot;
public:
	float areaRec();
	float periRec();
	Rectangle();
	~Rectangle();
};


