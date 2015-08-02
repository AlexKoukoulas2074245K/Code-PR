#pragma once
#include <windows.h>

inline DWORD getTime(){ return timeGetTime(); }

class WindowConfig
{
public:
	WindowConfig(const char* filename);
	static bool FULL_SCREEN;
	static unsigned long WIDTH;
	static unsigned long HEIGHT;
	static bool VSYNC;
	static float ASPECT;
	static char APP_NAME[16];

private:
	static const unsigned long FS_WIDTH;
	static const unsigned long FS_HEIGHT;
	static const unsigned long WD_WIDTH;
	static const unsigned long WD_HEIGHT;
};