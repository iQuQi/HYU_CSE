#include <iostream>
#include <string>
#include "my_vector.h"
using namespace std;

int main() {
	MyVector a, b;
	MyVector a_, b_;
	int len;
	while (1) {
		string menu1;
		string  oper, sec;
		cin >> menu1;

		

		if (menu1 == "new") {
			//cout << "己傍";
			cin >> len;
			//cout << " " << len;
			MyVector a2(len);
			a = a2;
			MyVector b2(len);
			b = b2;
			//cout << "己傍";
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
		
		}

		else if (menu1 == "quit") {
			break;
		}

		else if (menu1 == "a") {
			cin >> oper >> sec;
			if (oper == "+") {
				
				if (sec == "b") {
					a_ = a;
					a_ = a_ + b;
					cout << a_ << endl; 
				}
				else if(sec=="a"){
					a_ = a;
					a_ = a_ + a;
					cout << a_ <<endl;
				}
				else {
					//cout << "己傍1" << endl;
					a_ = a;
					//cout << "己傍2" << endl;
					a_ = a_ + stoi(sec);
					//cout << "己傍3" << endl;
					cout << a_ <<endl;
				}
				
			}
			else if (oper == "-") {
				
				if (sec == "b") {
					a_ = a;
					a_ = a_ - b;
					cout << a_ <<endl;
				}
				else if(sec=="a"){
					a_ = a;
					a_ = a_ - a;
					cout << a_<<endl;
				}
				else {
					a_ = a;
					a_ = a_ - stoi(sec);
					cout << a_<<endl;
				}

			}
		}
			
		else if (menu1 == "b") {
			cin >> oper >> sec;
			if (oper == "+"){
				
				if (sec == "b") {
					b_ = b;
					b_ = b_ + b;
					cout << b_<<endl;
				}
				else if(sec=="a") {
					b_ = b;
					b_ = b_ + a;
					cout << b_<<endl;
				}
				else {
					b_ = b;
					b_ = b_ + stoi(sec);
					cout << b_<<endl;
				}
			}
			else if (oper == "-") {
				
				if (sec == "b") {
					b_ = b;
					b_ = b_ - b;
					cout << b_<<endl;
					
				}
				else if(sec=="a"){
					b_ = b;
					b_ = b_ - a;
					cout << b_<<endl; 
				}
				else {
					b_ = b;
					b_ = b_ - stoi(sec);
					cout << b_ <<endl;
				}
			}

		}
			
		
		
		


	}


}