#include "accounts.h"



Account::Account()
{
	ID = 0;
	balance = 0;
}

Account::Account(int id)
{
	ID = id;
	balance = 0;
}

Account::~Account()
{
}



int AccountManager::dep(int id, int amount)
{
	int user = -1;
	for (int i = 0; i < 10; i++)
	{
		
		if (id == arr[i].ID) {
			user = i;
			break;
		}
	}
	if (user == -1) {
		cout << "Account does not exist"<< endl << endl;
		return 0;
	}

	if ((this->arr[user].balance + amount) > 1000000) {
		cout << "Failure : Deposit to user " << this->arr[user].ID << " " << amount<< endl;
		this->check(id);
		cout << endl;
		return 0;
	}

	this->arr[user].balance += amount;
	cout << "Success : Deposit to user " << this->arr[user].ID << " " << amount << endl;
	this->check(id);
	cout << endl;
	return 1;
}

int AccountManager::with(int id, int amount)
{
	int user = -1;
	for (int i = 0; i < 10; i++)
	{
		
		if (id == arr[i].ID) {
			user = i;
			break;
		}
	}
	if (user == -1) {
		cout << "Account does not exist" << endl << endl;
		return 0;
	}

	if ((this->arr[user].balance - amount) <0) {
		cout << "Failure : Withdraw to user " << this->arr[user].ID << " " << amount << endl;
		this->check(id);
		cout << endl;
		return 0;
	}

	this->arr[user].balance -= amount;
	cout << "Success : Withdraw to user " << this->arr[user].ID << " " << amount << endl;
	this->check(id);
	cout << endl;
	return 1;
}

int AccountManager::trans(int from, int to,int amount)
{
	int user_f = -1;
	for (int i = 0; i < 10; i++)
	{
		
		if (from == arr[i].ID) {
			user_f = i;
			break;
		}
	}

	int user_t = -1;
	for (int i = 0; i < 10; i++)
	{
		
		if (to == arr[i].ID) {
			user_t = i;
			break;
		}
	}
	if (user_f == -1 || user_t==-1) {
		cout << "Account does not exist" << endl << endl; 
		return 0;
	}

	if ((this->arr[user_f].balance - amount) <0 || (this->arr[user_t].balance + amount)> 1000000) {
		cout << "Failure : Transfer from user " << this->arr[user_f].ID << " to user "<< this->arr[user_t].ID << " "<<amount << endl;
		//cout << user_f << " " << user_t << endl;
		this->check(user_f);
		this->check(user_t);
		cout << endl;
		return 0;
	}
	

	this->arr[user_f].balance -= amount;
	this->arr[user_t].balance += amount;
	cout << "Success : Transfer from user " << this->arr[user_f].ID << " to user " << this->arr[user_t].ID << " " << amount << endl;
	//cout << user_f << " " << user_t << endl;
	this->check(user_f);
	this->check(user_t);
	cout << endl;

	return 1;

}

void AccountManager::check(int id)
{
	int user = -1;
	for (int i = 0; i < 10; i++)
	{
		
		if (id == arr[i].ID) {
			user = i;
			break;
		}
	}
	//cout << user;
	
	cout << "Balance of user " << arr[user].ID << " : " << arr[user].balance << endl ;
}

AccountManager::AccountManager()
{
	user =-1;
}

AccountManager::~AccountManager()
{
}
