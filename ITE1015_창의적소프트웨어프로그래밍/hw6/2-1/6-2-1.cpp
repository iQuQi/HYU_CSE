#include <iostream>
#include "shape.h"


using namespace std;

int main() {
	char menu=' ';
	
	
	

	while (menu != 'Q') {
		cout << "shape?" << endl;
		cin >> menu;
		if (menu == 'C') {
			Circle cir;
			cin >> cir.x >> cir.y >> cir.rad;
			cout << "area : " << cir.areaCir() << ", perimeter : " << cir.periCir() << endl;
		}
		else if (menu == 'R') {
			Rectangle rec;
			cin >> rec.x_top >> rec.y_top >> rec.x_bot >> rec.y_bot;
			cout << "area : " << rec.areaRec() << ", perimeter : " << rec.periRec() << endl; 
		}
		cout << endl;
	}
	
	


	
}