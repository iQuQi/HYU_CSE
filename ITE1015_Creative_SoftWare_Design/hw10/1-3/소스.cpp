#include <iostream>
#include <string>
#include <vector>
using namespace std;

class B
{
public:
	virtual ~B() {}
};
class C : public B
{
public:
	void test_C() { std::cout << "C::test_C()" << std::endl; }
};
class D : public B
{
public:
	void test_D() { std::cout << "D::test_D()" << std::endl; }
};

int main() {
	vector<B*> arr;

	string input;
	while (1) {
		cin >> input;
		if (input=="B")
		{
			arr.push_back(new B);
		}
		else if (input == "C") {
			
			arr.push_back(new C);

		}
		else if (input == "D") {
			arr.push_back(new D);

		}
		else if (input == "0") {
			for (int i = 0; i < arr.size(); i++)
			{
				C* pc = dynamic_cast<C*>(arr[i]);
				D* pd = dynamic_cast<D*>(arr[i]);
				if (pc) {
					pc->test_C();
				}
				if (pd) {
					pd->test_D();
				}

			}
			for (int i = 0; i < arr.size(); i++)
			{
				delete arr[i];
			}

		}


	}
	



}