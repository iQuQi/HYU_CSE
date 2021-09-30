#include <iostream>

#include <string>

#include "my_string.h"



using namespace std;



int main() {

	string a;

	MyString b;

	string d;

	MyString c;

	string e;

	int f;

	MyString g;

	MyString add;

	MyString mul;



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

					MyString mul_i = b;

					mul = mul_i * f;

					cout << mul;

				}

				else if (e == "+") {

					cin >> g;

					MyString add_i = b;

					add = add_i + c;

					cout << add;

				}

			}

		}



	}

}