#pragma once
#include "common.h"

/* FULL SCREEN SWITCH */
//#define FULL_SCREEN

namespace window
{

#ifdef FULL_SCREEN
	static const bool FULLSCREEN = true;
	static const ulong WIDTH = GetSystemMetrics(SM_CXSCREEN);
	static const ulong HEIGHT = GetSystemMetrics(SM_CYSCREEN);
#else
	static const bool FULLSCREEN = false;
	static const ulong WIDTH = 1000;
	static const ulong HEIGHT = 562;
#endif
	static const bool VSYNC = true;
	static const char* const APP_NAME = "Pokemon3D";
	static const ulong PEL_BIT = 32;
}