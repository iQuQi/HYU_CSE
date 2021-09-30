#include <iostream>
#include "list.h"
//#include <list>
using namespace std;

int main(){


	//링크드리스트에서는 배열하듯이 head+2이런식의 연산 불가능
	//next에 저장되어있는 주소값을 이용해서만 노드를 이동할 수 있음

	int array[5] = { 12, 7, 9, 21, 13 };
	List<int> li(array, 5);
	cout << li << endl; //12,7,9,21,13
	li.pop_back();
	cout << li << endl; //12,7,9,21
	li.push_back(15);
	cout << li << endl; //12,7,9,21,15
	li.pop_front();
	cout << li << endl; //7,9,21,15
	li.push_front(8);
	cout << li << endl; //8,7,9,21,15
	li.insert_at(4, 19);
	cout << li << endl; //8,7,9,21,19,15
	li.remove_at(1);
	cout << li << endl; //8,9,21,19,15
	return 0;


}