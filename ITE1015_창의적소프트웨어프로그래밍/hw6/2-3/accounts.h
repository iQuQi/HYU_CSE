#pragma once
#include <iostream>
using namespace std;
class Account
{
public:
	int ID;
	int balance;
	
public:
	Account();
	Account(int id);
	~Account();
};

class AccountManager
{
public:
	int user;
	Account *arr = new Account[10];

public:
	int dep(int id, int amount);
	int with(int id,int amount);
	int trans(int from,int to,int amount);
	void check(int id);

	AccountManager();
	~AccountManager();

};

