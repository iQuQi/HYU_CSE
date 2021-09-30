#include <iostream>

using namespace std;

int main() {

	int n;
	int min, max;
	cin >> n;
	int *arr = new int[n];
	for (int i = 0; i < n; i++) {
		cin >> arr[i];
		if (i == 0) {
			min = arr[i];
			max = arr[i];
		}
		else {
			if (min >= arr[i]) min = arr[i];
			if (max <= arr[i]) max = arr[i];
		}

	}

	cout << "min: " << min;
	cout << "max: " << max;

	delete[] arr;



}