#include "number.h"

int Square::getSquare()
{
	int sq;
	sq = (this->_num)*(this->_num);
	return sq;
}

int Cube::getCube()
{
	int cu;
	cu= (this->_num)*(this->_num)*(this->_num);
	return cu;
}
