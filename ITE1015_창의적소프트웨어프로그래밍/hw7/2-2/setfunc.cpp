#include "setfunc.h"


std::set<int> parseSet(const std::string & str)
{
	set<int> setnew;
	int size= str.size();
	string strok;
	
	//cout << size << endl;
	int fir = 1;
	while (1) {
		
		size_t pos = str.find(" ",fir);
		strok = str.substr(fir,pos-fir);
		//cout << strok << endl;
		//cout << str << endl;
		fir = pos + 1;
		//cout << "while 안임" << endl;
		setnew.insert(stoi(strok));
		

		if (pos+1==size) break;
	}
		
	//cout << "while 밖임" << endl;
	//printSet(setnew);
	return setnew;
}

std::set<int> getIntersection(const std::set<int>& set0, const std::set<int>& set1)
{
	set<int> inter;
	set<int> mid1;
	set<int> mid2;
	mid1 = getDifference(set0, set1);
	//printSet(mid1);
	inter = getDifference(set0, mid1);
	//printSet(inter);



	return inter;
}

std::set<int> getUnion(const std::set<int>& set0, const std::set<int>& set1)
{
	set<int> uni;
	set<int>::iterator it0 = set0.begin();
	set<int>::iterator it1 = set1.begin();
	int size1 = set0.size(), size2 = set1.size();
	for (int i = 0; i < size1; i++)
	{
		uni.insert(*it0);
		it0++;
	}
	for (int i = 0; i < size2; i++)
	{
		uni.insert(*it1);
		it1++;
	}


	return uni;
}

std::set<int> getDifference(const std::set<int>& set0, const std::set<int>& set1)
{
	set<int> dif;
	dif = set0;
	set<int>::iterator it0 = dif.begin();
	//cout << "dif진입 " << endl;
	int size1 = set0.size(), size2 = set1.size();
	for (int i = 0; it0!=dif.end(); i++)
	{
		int count = 1;
		//cout << "dif 처음포문" << endl;
		set<int>::iterator it1 = set1.begin();
		for (int j = 0; j < size2; j++)
		{
			//cout << "dif 두번째포문 " << endl;
			if (*it0 == *it1) {
				count = 0;
				it0 = dif.erase(it0);
				//cout << "dif if 안 " << endl;
				break;
			}
			it1++;
		}
		if(count==1)it0++;
	}
	//cout << "dif탈출 " << endl;
	return dif;
}

void printSet(const std::set<int>& _set) {
	//cout << "print진입 " << endl;
	int size = _set.size();
	set<int>::iterator iter=_set.begin();

	cout << "{ ";
	for (int i = 0; i < size; i++)
	{
		cout << *iter << " ";
		iter++;
	}
	cout << "}\n";
}