#include <iostream>

using namespace std;

void getSumDiff(int a, int b, int &sum, int& diff);

int main() {
	int a, b;
	int sum, diff;
	cin >> a >> b;
	getSumDiff(a, b, sum, diff);
	cout << "sum: " << sum << endl << "diff: " << diff << endl;

}

void getSumDiff(int a, int b, int & sum, int & diff)
{
	sum = a + b;
	diff = a - b;
	
}
