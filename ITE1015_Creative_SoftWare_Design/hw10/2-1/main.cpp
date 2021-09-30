#include <iostream>
#include "clocks.h"
#include <vector>
#include "clock_time.h"

using namespace std;

int main() {
	int sec;
	cin >> sec;
	vector<Clock*> arr;

	arr.push_back(new SundialClock(0,0,0));
	arr.push_back(new CuckooClock(0,0,0));
	arr.push_back(new GrandFatherClock(0, 0, 0));
	arr.push_back(new WristClock(0, 0, 0));
	arr.push_back(new AtomicClock(0, 0, 0));

	cout << "Reported clock times after resetting :" << endl;
	for (int i = 0; i < arr.size(); i++)
	{
		
		arr[i]->printMy();
		arr[i]->displayTime(); 
		
	}

	cout << endl << "Running the Clocks . . ." << endl << endl;
	for (int i = 0; i < arr.size(); i++)
	{
		for (int j = 0; j < sec; j++)
		{
			arr[i]->tick();
		}
	}


	cout << "Reported clock time after running :" << endl;
	for (int i = 0; i < arr.size(); i++)
	{
		arr[i]->printMy();
		arr[i]->displayTime();
	}

	for (int i = 0; i < arr.size(); i++)
	{
		delete arr[i];
	}



}