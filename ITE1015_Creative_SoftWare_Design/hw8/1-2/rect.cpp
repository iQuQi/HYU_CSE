#include "rect.h"
#include <iostream>

using namespace std;
Rectangle::Rectangle(int width, int height)
{
	
	wid = width;
	hei = height;
}

int Rectangle::getArea()
{
	int area;
	area = (hei)*(wid);
	return area;
}

int Rectangle::getPerimeter()
{
	int peri;
	peri = (hei)*2+(wid)*2;
	return peri;
}

Square::Square(int width):Rectangle(width,width)
{
	
}

void Square::print()
{
	cout << (wid) << "x" << (wid) << " Square" << endl;
}

NonSquare::NonSquare(int width, int height):Rectangle(width, height)
{
	
}

void NonSquare::print()
{
	cout << (wid) << "x" << (hei) << " NonSquare" << endl;
}

