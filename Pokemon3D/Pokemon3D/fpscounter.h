#pragma once

class FpsCounter
{
public:
	FpsCounter();
	~FpsCounter();
	
	void update();
	
	unsigned int getFPS() const;

private:
	unsigned int m_fps, m_count;
	unsigned long m_startTime;
};