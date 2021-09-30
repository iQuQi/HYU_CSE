#include <iostream>
#pragma once

using namespace std;
template <class T>
class MyContainer
{
public:
	MyContainer(int size) {
		n_elements = size;
		obj_arr = new T[n_elements];
		//for (int i = 0; i < n_elements; i++)
		//{
			//obj_arr[i] = NULL;
		//}
	}
		// Implement here
	~MyContainer() {
		delete[] obj_arr;
	}
		// Implement here
	void clear() {

	}
		// Implement here
	int size() {
		return n_elements;
	}
		// Implement here
	template <class U>
	friend std::istream& operator>> (std::istream &in, MyContainer<U> &b);
	template <class U>
	friend std::ostream& operator<< (std::ostream &out, MyContainer<U>&b);

protected:
	T * obj_arr = NULL;
	int n_elements;
};
template<class T>
istream& operator>> (std::istream &in, MyContainer<T> &b)
{
	for (int i = 0; i < b.size(); i++)
	{
		in >> b.obj_arr[i];
	}
	
	return in;
	
	
}
template<class T>
ostream& operator<< (std::ostream &out, MyContainer<T> &b)
{
	for (int i = 0; i < b.n_elements; i++)
	{
		out << b.obj_arr[i] << " ";
	}
	
	out << endl;
	return out;
}