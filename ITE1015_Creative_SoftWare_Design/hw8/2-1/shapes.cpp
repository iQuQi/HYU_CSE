#include "shapes.h"
#include <cmath>
#include <iostream>

using namespace std;

Shape::Shape(int _x, int _y, int _wid,int _hei, string _bru)
{
	x = _x;
	y = _y;

	wid = _wid;
	hei = _hei;
	
	bru = _bru;
}


Square::Square(int _x, int _y, int _wid ,string _bru) : Shape(_x,_y,_wid,_wid,_bru){}
Rectangle::Rectangle(int _x, int _y, int _wid,int _hei,string _bru) :Shape(_x, _y, _wid, _hei, _bru) {}
Diamond::Diamond(int _x, int _y, int _dis, string _bru) : Shape(_x, _y, _dis*2+1, _dis*2+1, _bru) {}

double Square::GetArea()
{

	double area;
	area = wid *wid;
	return area;
}

int Square::GetPerimeter()
{
	int peri;
	peri = wid * 4;
	return peri;
}

void Square::Draw(int _garo, int _sero)
{
	for (int i = 0; i < _sero + 1; i++) {
		for (int j = 0; j < _garo + 1; j++) {
			if (i == 0 && j == 0) cout << " ";
			else if (i == 0) cout << j - 1;
			else if (j == 0) cout << i - 1;
			else if (x < j && j <= x + wid && y < i && i <= y + hei) cout << bru;
			else cout << ".";

		}
		cout << endl;
	}
}

double Rectangle::GetArea()
{
	double area;
	area = wid * hei;
	return area;
}

int Rectangle::GetPerimeter()
{
	int peri;
	peri = wid * 2 + hei * 2;
	return peri;
}

void Rectangle::Draw(int _garo, int _sero)
{
	//cout << "¼º°ø";
	for (int i = 0; i < _sero+1; i++) {
		for (int j = 0; j < _garo+1; j++) {
			if (i == 0 && j == 0) cout << " ";
			else if (i == 0) cout << j-1;
			else if (j == 0) cout << i-1;
			else if (x < j && j <= x + wid && y < i && i <= y + hei) cout << bru;
			else cout << ".";

		}
		cout << endl;
	}
}

double Diamond::GetArea()
{
	double area;
	area = (double)(wid * hei) / 2;
	return area;
}

int Diamond::GetPerimeter()
{
	int peri;
	double len;
	len = ((double)wid)/2 * sqrt(hei/2);
	peri = len * 4;
	return peri;
}

void Diamond::Draw(int _garo, int _sero)
{
	int center_x = x ;
	int center_y = y + hei/2;
	//cout << x << " " << y  <<" " << hei/2<< endl;
	//cout <<center_x << " " << center_y << endl;
	for (int i = 0; i < _sero + 1; i++) {
		for (int j = 0; j < _garo + 1; j++) {
			//cout << "i-1 : " << i-1 << " j-1 : " << j-1 << "    ";
			//cout << x - hei / 2 - abs(center_y - (i - 1)) << " " << x + hei / 2 - abs(center_y - (i - 1)) << endl;
			if (i == 0 && j == 0) cout << " ";
			else if (i == 0) cout << j - 1;
			else if (j == 0) cout << i - 1;
			else if (x-(hei/2-abs(center_y-(i-1)))<= j-1 && j-1 <=x+ (hei/2-abs(center_y -(i-1)))&& y<= i-1 && i-1 < y+hei) cout << bru;
			else cout << ".";

		}
		cout << endl;
	}
}
