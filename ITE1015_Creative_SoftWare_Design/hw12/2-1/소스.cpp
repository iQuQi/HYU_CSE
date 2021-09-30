/*
 * note: This code is just an example of assignment 12-2.
 *		Therefore, it is not necessary to use the same code.
 *		I hope it will help you when you do your assignment or studing.
 */



#include <iostream>


template <class T>
class Node
{
public:
	T data;
	Node<T>* next;
};

template <class T>
class List {
private:
	Node<T> * head;
	int size;
public:
	List() : head(NULL) {}
	~List() {}            /*Remove and free all resources*/
	List(T* arr, int n_nodes) : List()
	{
		Node<T>** list = &this->head;  // "list" is a variable to make a list

		for (int i = 0; i < n_nodes; i++) {
			*list = new Node<T>;            // allocate new node
			(*list)->data = arr[i];         // initialize new node with data
			(*list)->next = NULL;           // The last node's next must be NULL;

			list = &((*list)->next);        // manipulate next node
		}

		/*
		 * Is there a way like this?
		 *
		Node<T>*& iter = this->head;
		for (int i = 0 ; i < n_nodes; i++ ){
		 iter = new Node<T>;
		 iter->data = arr[i];
		 iter->next = NULL;
		 &iter = &(iter->next);
		}
		*/

	}

	void insert_at(int idx, T data) // or void insert_at(int idx, const T& data);
	{
	}
	void remove_at(int idx)
	{
	}
	void pop_back()
	{
	}
	void push_back(T val) // or void push_back(const T& val)
	{
	}
	void pop_front()
	{
	}
	void push_front(T val) // or void push_front(const T& val)
	{
		// allocate new node
		Node<T>* new_node = new Node<T>;
		new_node->data = val;

		// insert new node into head;
		new_node->next = this->head;
		this->head = new_node;

	}
	// insert const keyword cuz of rvalue problem
	friend std::ostream& operator<<(std::ostream& out, List<T>& rhs)
	{
		Node<T> * iter = rhs.head;
		while (iter) {
			out << iter->data << " ";
			iter = iter->next;
		}
		return out;
	}

};

using namespace std;
int main() {
	int array[5] = { 12 , 7 , 9 , 21, 13 };
	List<int> li(array, 5);
	cout << li << endl;

	li.push_front(8);
	cout << li << endl;

	li.push_front(9);
	cout << li << endl;
	li.push_front(1);
	cout << li << endl;
	li.push_front(11);
	cout << li << endl;
	return 0;
}

