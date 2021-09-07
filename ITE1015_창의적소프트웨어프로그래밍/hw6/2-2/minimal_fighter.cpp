#include "minimal_fighter.h"
//using namespace std;


int MinimalFighter::hp()
{
	return mHP;
}

int MinimalFighter::power()
{
	
	return mPower;
}

FighterStatus MinimalFighter::status()
{

	return mStatus;
}

void MinimalFighter::setHP(int _hp)
{
	mHP = _hp;
}

void MinimalFighter::hit(MinimalFighter * _enemy)
{
	this->setHP((this->hp()) - _enemy->power());
	_enemy->setHP(_enemy->hp() - this->power());
	if (this->hp() <= 0) this->mStatus = Dead;
	if (_enemy->hp() <= 0) _enemy->mStatus = Dead;

}

void MinimalFighter::attack(MinimalFighter * _enemy)
{
	_enemy->setHP((_enemy->hp()) - this->power());
	this->mPower = 0;
	if (_enemy->hp() <= 0) _enemy->mStatus = Dead;

}

void MinimalFighter::fight(MinimalFighter * _enemy)
{
	while (this->hp() != 0 && _enemy->hp() != 0) {
		this->setHP((this->hp()) - _enemy->power());
		_enemy->setHP(_enemy->hp() - this->power());
	}
	if (this ->hp()<=0) this->mStatus = Dead;
	if (_enemy->hp()<=0) _enemy->mStatus = Dead;
}

MinimalFighter::MinimalFighter()
{
	mHP = 0;
	mPower = 0;
	mStatus = Invalid;
}

MinimalFighter::MinimalFighter(int _hp, int _power)
{
	if (_hp <= 0) {
		cout << "please input positive hp";
		return;
	}
	mHP = _hp;
	mPower = _power;
	mStatus = Alive;
}

MinimalFighter::~MinimalFighter()
{
}
