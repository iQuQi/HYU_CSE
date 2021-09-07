#include <iostream>
#include <string>

using namespace std;

typedef struct {
	string name;
	double score;

}Person;

int main(int argc, char ** argv) {

	argc -= 1;
	Person * arr = new Person[argc / 2];

	for (int i = 0; i < argc / 2; i++)
	{
		arr[i].name = argv[2(i + 1) + 1];
		arr[i].score = atof(argv[2(i + 1)]);
		cout << "Name : " << arr[i].name << ", Score : " << arr[i].score << endl;
		cout << "Name : " << arr[i].name << ", Score : " << arr[i].score << endl;
	}





	delete[] arr;
}
