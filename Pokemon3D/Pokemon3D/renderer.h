#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcommon.h>
#include <d3dx10.h>
#include <dxerr.h>
#include "common.h"
#include "window.h"

/* D3D library linking */
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")


class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize(const HWND hWindow);
	void ClearFrame();

private:
	bool PreInitialization(const HWND& hWindow, uint& outrrNum, uint& outrrDen);
	bool CoreInitialization(const HWND& hWindow, const uint rrNum, const uint rrDen);

private:
	comptr<ID3D11Device> mDevice;
	comptr<ID3D11DeviceContext> mDevcon;
	comptr<IDXGISwapChain> mSwapchain;
	comptr<ID3D11RenderTargetView> mBackBuffer;
	comptr<ID3D11Texture2D> mDepthBuffer;
	comptr<ID3D11DepthStencilState> mDepthStencilState;
	comptr<ID3D11DepthStencilView> mDepthStencilView;
	comptr<ID3D11BlendState> mBlendState;
	
	uint mVideoCardMemory;
	std::string mVideoCardName;
};