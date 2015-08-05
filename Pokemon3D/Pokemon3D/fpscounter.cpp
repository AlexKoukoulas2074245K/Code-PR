#include "fpscounter.h"
#include "common.h"
#pragma comment(lib, "winmm.lib")

FpsCounter::FpsCounter(): 
m_fps(0),
m_count(0),
m_startTime(timeGetTime()){}
FpsCounter::~FpsCounter(){}

void FpsCounter::update()
{	
	m_count++;
	unsigned long currTime = timeGetTime();
	if (currTime >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_startTime = currTime;
		m_count = 0;
	}
}

unsigned int FpsCounter::getFPS() const { return m_fps; }


