#include <iostream>
#include <string>
#include "rect.h"
using namespace std;

int main() {
	while (1) {
		string menu;
		cin >> menu;
		if (menu == "quit") break;

		if (menu == "nonsquare") {
			
			int fir,sec;
			cin >> fir >> sec;
			NonSquare non(fir, sec);
			non.print();
			cout << "Area : " << non.getArea()<<endl;
			cout << "Perimeter : " << non.getPerimeter()<<endl;
		}
		else if (menu == "square") {
			
			int fir;
			cin >> fir;
			Square sq(fir);
			sq.print();
			cout << "Area : " << sq.getArea()<<endl;
			cout << "Perimeter : " << sq.getPerimeter() << endl; 
		}
		



	}


}