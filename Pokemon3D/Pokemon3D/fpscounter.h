#pragma once

class FpsCounter
{
public:
	FpsCounter();
	~FpsCounter();
	void Update();
	
private:
	unsigned int mFps, mCount;
	unsigned long mStartTime;
};