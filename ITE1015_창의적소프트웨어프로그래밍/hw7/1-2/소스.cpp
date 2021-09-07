#include <iostream>
#include <string>
#include "intset.h"
using namespace std;

int main() {

	string menu;
	intgerSet set;
	

	while (1) {
		cin >> menu;
		if (menu == "add") {
			//cout << "己傍";
			cin >> menu;
			//cout << "己傍2";
			int num = atoi(menu.c_str());
			//cout << num;
			set.AddNumber(num);
			vector<int> re;
			re=set.GetAll();
			//cout << "己傍3";
			for  (vector<int>::iterator it=re.begin(); it!=re.end() ; it++)
			{
				//cout << "己傍4";
				cout << *it << " ";
			}
			cout << endl;
		}
		else if (menu == "del") {
			cin >> menu;
			int num = atoi(menu.c_str());
			set.DeleteNumber(num);
			vector<int> re2;
			re2 = set.GetAll();
			for (vector<int>::iterator it = re2.begin(); it != re2.end(); it++)
			{
				cout << *it << " ";
			}
			cout << endl;
		}
		
		else if (menu == "get") {
			cin >> menu;
			int num = atoi(menu.c_str());
			cout << set.GetItem(num) << endl;
		}
		else if (menu == "quit") break;
		
	
	}



}