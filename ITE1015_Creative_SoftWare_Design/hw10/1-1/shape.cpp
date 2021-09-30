#include "shape.h"


Shape::Shape()
{

}

Shape::Shape(double wid_, double hei_)
{
	

}

Shape::~Shape()
{

}

Triangle::Triangle()
{
}

Triangle::Triangle(double wid_, double hei_)
{
	wid = wid_;
	hei = hei_;
}

Triangle::~Triangle()
{
}

double Triangle::getArea()
{
	double area;
	area = wid * hei / 2;
	return area;
}

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(double wid_, double hei_)
{
	wid = wid_;
	hei = hei_;
}

Rectangle::~Rectangle()
{
}

double Rectangle::getArea()
{
	double area;
	area = wid * hei;
	return area;
}
