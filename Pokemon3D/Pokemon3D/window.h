#pragma once
#include <windows.h>

/* FULL SCREEN SWITCH */
//#define FULL_SCREEN

inline DWORD getTime(){ return timeGetTime(); }

namespace window
{
#ifdef FULL_SCREEN
	static const bool FULLSCREEN = true;
	static const unsigned long WIDTH = GetSystemMetrics(SM_CXSCREEN);
	static const unsigned long HEIGHT = GetSystemMetrics(SM_CYSCREEN);
#else
	static const bool FULLSCREEN = false;
	static const unsigned long WIDTH = 1000;
	static const unsigned long HEIGHT = 562;
#endif
	static const bool VSYNC = true;
	static const char* const APP_NAME = "Pokemon3D";
	static const unsigned long PEL_BIT = 32;
}