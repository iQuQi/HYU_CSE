#include <iostream>
#include <string>
using namespace std;

int main() {

	string fir, sec,con;

	cin >> fir;
	cin >> sec;

	con = fir + sec;

	int strlen;
	strlen = size(con);



	cout << con << endl;
	cout << con[0]<< endl;
	cout << con[strlen-1] << endl;



}