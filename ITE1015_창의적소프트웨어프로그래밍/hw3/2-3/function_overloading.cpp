#include <iostream>
#include <string>
using namespace std;

int add(int a, int b);
string add(string a, string b);


int main() {
	int fir, sec;

	string fir_s, sec_s;

	cin >> fir >> sec >> fir_s >> sec_s;

	cout << add(fir, sec) << endl;
	cout << add(fir_s, sec_s);


}

int add(int a, int b)
{
	return a+b;
}

string add(string a, string b)
{
	string value = a + '-' + b;
	return value;
}
