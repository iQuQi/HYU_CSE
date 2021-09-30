


#include <iostream>
using namespace std;

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
	Node<T> * tail;
	int size;
public:
	List() : head(NULL) {}
	~List() {}            /*Remove and free all resources*/
	List(T* arr, int n_nodes) : List()
	{
		size = n_nodes;
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
		Node<T>* new_node = new Node<T>;
		new_node->data = data;

		Node<T> * find1 = head;
		Node<T> * find2 = head;
		for (int i = 0; i < idx; i++)
		{
			find1 = find1->next;//제일끝 노드 가리킴
		}
		for (int i = 0; i < idx-1; i++)
		{
			find2 = find2->next;//제일끝에서 한칸앞 노드 가리킴
		}

		new_node->next = find1;
		find2->next = new_node;
		
		//cout << "insert성공";
		size++;

	}
	void remove_at(int idx)
	{

		Node<T> * find1 = head;
		Node<T> * find2 = head;
		for (int i = 0; i < idx; i++)
		{
			find1 = find1->next;//제일끝 노드 가리킴
		}
		for (int i = 0; i < idx-1; i++)
		{
			find2 = find2->next;//제일끝에서 한칸앞 노드 가리킴
		}

		
		find2->next = find1->next;
		delete find1;
		
		//cout << "remov성공";
		size--;

	}
	void pop_back()
	{
		Node<T> * find1 = head;
		Node<T> * find2 = head;
		for (int i = 0; i < size - 1; i++)
		{
			find1 = find1->next;//제일끝 노드 가리킴
		}
		for (int i = 0; i < size - 2; i++)
		{
			find2 = find2->next;//제일끝에서 한칸앞 노드 가리킴
		}
		delete find1;
		find2->next = NULL;

		//cout << "popback성공";
		size--;
	}
	void push_back(T val) // or void push_back(const T& val)
	{
		// allocate new node
		Node<T>* new_node = new Node<T>;
		new_node->data = val;

		// insert new node into head;
		new_node->next = NULL;
		Node<T> * find = head;
		for (int i = 0; i < size-1; i++)
		{
			find = find->next;
		}
		find->next = new_node;
		//cout << "pu_ba성공";
		size++;
	}
	void pop_front()
	{
		Node<T>* temp;
		temp = this->head;
		this->head = head->next;
		delete temp;
		//cout << "pop_fr성공";
		size--;


	}
	void push_front(T val) // or void push_front(const T& val)
	{
		// allocate new node
		Node<T>* new_node = new Node<T>;
		new_node->data = val;

		// insert new node into head;
		new_node->next = this->head;
		this->head = new_node;
		//cout << "pu_fr성공";
		size++;

	}
	// insert const keyword cuz of rvalue problem
	friend std::ostream& operator<<(std::ostream& out, List<T>& rhs)
	{
		Node<T> * iter = rhs.head;
		while (iter) {
			out << iter->data;
			if (!(iter->next == NULL)) {
				 cout << ",";
			}
			iter = iter->next;
		}
		return out;
	}

};

