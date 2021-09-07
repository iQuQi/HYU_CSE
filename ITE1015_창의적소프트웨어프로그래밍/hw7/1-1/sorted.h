#pragma once
using namespace std;
#include <vector>
#include <algorithm>


class sortedArray
{
public:
	sortedArray();
	~sortedArray();

	void AddNumber(int num);

	vector<int> GetSortedAscending();
	vector<int> GetSortedDescending();
	int GetMax();
	int GetMin();

private:
	vector<int> numbers_;

};

