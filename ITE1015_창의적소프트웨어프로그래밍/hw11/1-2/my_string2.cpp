#include <string>



#include <iostream>



#include "my_string2.h"







using namespace std;







MyString2 MyString2::operator*(const int b) {



	string a = str;



	for (int i = 1; i < b; i++) {



		str = str + a;



	}



	return *this;



}







MyString2 MyString2::operator+(const MyString2& b) {



	str = str + b.str;



	return *this;



}







ostream& operator<<(ostream& out, MyString2& my_string) {



	out << my_string.str << endl;



	return out;



}



istream& operator>>(istream& in, MyString2& my_string) {



	in >> my_string.str;



	return in;



}