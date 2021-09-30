#include "clocks.h"
#include <iostream>
#include "clock_time.h"
using namespace std;

Clock::Clock()
{
}

Clock::Clock(int hour, int minute, int second, double dirftPerSecond)
{
	_totalDrift = 0;
	_driftPerSecond = dirftPerSecond;
	_clockTime.setTime(hour, minute, second, 1);
}

Clock::~Clock()
{
}

void Clock::reset()
{
	_clockTime.reset();
}

void Clock::tick()
{
	_clockTime.increment();
	_totalDrift += _driftPerSecond;
}

NaturalClock::NaturalClock()
{
}

NaturalClock::~NaturalClock()
{
	
}

void NaturalClock::displayTime()
{
	_clockTime.display();
	cout << ", total drift : " << _totalDrift << endl;
}

MechanicalClock::MechanicalClock()
{
}

MechanicalClock::~MechanicalClock()
{
}

void MechanicalClock::displayTime()
{
	_clockTime.display();
	cout << ", total drift : " << _totalDrift << endl;
}

DigitalClock::DigitalClock()
{
}

DigitalClock::~DigitalClock()
{
}

void DigitalClock::displayTime()
{
	_clockTime.display();
	cout << ", total drift : " << _totalDrift << endl;
}

QuantumClock::QuantumClock()
{
}

QuantumClock::~QuantumClock()
{
}

void QuantumClock::displayTime()
{
	_clockTime.display();
	cout << ", total drift : " << _totalDrift << endl;
}

SundialClock::SundialClock()
{
}

SundialClock::~SundialClock()
{
}

void SundialClock::printMy()
{
	cout << "SudialClock ";
}

CuckooClock::CuckooClock()
{
}

CuckooClock::~CuckooClock()
{
}

void CuckooClock::printMy()
{
	cout << "CuckooClock ";
}

GrandFatherClock::GrandFatherClock()
{
}

GrandFatherClock::~GrandFatherClock()
{
}

void GrandFatherClock::printMy()
{
	cout << "GrandFatherClock ";
}

WristClock::WristClock()
{
}

WristClock::~WristClock()
{
}

void WristClock::printMy()
{
	cout << "WristClock ";
}

AtomicClock::AtomicClock()
{
}

AtomicClock::~AtomicClock()
{
}

void AtomicClock::printMy()
{
	cout << "AtomicClock ";
}
