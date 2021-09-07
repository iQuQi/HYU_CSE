#include <iostream>
#include <string>
#include <vector>
using namespace std;
class A {
private:
	int * memberA;
public:
	A() {}
	A(int a) {
		memberA= new int(a);
		cout << "new memeberA" << endl;
	}
	virtual ~A() {
		delete memberA;
		cout << "delete memberA" << endl;
	}
	virtual void print() {
		cout << "*memberA "<<*memberA<<endl;
	}
	
};
class B : public A {
private:
	double * memberB;
public:
	B() {}
	B(double b) : A(1) {
		memberB = new double(b);
		cout << "new memeberB" << endl;

	}
	~B() {
		delete memberB;
		cout << "delete memberB" << endl;
	}
	void print() {
		A::print();
		cout << "*memberB "<<*memberB << endl;
	}
};
class C : public B {
private:
	string * memberC;
public:
	
	C(){}
	C(string c) : B(1.1) {
		memberC = new string(c);
		cout << "new memeberC" << endl;
	}
	~C() {
		delete memberC;
		cout << "delete memberC" << endl;
	}
	void print() {
		B::print();
		cout <<"*memberC " <<*memberC << endl;
	}

};
int main() {
	int num;
	double num2;
	string str;
	cin >> num >> num2 >> str;

	vector<A*> arr;
	arr.push_back(new A(num));
	arr.push_back(new B(num2));
	arr.push_back(new C(str));
	for (int i = 0; i < arr.size(); i++)
	{
		arr[i]->print();
	}
	for (int i = 0; i < arr.size(); i++)
	{
		delete arr[i];
	}


}