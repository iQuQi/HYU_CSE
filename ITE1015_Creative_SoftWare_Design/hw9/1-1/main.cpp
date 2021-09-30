#include <iostream>
#include <string>
#include <vector>
using namespace std;
string a_s = "A::test()";
string b_s = "B::test()";
string c_s = "C::test()";


class A {
public:
	virtual string test() {
		return a_s;
	}
	A() {}
	~A() {}
};
class B : public A{
public:
	virtual string test() {
		return b_s;
	}
	B() {}
	~B() {}

};
class C : public B{
public:
	virtual string test() {
		return c_s;
	}
	C() {}
	~C() {}
};
int main() {
	vector<A*> arr;
	arr.push_back(new A);
	arr.push_back(new B);
	arr.push_back(new C);
	for (int i = 0; i< arr.size(); i++)
	{
		cout << arr[i]->test()<<endl;
	}
	delete arr[0];
	delete arr[1];
	delete arr[2];
}