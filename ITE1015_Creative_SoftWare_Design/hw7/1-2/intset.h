#pragma once
using namespace std;
#include <algorithm>
#include <vector>

class intgerSet
{
public:
	intgerSet();
	~intgerSet();
	void AddNumber(int num);
	void DeleteNumber(int num);

	int GetItem(int pos);
	vector<int> GetAll();


private:
	vector<int> numbers_;
	
};

