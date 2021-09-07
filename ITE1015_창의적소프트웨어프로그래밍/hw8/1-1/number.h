#pragma once
class Number
{
protected:
	int _num;
public:
	void setNumber(int num)
	{
		_num = num;
	}
	int getNumber()
	{
		return _num;
	}
};
class Square : public Number
{
public:
	int getSquare(); // Implemented to return the square of the number
	//specified by setNumber()
};
class Cube : public Square
{
public:
	int getCube(); // Implemented to return the cube of the number
	//specified by setNumber()
};