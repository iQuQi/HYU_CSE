#include <iostream>
#include <string>
#include "setfunc.h"
using namespace std;

int main() {
	while (1) {
		string fir;
		string menu;
		string menu2;
		string menu3;
		
		cin >> fir;
		if (fir == "0") break;
		getline(cin, menu, '}');
		getline(cin, menu2, '}');
		menu3 = menu2.substr(1, 1);
		menu2 = menu2.substr(4);
		//cout << menu << endl;
		//cout << menu2 << endl;
		//cout << menu3 << endl;

		//cout << "����1 " << endl;
		
		set<int> se1=parseSet(menu);
		//printSet(se1);
		set<int> se2=parseSet(menu2);
		//printSet(se2);
		//cout << "����2 " << endl;

		if (menu3 == "+") {
			//cout << "���� uni" << endl;
			set<int> re =getUnion(se1, se2);
			printSet(re);
		}
		else if (menu3 == "*") {
			//cout << "���� inter" << endl;
			set<int> re = getIntersection(se1,se2);
			printSet(re);
		}
		else if (menu3 == "-") {
			//cout << "���� dif" << endl;
			set<int> re = getDifference(se1, se2);
			printSet(re);
		}
		

	}

}