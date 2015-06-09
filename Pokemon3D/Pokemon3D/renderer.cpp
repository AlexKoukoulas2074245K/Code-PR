#include "renderer.h"

Renderer::Renderer(){}
Renderer::~Renderer(){}

bool Renderer::Initialize(const HWND hWindow)
{
	uint rrNum, rrDen;
	if (!PreInitialization(hWindow, rrNum, rrDen)) return false;
	if (!CoreInitialization(hWindow, rrNum, rrDen)) return false;
	return true;
}

void Renderer::ClearFrame()
{
	mSwapchain->Present(1, 0);
	mDevcon->ClearRenderTargetView(mBackBuffer.Get(), d3d::BG_COLOR);
	mDevcon->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

bool Renderer::PreInitialization(const HWND& hWindow, uint& outrrNum, uint& outrrDen)
{
	/* DXGI factory creation */
	comptr<IDXGIFactory> factory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
	
	/* Crerate graphics card adapter */
	comptr<IDXGIAdapter> adapter;
	HR(factory->EnumAdapters(0, &adapter));
	
	/* Enum the primary adapter output */
	comptr<IDXGIOutput> adapterOutput;
	HR(adapter->EnumOutputs(0, &adapterOutput));
	
	/* Get number of modes that fit the unorm display format */
	uint numModes;
	HR(adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		NULL));
	
	/* Fill display modes */
	DXGI_MODE_DESC* dispModeList = new DXGI_MODE_DESC[numModes];
	HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, dispModeList));
	
	/* Find appropriate display mode for the current window resolution */
	for (size_t i = 0; i < numModes; i++)
	{
		if (dispModeList[i].Width == (uint) window::WIDTH &&
			dispModeList[i].Height == (uint) window::HEIGHT)
		{
			outrrNum = dispModeList[i].RefreshRate.Numerator;
			outrrDen = dispModeList[i].RefreshRate.Denominator;
		}
	}
	delete[] dispModeList;

	/* Get video card description */
	DXGI_ADAPTER_DESC adapterDesc = {};
	HR(adapter->GetDesc(&adapterDesc));

	/* Store video card memory */
	mVideoCardMemory = (uint) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	/* Store video card name */
	mVideoCardName = util::wstringToString(adapterDesc.Description);
	LOGLN(mVideoCardName);
	LOGLN(mVideoCardMemory);

	return true;
}

bool Renderer::CoreInitialization(const HWND& hWindow, const uint rrNum, const uint rrDen)
{
	/* Swap chain description */
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = window::WIDTH;
	scd.BufferDesc.Height = window::HEIGHT;
	scd.BufferDesc.Format = d3d::BACK_BUFFER_FORMAT;
	scd.BufferDesc.RefreshRate.Numerator = window::VSYNC ? rrNum : 0;
	scd.BufferDesc.RefreshRate.Denominator = window::VSYNC ? rrDen : 1;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = d3d::MS_COUNT;
	scd.SampleDesc.Quality = d3d::MS_QUAL;
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.Windowed = !window::FULLSCREEN;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	/* Swap chain device and device context creation */
	HR(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&mSwapchain,
		&mDevice,
		NULL,
		&mDevcon));
	
	/* Create render target view*/
	comptr<ID3D11Texture2D> backBufferPtr;
	HR(mSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBufferPtr));
	HR(mDevice->CreateRenderTargetView(backBufferPtr.Get(), NULL, &mBackBuffer));

	/* Create depth buffer */
	D3D11_TEXTURE2D_DESC dbd = {};
	dbd.Width = window::WIDTH;
	dbd.Height = window::HEIGHT;
	dbd.MipLevels = 1;
	dbd.ArraySize = 1;
	dbd.Format = d3d::DEPTH_BUFFER_FORMAT;
	dbd.SampleDesc.Count = d3d::MS_COUNT;
	dbd.SampleDesc.Quality = d3d::MS_QUAL;
	dbd.Usage = D3D11_USAGE_DEFAULT;
	dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dbd.CPUAccessFlags = 0;
	dbd.MipLevels = 0;
	HR(mDevice->CreateTexture2D(&dbd, NULL, &mDepthBuffer));
	
	/* Create and set the custom depth Stencil State */
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
	mDevcon->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

	/* Create the depth stencil view */
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = d3d::DEPTH_BUFFER_FORMAT;
	dsvd.ViewDimension = d3d::DEPTH_BUFFER_DIMENSION;
	dsvd.Texture2D.MipSlice = 0;
	HR(mDevice->CreateDepthStencilView(mDepthBuffer.Get(), &dsvd, &mDepthStencilView));
	
	/* Set the newly created render target view along with the depth stencil view */
	mDevcon->OMSetRenderTargets(1, mBackBuffer.GetAddressOf(), mDepthStencilView.Get());

	/* Create and set the blend state */
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd.IndependentBlendEnable = FALSE;
	bd.AlphaToCoverageEnable = TRUE;
	HR(mDevice->CreateBlendState(&bd, &mBlendState));
	mDevcon->OMSetBlendState(mBlendState.Get(), 0, 0xFFFFFFFF);

	/* Create and set the viewport */
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(window::WIDTH);
	viewport.Height = static_cast<FLOAT>(window::HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDevcon->RSSetViewports(1, &viewport);

	return true;
}