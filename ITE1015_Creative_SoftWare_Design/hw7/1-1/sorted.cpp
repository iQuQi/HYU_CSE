#include "sorted.h"



sortedArray::sortedArray()
{
}

sortedArray::~sortedArray()
{
}

void sortedArray::AddNumber(int num)
{
	numbers_.push_back(num);
	return;
}

vector<int> sortedArray::GetSortedAscending()
{

	
	sort(numbers_.begin(), numbers_.end());
	return numbers_;
}

vector<int> sortedArray::GetSortedDescending()
{


	sort(numbers_.begin(), numbers_.end(),greater<int>());
	return numbers_;
}

int sortedArray::GetMax()
{
	
	vector<int>::iterator it;
	it = max_element(numbers_.begin(),numbers_.end());
	
	return *it;
}

int sortedArray::GetMin()
{
	vector<int>::iterator it;
	it = min_element(numbers_.begin(), numbers_.end());

	return *it;
}
