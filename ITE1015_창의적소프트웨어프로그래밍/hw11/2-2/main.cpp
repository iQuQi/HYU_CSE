#include <iostream>
#include <string>
#include "my_vector2.h"
using namespace std;

int main() {
	
	
	int len;
	string menu1;
	string  oper, sec;
	cin >> menu1;

	if (menu1 == "quit") return 0;
	//cout << "¼º°ø";
	cin >> len;
	//cout << " " << len;
	MyVector2 a2(len);
	MyVector2 a(a2);
	MyVector2 b2(len);
	MyVector2 b(b2);
		
	cout << "enter a" << endl;
	for (int i = 0; i < len; i++)
	{
		cin >> a;
	}

	cout << "enter b" << endl;
	for (int i = 0; i < len; i++)
	{
		cin >> b;
	}

	while (menu1!="quit") {
		
		cin >> menu1;
		

		

		if (menu1 == "a") {
			cin >> oper >> sec;
			if (oper == "+") {

				if (sec == "b") {
					MyVector2 a1(a);
					MyVector2 a1_(a1 + b);
					cout << a1_ << endl;
				}
				else if (sec == "a") {
					MyVector2 a2(a);
					MyVector2 a2_(a2 + a);
					cout << a2_ << endl;
				}
				else {
					MyVector2 a3(a);
					MyVector2 a3_(a3 + stoi(sec));
					
					cout << a3_ << endl;
				}

			}
			else if (oper == "-") {

				if (sec == "b") {
					MyVector2 a1(a);
					MyVector2 a1_(a1 - b);
					cout << a1_ << endl;
				}
				else if (sec == "a") {
					MyVector2 a2(a);
					MyVector2 a2_(a2 - a);
					cout << a2_ << endl;
				}
				else {
					MyVector2 a3(a);
					MyVector2 a3_(a3 - stoi(sec));
					cout << a3_ << endl;
				}

			}
		}

		else if (menu1 == "b") {
			cin >> oper >> sec;
			if (oper == "+") {

				if (sec == "b") {
					MyVector2 b1(b);
					MyVector2 b1_(b1 + b);
					cout << b1_ << endl;
				}
				else if (sec == "a") {
					MyVector2 b2(b);
					MyVector2 b2_(b2 + a);
					cout << b2_ << endl;
				}
				else {
					MyVector2 b3(b);
					MyVector2 b3_(b3 + stoi(sec));
					cout << b3_ << endl;
				}
			}
			else if (oper == "-") {

				if (sec == "b") {
					MyVector2 b1(b);
					MyVector2 b1_(b1 - b);
					cout << b1_ << endl;

				}
				else if (sec == "a") {
					MyVector2 b2(b);
					MyVector2 b2_(b2 - a);
					cout << b2_ << endl;
				}
				else {
					MyVector2 b3(b);
					MyVector2 b3_(b3 - stoi(sec));
					cout << b3_ << endl;
				}
			}

		}






	}

	return 0;

}