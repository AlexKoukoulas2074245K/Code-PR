#include "fpscounter.h"
#include "common.h"
#pragma comment(lib, "winmm.lib")

FpsCounter::FpsCounter(): mFps(0), mCount(0), mStartTime(timeGetTime()){}
FpsCounter::~FpsCounter(){}

void FpsCounter::Update()
{
	
	mCount++;
	unsigned long currTime = timeGetTime();
	if (currTime >= (mStartTime + 1000))
	{
		mFps = mCount;
		LOGLN(mFps);
		mStartTime = currTime;
		mCount = 0;
	}
}



