#pragma once
#include <memory>

/* Typedefs */
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#define uptr std::unique_ptr
#define sptr std::shared_ptr

/* Build Dependent */
#ifdef _DEBUG
#include <Windows.h>
inline void LOG(const char* const out){ OutputDebugString(out); }
inline void LOGLN(const char* const out){ OutputDebugString(">>> "); LOG(out); LOG("\n"); }
inline void LOGLN(const std::string& out){ OutputDebugString(">>> "); LOG(out.c_str()); LOG("\n"); }
inline void LOGLN(const double out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const float out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const uint out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const ulong out){ LOGLN(std::to_string(out)); }

#else
inline void LOG(const char* const out){}
inline void LOGLN(const char* const out){}
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
}

