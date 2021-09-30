#include "canvas.h"
#include <string>
#include <iostream>
using namespace std;


Canvas::Canvas(size_t row, size_t col)
{
	row_ = row;
	col_ = col;
	can = new string*[row];
	for (int i = 0; i < row; i++)
	{
		can[i] = new string[col];
		for (int j = 0; j < col; j++)
		{
			
			can[i][j] = ".";
		}
	}
}

Canvas::~Canvas()
{
}

void Canvas::Resize(size_t w, size_t h)
{
	
	for (int i = 0; i < row_; i++)
	{
		delete[] can[i];
	}
	delete[] can;
	row_ = h;
	col_ = w;
	


	can = new string*[h];
	for (int i = 0; i < h; i++)
	{
		can[i] = new string[w];
		for (int j = 0; j < w; j++)
		{
			can[i][j] = ".";
		}
	}
}

bool Canvas::DrawPixel(int x, int y)
{
	bool isok = true;
	if (x > col_-1 || x<0 || y>row_-1 || y < 0) isok = false;
	return  isok;
}

void Canvas::Print()
{

	for (int i = 0; i <= row_; i++)
	{
		for (int j = 0; j <= col_; j++)
		{
			if (i == 0 && j == 0) cout << " ";
			else if (i == 0) cout << (j - 1)%10;
			else if (j == 0) cout << (i - 1)%10;
			else cout << can[i - 1][j - 1];
		}
		cout << endl;
	}
}

void Canvas::Clear()
{	
	for (int i = 0; i < row_; i++)
	{
		for (int j = 0; j < col_; j++)
		{
			can[i][j] = ".";
		}
	}
}

Shape::~Shape()
{
}


void Shape::print()
{
}



Rectangle::Rectangle(int x, int y, int wid, int hei, string brush)
{
	_x = x;
	_y = y;
	_hei = hei;
	_wid = wid;
	_bru = brush;

}

Rectangle::~Rectangle()
{
}

void Rectangle::Draw(Canvas * canvas)
{
	
	for (int i = 0; i < _hei; i++){
		for (int j = 0; j < _wid; j++){
			if (canvas->DrawPixel(_x + j, _y + i)) {
				canvas->can[_y + i][_x + j] = _bru;
			}
			else continue;
		}
	}
	
	
	
	
}

void Rectangle::print()
{
	cout << "rect " << _x << " " << _y << " " << _wid << " " << _hei << " " << _bru << endl;
}

UpTriangle::UpTriangle(int x, int y, int hei,string bru)
{
	_x = x;
	_y = y;
	_hei = hei;
	_wid = 2 * hei - 1;
	_bru = bru;
}

UpTriangle::~UpTriangle()
{

}

void UpTriangle::Draw(Canvas * canvas)
{
	for (int i = 0; i < _hei; i++)
	{
		for (int j = -i; j <= i; j++)
		{
			if (canvas->DrawPixel(_x+j,_y+i)) {
				canvas->can[_y + i][_x + j] = _bru;
			}
			else continue;
			
		}
	}
	
}

void UpTriangle::print()
{
	cout << "tri up " << _x << " " << _y << " " <<_hei <<" "<< _bru << endl;
}

DownTriangle::DownTriangle(int x, int y, int hei,string bru)
{
	_x = x;
	_y = y;
	_hei = hei;
	_wid = 2 * hei - 1;
	_bru = bru;
	
}

DownTriangle::~DownTriangle()
{
}

void DownTriangle::Draw(Canvas * canvas)
{
	for (int i =_hei; i >0; i--)
	{
		for (int j =-(_hei-i); j <= (_hei-i); j++)
		{
			if (canvas->DrawPixel(_x + j, _y - (_hei - i))) {
				canvas->can[_y - (_hei - i)][_x + j] = _bru;
			}
			else continue;
			

		}
	}
	
}

void DownTriangle::print()
{
	cout << "tri down " << _x << " " << _y << " " <<_hei << " "<< _bru << endl;
}


Diamond::Diamond(int x, int y, int dis, string brush)
{
	_x = x;
	_y = y;
	_hei = dis*2+1;
	_wid = dis * 2+1;
	_bru = brush;

}

Diamond::~Diamond()
{
}

void Diamond::Draw(Canvas * canvas)
{
	int center_x = _x;
	int center_y = _y + _hei / 2;
	for (int i = 0; i < canvas->row_ ; i++) {
		for (int j = 0; j < canvas->col_; j++) {
			if (canvas->DrawPixel(j,i)){
				if (_x - (_hei / 2 - abs(center_y - i)) <= j && j <= _x + (_hei / 2 - abs(center_y - i)) && _y <= i && i < _y + _hei)  canvas->can[i][j] = _bru;
			}
			else continue;

			
		}
	}
	
}

void Diamond::print()
{
	cout << "diamond" << " " << _x << " " << _y << " " << _hei / 2 << " " << _bru << endl;
}

