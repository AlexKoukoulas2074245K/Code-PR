#pragma once
#include "window.h"

class Clock
{
public:
	Clock();
	~Clock();

	bool  InitClock();
	double GetCounter();

private:
	double freq = 0.0;
	_int64 start = 0;
};