#pragma once

#include <memory>
#include <string>

/* Typedefs */
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#define uptr std::unique_ptr
#define sptr std::shared_ptr

/* Build Dependent */
#ifdef _DEBUG
#include <Windows.h>
#include <fstream>
extern std::ofstream LOGFILE;
inline void START_EXT_LOGGING() { LOGFILE.open("log.txt"); }
inline void STOP_EXT_LOGGING() { LOGFILE.close(); }
inline void LOG(const char* const out){ OutputDebugString(out); LOGFILE << out; }
inline void LOGLN(const char* const out){ OutputDebugString(">>> "); LOG(out); LOG("\n"); }
inline void LOGLN(const std::string& out){ OutputDebugString(">>> "); LOG(out.c_str()); LOG("\n"); }
inline void LOGLN(const double out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const float out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const uint out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const ulong out){ LOGLN(std::to_string(out)); }

#else
inline void START_EXT_LOGGING(){}
inline void STOP_EXT_LOGGING(){}
inline void LOG(const char* const out){}
inline void LOGLN(const char* const out){}
inline void LOGLN(const std::string& out){}
inline void LOGLN(const double out){}
inline void LOGLN(const float out){}
inline void LOGLN(const uint out){}
inline void LOGLN(const ulong out){}
#endif

/* Utility */
namespace util
{
	template<class A>
	struct pair{ A a, b; };
	inline std::string wstringToString(const std::wstring in) 
	{ return std::string(in.begin(), in.end()); }
	inline std::wstring stringToWString(const std::string in) 
	{ return std::wstring(in.begin(), in.end()); }
	inline float maxf(float a, float b){ return a > b ? a : b; }
	/* Standard LERP */
	inline bool lerp(const float current,
				  const float target,
				  const float dt,
				  float& outResult)
	{
		float fdiff = target - current;
		if (fdiff > dt) { outResult += dt; return false; }
		else if (fdiff < -dt) { outResult -= dt; return false; }
		else { outResult = target; return true; }
	}
}

