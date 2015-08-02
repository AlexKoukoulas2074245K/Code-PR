#pragma once
#include <wrl\client.h>
#include <comdef.h>
#include <string>
#include <d3d11.h>
#include <dxerr.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcommon.h>
#include <d3dx10.h>
#include <dxerr.h>
#include <xnamath.h>

#include "winconfig.h"

#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

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
#ifndef HR
#define HR(x) (x)
#endif

#define comptr Microsoft::WRL::ComPtr

namespace d3dconst
{
	static const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const DXGI_FORMAT DEPTH_BUFFER_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
	static const D3D11_DSV_DIMENSION DEPTH_BUFFER_DIMENSION = D3D11_DSV_DIMENSION_TEXTURE2D;
	static const DXGI_FORMAT INDEX_FORMAT = DXGI_FORMAT_R32_UINT;
	static const DXGI_FORMAT POSITION_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
	static const DXGI_FORMAT TEXCOORD_FORMAT = DXGI_FORMAT_R32G32_FLOAT;
	static const DXGI_FORMAT NORMAL_FORMAT = DXGI_FORMAT_R32G32B32_FLOAT;
	static const D3D11_PRIMITIVE_TOPOLOGY PRIMITIVES = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	static const unsigned int MS_COUNT = 1;
	static const unsigned int MS_QUAL = 0;
	static const float BG_COLOR[4] = {184.0f / 255.0f, 136.0f / 255.0f, 249.0f / 255.0f, 1.0f};
	static const char* const VS_PROFILE = "vs_5_0";
	static const char* const PS_PROFILE = "ps_5_0";
	static const char* const SHADER_FUNC = "main";
}