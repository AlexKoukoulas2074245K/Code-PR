#include <Windows.h>

/* Build Dependent */
#ifdef _DEBUG
#include "vld.h"
inline void LOG(const char* const out){ OutputDebugString(out); }
inline void LOGLN(const char* const out){ LOG(out); LOG("n"); }

#else
inline void LOG(const char* const out){}
inline void LOGLN(const char* const out){}
#endif
 
/* Typedefs */
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/* Finals */
namespace window
{
	static const bool FULLSCREEN = true;
	static const bool VSYNC = true;
	static const char* const APP_NAME = "Pokemon3D";
	static const ulong WIDTH = GetSystemMetrics(SM_CXSCREEN);
	static const ulong HEIGHT = GetSystemMetrics(SM_CYSCREEN);
	static const ulong PEL_BIT = 32;
}

/* Utility */
namespace util
{
	template<class A>
	struct pair{ A a, b; };
}

