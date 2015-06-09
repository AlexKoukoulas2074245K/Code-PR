#pragma once
#include <windows.h>
#include <memory>
#include <wrl\client.h>
#include <comdef.h>
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include <dxerr.h>
#pragma comment (lib, "dxerr.lib")

/* Typedefs */
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef HRESULT hres;
#define comptr Microsoft::WRL::ComPtr
#define uptr std::unique_ptr
#define sptr std::shared_ptr

/* Build Dependent */
#ifdef _DEBUG
inline void LOG(const char* const out){ OutputDebugString(out); }
inline void LOGLN(const char* const out){ OutputDebugString(">>> "); LOG(out); LOG("\n"); }
inline void LOGLN(const std::string& out){ OutputDebugString(">>> "); LOG(out.c_str()); LOG("\n"); }
inline void LOGLN(const double out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const float out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const uint out){ LOGLN(std::to_string(out)); }
inline void LOGLN(const ulong out){ LOGLN(std::to_string(out)); }

/* DirectX Hresult trace macro */
#ifndef HR
#define HR(x) \
{ \
HRESULT hr = (x); \
if(FAILED(hr)) \
{ \
DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
} \
}
#endif

#else
inline void LOG(const char* const out){}
inline void LOGLN(const char* const out){}
inline void LOGLN(const double out){}
inline void LOGLN(const float out){}
inline void LOGLN(const uint out){}
inline void LOGLN(const ulong out){}

#ifndef HR
#define HR(x) (x)
#endif
#endif

namespace d3d
{
	static const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT DEPTH_BUFFER_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const D3D11_DSV_DIMENSION DEPTH_BUFFER_DIMENSION = D3D11_DSV_DIMENSION_TEXTURE2D;
	static const uint MS_COUNT = 1;
	static const uint MS_QUAL = 0;
	static const float BG_COLOR[4] = {184.0f / 255.0f, 136.0f / 255.0f, 249.0f / 255.0f, 1.0f};
}

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

