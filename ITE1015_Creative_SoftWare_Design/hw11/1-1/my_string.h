#ifndef __STRING_H__

#define __STRING_H__



// my_string.h - DO NOT modify this class definition

class MyString

{

public:

	// Implement operators

	MyString& operator=(const MyString& b);

	MyString operator+(const MyString& b);

	MyString operator*(const int b);

	friend std::ostream& operator<<(std::ostream& out, MyString&

		my_string);

	friend std::istream& operator>>(std::istream& in, MyString&

		my_string);

private:

	std::string str;

};

#endif //__STRING_H__
