#pragma once
#include <string>
using namespace std;
class Shape {
public:
	Shape() {};
	Shape(int _x, int _y, int _wid, int _hei ,string _bru);
	double GetArea() {};
	int GetPerimeter() {};
	void Draw(int canvas_width, int canvas_height) {};
protected:
	int x,y;
	int wid, hei;
	string bru;
	
};

class Square : public Shape  {
public:
	Square() {};
	Square(int _x,int _y,int _wid ,string _bru);
	double GetArea();
	int GetPerimeter();
	void Draw(int _garo,int _sero);

};

class Rectangle : public Shape {
public:
	Rectangle() {};
	Rectangle(int _x, int _y, int _wid, int _hei,string _bru);
	double GetArea();
	int GetPerimeter();
	void Draw(int _garo, int _sero);

};

class Diamond :public Shape {
public:
	Diamond() {};
	Diamond(int _x, int _y, int _dis, string _bru);
	double GetArea();
	int GetPerimeter();
	void Draw(int _garo, int _sero);

};



	//isinshape 함수 만들어주기