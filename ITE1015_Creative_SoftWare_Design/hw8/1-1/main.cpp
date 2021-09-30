#include <iostream>
#include <string>
#include "number.h"
using namespace std;

int main() {

	
	while (1) {
		string menu;
		cin >> menu;
		if (menu == "quit") break;

		int num;
		cin >> num;
		if (menu == "number") {
			Number nu;
			nu.setNumber(num);
			cout << "getNumber() : " << nu.getNumber() << endl;
		}
		else if (menu == "square") {
			Square sq;
			sq.setNumber(num);
			cout << "getNumber() : " << sq.getNumber() << endl;
			cout << "getSquare() : " << sq.getSquare() << endl;
		} 
		else if (menu == "cube") {
			Cube cu;
			cu.setNumber(num);
			cout << "getNumber() : " << cu.getNumber() << endl;
			cout << "getSquare() : " << cu.getSquare() << endl;
			cout << "getCube() : " << cu.getCube() << endl;

		}

	}
	



}