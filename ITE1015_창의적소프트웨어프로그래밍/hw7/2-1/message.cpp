#include "message.h"
#include <iostream>
#include <string>
#include <map>



MessageBook::MessageBook()
{
}

MessageBook::~MessageBook()
{
}

void MessageBook::AddMessage(int number, const string & message)
{
	messages.insert(make_pair(number, message));
}


void MessageBook::DeleteMessage(int number)
{

	map<int, string>::iterator it ;
	it = messages.find(number);
	if(it!=messages.end()) messages.erase(it);
	
}

vector<int> MessageBook::GetNumbers()
{
	vector<int> num;
	map<int,string>::iterator iter;
	for (iter=messages.begin(); iter!=messages.end(); iter++) num.push_back(iter->first);
	return num;
}

const string & MessageBook::GetMessage(int number)
{
	

	map <int, string> ::iterator it2;

	it2 = messages.find(number);

	return it2->second;

	

	// TODO: 여기에 반환 구문을 삽입합니다.
}
