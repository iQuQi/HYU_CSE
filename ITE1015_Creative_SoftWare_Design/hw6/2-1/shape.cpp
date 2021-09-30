#include "shape.h"


float Rectangle::areaRec()
{
	float area;
	float garo,sero;
	garo = (this->x_bot) - (this->x_top);
	sero = (this->y_top) - (this->y_bot);
	area = garo * sero;
	return area;
}

float Rectangle::periRec()
{
	float peri;
	float garo, sero;
	garo = (this->x_bot) - (this->x_top);
	sero = (this->y_top) - (this->y_bot);

	peri = 2 * (garo + sero);
	return peri;
}

Rectangle::Rectangle()
{
}

Rectangle::~Rectangle()
{
}

float Circle::areaCir()
{
	float area;
	area = (this->rad)*(this->rad)*PI;
	return area;
}

float Circle::periCir()
{
	float peri;
	peri = 2 * (this->rad)*PI;
	return peri;
}

Circle::Circle()
{
}

Circle::~Circle()
{
}
