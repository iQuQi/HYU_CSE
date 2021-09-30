#include <iostream>
#include <string>
#include <vector>
#include  "shape.h"
using namespace std;

int main() {
	string input;
	vector<Shape*>arr;
	while (1) {
		cin >> input;
		
		if (input=="r")
		{
			int wid_, hei_;
			cin >> wid_ >> hei_;
			arr.push_back(new Rectangle(wid_, hei_));
		}
		else if (input == "t") {


			int wid_, hei_;
			cin >> wid_ >> hei_;
			arr.push_back(new Triangle(wid_, hei_));
		}
		else if (input == "0") {
			for (int i = 0; i < arr.size(); i++)
			{
				cout << arr[i]->getArea()<<endl;

			}
			for (int i = 0; i < arr.size(); i++)
			{
				delete arr[i];
			}
			break;
			
		}


	}




}