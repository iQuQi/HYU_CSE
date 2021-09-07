#include <iostream>
#include <string>
using namespace std;
template <typename T>
void myswap(T &fir,T &sec) {
	T temp;
	temp = fir;
	fir = sec;
	sec = temp;

}

int main() {
	
	int a, b;
	cin >> a >> b;
	myswap(a, b);
	cout << "After calling myswap() : " << a << " " << b <<endl;

	double c, d;
	cin >> c >> d;
	myswap(c, d);
	cout << "After calling myswap() : " << c << " " << d<<endl;

	string st1, st2;
	cin >> st1>> st2;
	myswap(st1, st2);
	cout << "After calling myswap() : " << st1 << " " << st2<<endl;

}