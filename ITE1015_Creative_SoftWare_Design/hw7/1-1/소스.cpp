#include <iostream>
#include <vector>
#include <string>
#include "sorted.h"
using namespace std;

int main() {
	sortedArray arr;
	string menu;
	
	

	while (1) {
	
		cin >> menu;
		
		
		if (menu == "ascend") {
			//cout << "己傍2" << endl;
			vector<int> re;
			re =arr.GetSortedAscending();
			for(vector<int>::iterator it=re.begin();  it!=re.end(); it++)
			{
				cout << *it << " ";
			}
			cout << endl;

		}
		else if (menu == "descend") {
			//cout << "己傍3" << endl;
			vector<int> re2;
			re2 = arr.GetSortedDescending();
			for (vector<int>::iterator it = re2.begin(); it != re2.end(); it++)
			{
				cout << *it << " ";
			}
			cout << endl;
		}
		else if (menu == "max") {
			//cout << "己傍4" << endl;
			cout << arr.GetMax() << endl;
		}
		else if (menu == "min") {
			//cout << "己傍5" << endl;
			cout << arr.GetMin()<< endl;
		}
		else if (menu == "quit") break;

		else {
			int num = atoi(menu.c_str());
			arr.AddNumber(num);
		}
	}


}