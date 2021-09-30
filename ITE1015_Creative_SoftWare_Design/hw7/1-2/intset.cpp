#include "intset.h"





intgerSet::intgerSet()
{
}

intgerSet::~intgerSet()
{
}

void intgerSet::AddNumber(int num)
{	
	
	for (int i = 0; i < numbers_.size(); i++)
	{
		if (num == numbers_[i]) numbers_.erase(numbers_.begin() + i);
	}
	this->numbers_.push_back(num);
	sort(numbers_.begin(), numbers_.end());
}

void intgerSet::DeleteNumber(int num)
{
	
	for (int i = 0; i < numbers_.size(); i++)
	{
		if (num == numbers_[i]) numbers_.erase(numbers_.begin() + i);
	}
	sort(numbers_.begin(), numbers_.end());
}

int intgerSet::GetItem(int pos)
{
	int re;
	if (pos > numbers_.size() - 1) re = -1;
	else re = numbers_[pos];
	return re;
}

vector<int> intgerSet::GetAll()
{
	return numbers_;
}
