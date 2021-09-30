#include <iostream>
#include <string>

using namespace std;
void swapInt(int &n1, int &n2);
void swapString(string& s1, string &s2);

int main() {
	int n1, n2;
	cin >> n1 >> n2;
	string s1, s2;
	cin >> s1 >> s2;

	cout << "n1: " << n1 << ", n2: " << n2 << ", s1: " << s1 << ", s1: " << s2 << endl;

	swapInt(n1, n2);
	swapString(s1, s2);

	cout << "n1: " << n1 << ", n2: " << n2 << ", s1: " << s1 << ", s1: " << s2;

}

void swapInt(int & n1, int & n2)
{
	int temp;
	temp = n1;
	n1 = n2;
	n2 = temp;
}

void swapString(string & s1, string & s2)
{
	string temp;
	temp = s1;
	s1 = s2;
	s2 = temp;
}
