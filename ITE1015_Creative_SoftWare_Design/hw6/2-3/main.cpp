#include <iostream>
#include "accounts.h"
using namespace std;


int main() {
	char menu=' ';
	
	AccountManager manage;
	int ch_1[10] = { 0 }, ch_2[10] = { 0 }, ch_3[10] = { 0 };
	
	while (menu != 'Q') {
		cout << "Job?" << endl;
		
		cin >> menu;
		if (menu == 'N') {
			
			if (manage.user == 10) {
				cout << "you can't make new account anymore" << endl;
				continue;
			}
			manage.user++;
			//cout << manage.user;
			Account user(manage.user);
			manage.arr[manage.user] = user;
			cout << "Account for user " << manage.arr[manage.user].ID << " registered" << endl;
			
			
			manage.check(manage.arr[manage.user].ID);
			cout << endl;
		}
		else if (menu == 'D') {
			int id;
			int amount;
			cin >> id >> amount;
			if (ch_1[id] == 1) {
				cout << "you can't deposit anymore" << endl << endl;
				continue;
			}
			
			
			ch_1[id]=manage.dep(id, amount);
		
		}
		else if (menu == 'W') {
			int id;
			int amount;
			cin >> id >> amount;
			if (ch_2[id] == 1) {
				cout << "you can't withdraw anymore" << endl << endl;
				continue;
			}
			
			
			ch_2[id]=manage.with(id, amount);
			
		}
		else if (menu == 'T') {
			int from, to, amount;
			cin >> from >> to >> amount;
			if (ch_3[from] ==1) {
				cout << "you can't transfer anymore" << endl << endl;
				continue;
			}
			
		
			ch_3[from]=manage.trans(from, to,amount);
			
			
		}

		
	}
	
	


}


