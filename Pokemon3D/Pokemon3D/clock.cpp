#include "clock.h"
#include "common.h"

Clock::Clock(){}
Clock::~Clock(){}

bool Clock::initClock()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		LOGLN("Clock initialization failed");
		return false;
	}

	m_freq = double(li.QuadPart) / 1000.0;
	QueryPerformanceCounter(&li);
	m_start = li.QuadPart;

	return true;
}

double Clock::getCounter() const
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - m_start) / m_freq;
}