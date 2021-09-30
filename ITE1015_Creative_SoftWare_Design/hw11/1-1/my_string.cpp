#include <string>

#include <iostream>

#include "my_string.h"



using namespace std;



MyString& MyString::operator=(const MyString& string) {

	str = string.str;

	return *this;

}



MyString MyString::operator*(const int b) {

	string a = str;

	for (int i = 1; i < b; i++) {

		str = str + a;

	}

	return *this;

}



MyString MyString::operator+(const MyString& b) {

	str = str + b.str;

	return *this;

}



ostream& operator<<(ostream& out, MyString& my_string) {

	out << my_string.str << endl;

	return out;

}

istream& operator>>(istream& in, MyString& my_string) {

	in >> my_string.str;

	return in;

}