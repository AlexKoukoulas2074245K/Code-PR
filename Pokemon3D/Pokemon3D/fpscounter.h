#pragma once

class FpsCounter
{
public:
	FpsCounter();
	~FpsCounter();
	void Update();
	unsigned int getFPS() const { return mFps; }

private:
	unsigned int mFps, mCount;
	unsigned long mStartTime;
};