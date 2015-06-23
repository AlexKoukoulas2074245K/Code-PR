#include "clock.h"
#include "common.h"

Clock::Clock(){}
Clock::~Clock(){}

bool Clock::InitClock()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		LOGLN("Clock initialization failed");
		return false;
	}

	freq = double(li.QuadPart) / 1000.0;
	QueryPerformanceCounter(&li);
	start = li.QuadPart;

	return true;
}

double Clock::GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - start) / freq;
}