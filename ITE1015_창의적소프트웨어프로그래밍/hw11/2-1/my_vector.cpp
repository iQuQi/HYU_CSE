#include "my_vector.h"
#include <iostream>
using namespace std;
MyVector::MyVector()
{
	
}

MyVector::MyVector(int length)
{
	this->length = length;
	this->a = new double[length];
	for (int i = 0; i < length; i++)
	{
		*(a + i) = 0;
		//cout << "for 성공 " << a[i] << endl;
	}
}

MyVector::~MyVector()
{	
	//cout << "소멸자 호출" << endl;
	delete[] a;
}

MyVector & MyVector::operator=(const MyVector & b)
{
	this->length = b.length;
	
	this->a = new double[length];
	
	for (int i = 0; i < length; i++)
	{
		this->a[i] = b.a[i];
	}
	
	return *this;
}

MyVector MyVector::operator+(const MyVector & b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] += b.a[i];
	}
	return *this;
}

MyVector MyVector::operator-(const MyVector & b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] -= b.a[i];
	}
	return *this;
}

MyVector MyVector::operator+(const int b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] += b;
	}
	return *this;
}

MyVector MyVector::operator-(const int b)
{
	for (int i = 0; i < length; i++)
	{
		this->a[i] -= b;
	}
	return *this;
}

std::ostream & operator<<(std::ostream & out, MyVector & b)
{

	for (int i = 0; i < b.length; i++)
	{
		out << b.a[i] << " ";
	}

	return out;
}

std::istream & operator>>(std::istream & in, MyVector & b)
{
	for (int i = 0; i < b.length; i++)
	{
		if (b.a[i] == 0) {
			in >> b.a[i];
		}
	}
	return in;
}
