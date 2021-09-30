#include "my_vector2.h"
#include <iostream>

MyVector2::MyVector2()
{
}

MyVector2::MyVector2(int length)
{
	this->length = length;
	this->a = new double[length];
	for (int i = 0; i < length; i++)
	{
		*(a + i) = 0;
		//cout << "for ¼º°ø " << a[i] << endl;
	}
}

MyVector2::MyVector2(const MyVector2 & mv)
{
	length = mv.length;
	this->a = new double[length];
	for (int i = 0; i < mv.length; i++)
	{
		a[i] = mv.a[i];
	}
}

MyVector2::~MyVector2()
{
	delete[] a;
}

MyVector2 MyVector2::operator+(const MyVector2 & b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] += b.a[i];
	}
	return *this;
}

MyVector2 MyVector2::operator-(const MyVector2 & b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] -= b.a[i];
	}
	return *this;

}

MyVector2 MyVector2::operator+(const int b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] += b;
	}
	return *this;
}

MyVector2 MyVector2::operator-(const int b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] -= b;
	}
	return *this;
}

std ::ostream & operator<<(std::ostream & out, MyVector2 & b)
{

	for (int i = 0; i < b.length; i++)
	{
		out << b.a[i] << " ";
	}

	return out;
}

std ::istream & operator>>(std::istream & in, MyVector2 & b)
{

	for (int i = 0; i < b.length; i++)
	{
		if (b.a[i] == 0) {
			in >> b.a[i];
		}
	}
	return in;
}
