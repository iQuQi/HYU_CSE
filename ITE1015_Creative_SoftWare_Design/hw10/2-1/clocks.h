#pragma once
#include "clock_time.h"

class Clock {
protected :
	ClockTime _clockTime;
	double _driftPerSecond;
	double _totalDrift;
public:
	Clock();
	Clock(int hour, int minute, int second, double dirftPerSecond);
	virtual ~Clock();
	void reset();
	void tick();
	virtual void displayTime() = 0;
	virtual void printMy()=0;

};

class NaturalClock : public Clock{
public:
	NaturalClock();
	NaturalClock(int hour, int minute, int second, double dirftPerSecond) :Clock(hour, minute, second, dirftPerSecond) {}
	virtual ~NaturalClock();
	virtual void displayTime();
};

class MechanicalClock : public Clock {
public:
	MechanicalClock();
	MechanicalClock(int hour, int minute, int second, double dirftPerSecond) :Clock(hour, minute, second, dirftPerSecond) {}
	virtual ~MechanicalClock();
	virtual void displayTime();
};

class DigitalClock : public Clock {
public:
	DigitalClock();
	DigitalClock(int hour, int minute, int second, double dirftPerSecond) :Clock(hour, minute, second, dirftPerSecond) {}
	virtual ~DigitalClock();
	virtual void displayTime();
};

class QuantumClock : public Clock {
public:
	QuantumClock();
	QuantumClock(int hour, int minute, int second, double dirftPerSecond) :Clock(hour, minute, second, dirftPerSecond) {}
	virtual ~QuantumClock();
	virtual void displayTime();
};

class SundialClock : public NaturalClock {
public:
	SundialClock();
	SundialClock(int hours, int minutes, int seconds) :NaturalClock(hours, minutes, seconds,0.0) {}
	~SundialClock();
	void printMy();
};

class CuckooClock : public MechanicalClock {
public:
	CuckooClock();
	CuckooClock(int hours, int minutes, int seconds) :MechanicalClock(hours, minutes, seconds,0.000694444) {}
	~CuckooClock();
	void printMy();

};

class GrandFatherClock : public MechanicalClock {
public:
	GrandFatherClock();
	GrandFatherClock(int hours, int minutes, int seconds) :MechanicalClock(hours, minutes, seconds,0.000347222) {}
	~GrandFatherClock();
	void printMy();
};

class WristClock : public DigitalClock {
public:
	WristClock();
	WristClock(int hours, int minutes, int seconds) :DigitalClock(hours, minutes, seconds,0.000034722) {}
	~WristClock();
	void printMy();


};

class AtomicClock : public QuantumClock {
public:
	AtomicClock();
	AtomicClock(int hours, int minutes, int seconds) :QuantumClock(hours, minutes, seconds,0.0) {}
	~AtomicClock();
	void printMy();

};