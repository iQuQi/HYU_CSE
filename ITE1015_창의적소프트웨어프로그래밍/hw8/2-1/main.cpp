#include <iostream>
#include <string>
#include "shapes.h"
using namespace std;

int main() {
	int garo,sero;
	cin >> garo >> sero;

	while (1) {
		string menu;
		cin >> menu;
		if (menu == "quit") break;


		if (menu == "rect") {
			
			int tl_x, tl_y,wid,hei;
			string bru;
			cin >> tl_x >> tl_y >> wid >> hei >> bru;
			Rectangle rect(tl_x,tl_y,wid,hei,bru);
			cout << "Area : " << rect.GetArea() << endl;
			cout << "Perimeter : " << rect.GetPerimeter() << endl;
			//cout << "성1공" << endl;
			rect.Draw(garo,sero);
			//cout << "성2공" << endl;
		}
		else if (menu == "square") {
			int tl_x, tl_y,len;
			string bru;
			cin >> tl_x >> tl_y >> len >>  bru;
			Square squ(tl_x, tl_y, len, bru);
			cout << "Area : " << squ.GetArea() << endl;
			cout << "Perimeter : " << squ.GetPerimeter() << endl;
			squ.Draw(garo,sero);
		}
		
		else if (menu == "diamond") {
			int tc_x, tc_y,dis;
			string bru;
			cin >> tc_x >> tc_y >> dis >> bru;
			Diamond dia(tc_x, tc_y, dis, bru);
			cout << "Area : " << dia.GetArea() << endl;
			cout << "Perimeter : " << dia.GetPerimeter() << endl;
			dia.Draw(garo,sero);
		}

		
	}


}