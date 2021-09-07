#include <iostream>
#include "message.h"
using namespace std;

int main() {
	MessageBook mb;
	string str;

	while (1) {
		
		int count = 0;
		count++;
		cin >> str;

		if (str == "add") {
			int num;
			string mes;
			cin >> num;
			getline(cin, mes);
			if (count ==1)mb.AddMessage(num, mes.substr(1));
			else {
				mb.DeleteMessage(num);
				mb.AddMessage(num, mes.substr(1));
			}

			
		}
		else if (str == "delete") {
			int num;
			cin >> num;

			mb.DeleteMessage(num);
			cin.ignore();

		}
		else if (str == "print") {
			int num;
			cin >> num;
			vector<int>rere;
			rere = mb.GetNumbers();
	
			int ch = 0;
			for (int i = 0; i < rere.size(); i++)
			{
				if (rere[i] == num) { 
					ch = 1;
					break;
				};
				
			}
			
			if (ch == 1) {
				cout << mb.GetMessage(num) << endl;
				cin.ignore();
			}
			
			else {
				//cout << "성공 얍" << endl;
				cout << " \n";
			}
			//cout << "성공 끝" << endl;
			
			

		}
		

		
		else if (str == "list") {
			
			vector<int>re;
			re=mb.GetNumbers();
			for (int i=0;i< re.size();i++)
			{
				cout <<re[i] << " : " << mb.GetMessage(re[i]) << endl;
			}

		}
		else if (str == "quit") break;
		
	}


}