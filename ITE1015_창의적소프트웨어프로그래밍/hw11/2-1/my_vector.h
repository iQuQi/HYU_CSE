#include <iostream>
#ifndef __MY_VECTOR_H__
#define __MY_VECTOR_
//my_vector.h DO NOT modify this class definition
class MyVector {
public:
	// Implement constructor & destructor
	MyVector();
	MyVector(int length);
	~MyVector();
	MyVector&operator=(const MyVector& b);
	MyVector operator+(const MyVector& b);
	MyVector operator-(const MyVector& b);
	MyVector operator+(const int b);
	MyVector operator-(const int b);
	friend std::ostream& operator<< (std::ostream& out, MyVector& b);
	friend std::istream& operator>> (std::istream& in, MyVector& b);
private:
	int length;
	double *a;
};
#endif // __MY_VECTOR_H__
