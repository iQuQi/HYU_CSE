#pragma once
class Shape
{
public:
	Shape();
	Shape(double wid_, double hei_);
	~Shape();
	virtual double getArea()=0;
protected:
	int hei;
	int wid;
};

class Triangle : public Shape {
public:
	Triangle();
	Triangle(double wid_,double hei_);
	~Triangle();
	double getArea(); 

};

class Rectangle : public Shape {
public:
	Rectangle();
	Rectangle(double wid_,double hei_);
	~Rectangle();
	double getArea();

};