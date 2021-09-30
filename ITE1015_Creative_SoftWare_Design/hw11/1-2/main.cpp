#include <iostream>

#include <string>

#include "my_string2.h"



using namespace std;



int main() {

	string a;

	MyString2 b;

	string d;

	MyString2 c;

	string e;

	int f;

	MyString2 g;



	cin >> a;

	if (a == "new") {

		cout << "enter a" << endl;

		cin >> b;

		cout << "enter b" << endl;

		cin >> c;

		while (1) {

			cin >> d;

			if (d == "quit") {

				break;

			}

			else {

				cin >> e;

				if (e == "*") {

					cin >> f;

					MyString2 mul_i(b);

					MyString2 mul(mul_i * f);

					cout << mul;

				}

				else if (e == "+") {

					cin >> g;

					MyString2 add_i(b);

					MyString2 add(add_i + c);

					cout << add;

				}

			}

		}



	}

}