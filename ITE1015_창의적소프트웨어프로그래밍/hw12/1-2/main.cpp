#include <iostream>
#include <string>
#include "my_container.h"
//#include <vector>
using namespace std;

int main() {
	
	int num;
	cin >> num;
	MyContainer<int> co1(num);

	operator>><int>(cin, co1);
	operator<<<int>(cout, co1);

	cin >> num;
	MyContainer<double> co2(num);
	operator>><double>(cin, co2);
	operator<<<double>(cout, co2);

	cin >> num;
	MyContainer<string> co3(num);
	operator>><string>(cin, co3);
	operator<<<string>(cout, co3);
	

	



}