#include <iostream>
#include <string>
#include <vector>
#include  "canvas.h"
using namespace std;

int main() {
	int garo, sero;
	cin >> garo >> sero;
	Canvas can(garo,sero);
	//캔버스 출력하기
	for (int i = 0; i <= garo; i++){
		for (int j = 0; j <= sero; j++){
			if (i == 0 && j == 0) cout << " ";
			else if (i == 0) cout << j - 1;
			else if (j == 0) cout << i - 1;

			else cout << ".";
		}
		cout << endl;
	}

	vector<Shape*> arr;
	//메뉴 받기
	string menu1,menu2;
	while (1) {
		cin >> menu1;
		if (menu1 == "add") {
			cin >> menu2;
			if (menu2 == "rect") {
				int top_left_x, top_left_y, width, height;
				string bru;
				cin >> top_left_x >> top_left_y >> width >> height >> bru;
				arr.push_back(new Rectangle(top_left_x, top_left_y, width, height,bru));
			}
			else if (menu2 == "diamond") {
				int x, y, distance;
				string bru;
				cin >> x >> y >> distance >> bru;
				arr.push_back(new Diamond(x, y, distance, bru));
			}
			else if (menu2 == "tri_up") {
				int x, y, hei;
				string bru;
				cin >> x >> y >> hei >> bru;
				arr.push_back(new UpTriangle(x, y, hei, bru));
			}
			else if (menu2 == "tri_down") {
				int x, y, hei;
				string bru;
				cin >> x >> y >> hei >> bru;
				arr.push_back(new DownTriangle(x,y, hei, bru));

			}
		
		}
		else if (menu1 == "delete") {
			int index;
			cin >> index;
			vector<Shape *>::iterator it=arr.begin();
			if(!(index>=arr.size()||index<0)) arr.erase(it + index);
			

		}
		else if (menu1 == "draw") {
			can.Clear();
			for (int i = 0; i < arr.size(); i++)
			{
				arr[i]->Draw(&can);
			}
			can.Print();
		}
		else if (menu1 == "dump") {
			for (int i = 0; i < arr.size(); i++)
			{
				cout << i << " "; 
				arr[i]->print();
				
			}
		}
		else if (menu1 == "resize") {
			int newHei, newWid;
			cin >> newWid >> newHei;
			can.Resize(newWid, newHei);
		}
		else if (menu1 == "quit") break;
	}



}