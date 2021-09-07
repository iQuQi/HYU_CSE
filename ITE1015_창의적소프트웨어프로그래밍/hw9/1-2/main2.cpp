#include <iostream>
#include <string>
#include <vector>
using namespace std;
string a_s = "This is an instance of class A";
string b_s = "This is an instance of class B";
string c_s = "This is an instance of class C";
class A {
public:
	virtual string getTypeInfo() {
		return a_s;
	}
};
class B : public A {
public:
	virtual string getTypeInfo() {
		return b_s;
	}
};
class C : public B {
public:
	virtual string getTypeInfo() {
		return c_s;
	}

};
void printObjectTypeInfo1(A* object) {
	cout << object->getTypeInfo()<<endl;
}
void printObjectTypeInfo2(A& object) {
	cout << object.getTypeInfo()<<endl;
}

int main() {
	vector<A*> arr;
	arr.push_back(new A);
	arr.push_back(new B);
	arr.push_back(new C);
	for (int i = 0; i < arr.size(); i++)
	{
		printObjectTypeInfo1(arr[i]);
		printObjectTypeInfo2(*(arr[i]));
	}
	for (int i = 0; i < arr.size(); i++)
	{
		delete arr[i];
	}
}