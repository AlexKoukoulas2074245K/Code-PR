#pragma once

class Clock
{
public:
	Clock();
	~Clock();

	bool initClock();
	double getCounter() const;

private:
	double m_freq = 0.0;
	_int64 m_start = 0;
};