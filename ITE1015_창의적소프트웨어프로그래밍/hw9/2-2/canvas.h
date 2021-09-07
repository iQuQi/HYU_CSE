#pragma once
#include <string>
using namespace std;
class Canvas {
public:
	Canvas(size_t row, size_t col);
	~Canvas();
	void Resize(size_t w, size_t h);
	bool DrawPixel(int x, int y);
	void Print();
	void Clear();

	string **can;
	int row_, col_;
};
class Shape {
public:
	virtual ~Shape();
	virtual void Draw(Canvas* canvas) {};
	virtual void print();
protected:
	string _bru;
	int _x, _y;
	int _hei,_wid;
};
class Rectangle : public Shape {
public:
	Rectangle(int x,int y, int wid ,int hei, string brush);
	//À§¿ÞÁ¡ ÁÂÇ¥
	~Rectangle();
	virtual void Draw(Canvas* canvas);
	virtual void print();
	

};
class UpTriangle : public Shape {
public:
	UpTriangle(int x, int y , int hei,string bru);
	//À§Áß°£Á¡ ÁÂÇ¥
	~UpTriangle();
	virtual void Draw(Canvas* canvas);
	virtual void print();

};
class DownTriangle : public Shape { 
public:
	DownTriangle(int x, int y, int hei,string bru);
	//¾Æ·¡Áß°£Á¡ ÁÂÇ¥
	~DownTriangle();
	virtual void Draw(Canvas* canvas);
	virtual void print();

};
class Diamond : public Shape {
public:
	Diamond(int x, int y, int dis, string brush);
	//À§Áß°£Á¡ ÁÂÇ¥ 
	~Diamond();
	virtual void Draw(Canvas* canvas);
	virtual void print();
	
};
