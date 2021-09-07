#include <iostream>
#include <string>
#include "animal.h"
#include <vector>

using namespace std;

int main() {
	string menu;
	string name;
	int age;
	int nu;
	string fav;
	vector<Animal*> arr;
	while (1) {
		cin >> menu;
		if (menu == "z") {
			cin >> name >> age >> nu;
			arr.push_back(new Zebra(name,age,nu));
		}
		else if (menu == "c") {
			cin >> name >> age >> fav;
			arr.push_back(new Cat(name, age, fav));
		}
		else if (menu == "0") {
			for (int i = 0; i < arr.size(); i++)
			{
				arr[i]->printInfo();
			}
		}
	}


}