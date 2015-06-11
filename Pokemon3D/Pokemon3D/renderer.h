#pragma once
#include "d3dcommon.h"
#include "ddsloader.h"
#include "window.h"
#include "common.h"
#include "shader.h"
#include "body.h"
#include "camera.h"
#include <map>

/* Convenience macros */
#define IACTIVE_SHADER mShaders[mActiveShaderType]
#define H_ACTIVE_VERTEX_SHADER IACTIVE_SHADER.getVertexShader()
#define P_ACTIVE_VERTEX_SHADER H_ACTIVE_VERTEX_SHADER.Get()
#define PP_ACTIVE_VERTEX_SHADER H_ACTIVE_VERTEX_SHADER.GetAddressOf()
#define H_ACTIVE_PIXEL_SHADER IACTIVE_SHADER.getPixelShader()
#define P_ACTIVE_PIXEL_SHADER H_ACTIVE_PIXEL_SHADER.Get()
#define PP_ACTIVE_PIXEL_SHADER H_ACTIVE_PIXEL_SHADER.GetAddressOf()

class Renderer
{
public:
	enum ShaderType
	{
		DEFAULT, HUD, NONE
	};

	
	Renderer();
	~Renderer();

	bool Initialize(const HWND hWindow);
	bool PrepareBody(Body& body, const ShaderType shader);
	void RenderBody(Body& body, const ShaderType shader);
	void PrepareFrame(const D3DXMATRIX& currProjection, const D3DXVECTOR4& currCamPos);
	void CompleteFrame();

private:
	bool PreInitialization(const HWND& hWindow, uint& outrrNum, uint& outrrDen);
	bool CoreInitialization(const HWND& hWindow, const uint rrNum, const uint rrDen);
	bool ShaderInitialization();
	bool LayoutInitialization();
	void ChangeActiveLayout(const ShaderType shader);

private:
	comptr<ID3D11Device> mDevice;
	comptr<ID3D11DeviceContext> mDevcon;
	comptr<IDXGISwapChain> mSwapchain;
	comptr<ID3D11RenderTargetView> mBackBuffer;
	comptr<ID3D11Texture2D> mDepthBuffer;
	comptr<ID3D11DepthStencilState> mDepthStencilState;
	comptr<ID3D11DepthStencilView> mDepthStencilView;
	comptr<ID3D11BlendState> mBlendState;
	comptr<ID3D11InputLayout> mGlobalDefaultLayout;
	comptr<ID3D11InputLayout> mGlobalHUDLayout;

	uint mVideoCardMemory;
	
	std::string mVideoCardName;
	ShaderType mActiveShaderType;
	std::map<ShaderType, Shader> mShaders;
	std::map<ShaderType, std::string> mShaderFiles;
	std::map<ShaderType, comptr<ID3D11InputLayout>> mShaderLayouts;
	
	D3DXMATRIX mCurrProjection;
	D3DXVECTOR4 mCurrCamPosition;
};