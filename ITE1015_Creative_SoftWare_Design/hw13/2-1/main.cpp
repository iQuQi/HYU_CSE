#include <iostream>
using namespace std;
class A
{
public:
	A(int n)
	{
		//implement something here
		if (n <= 0) throw 1;
		
		cout << "ID=" << n << ": constructed\n";
		n_ID = n;

		data = new int[n];
		
	
	}
	~A()
	{
		//if (n_ID > 0) {
			cout << "ID=" << n_ID << ": destroyed\n";
			delete data;
		//}
		//implement something here
		
	}
private:
	int* data = NULL;
	int n_ID;
};
int main() {
	try {
		A a(3);
		A b(2);
		{
			A c(1);
			A d(0);
			A e(-1);
		}
	}
	catch (int a) {

		cout << "caught in the main" << endl;
	}
	
	return 0;
}