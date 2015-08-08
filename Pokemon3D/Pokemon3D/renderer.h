#pragma once
#include <map>
#include <list>

#include "d3dcommon.h"
#include "config.h"
#include "pokedef.h"
#include "common.h"
#include "shader.h"
#include "camera.h"
#include "colors.h"

/* Convenience macros */
#define IACTIVE_SHADER m_shaders[m_activeShaderType]
#define H_ACTIVE_VERTEX_SHADER IACTIVE_SHADER.getVertexShader()
#define P_ACTIVE_VERTEX_SHADER H_ACTIVE_VERTEX_SHADER.Get()
#define PP_ACTIVE_VERTEX_SHADER H_ACTIVE_VERTEX_SHADER.GetAddressOf()
#define H_ACTIVE_PIXEL_SHADER IACTIVE_SHADER.getPixelShader()
#define P_ACTIVE_PIXEL_SHADER H_ACTIVE_PIXEL_SHADER.Get()
#define PP_ACTIVE_PIXEL_SHADER H_ACTIVE_PIXEL_SHADER.GetAddressOf()

class Body;
class HUDComponent;
class StaticModel;
class Texture;
class Renderer
{
public:
	enum ShaderType
	{
		DEFAULT, HUD, NONE
	};

public:
	static Renderer& get();

public:
	bool initialize(const HWND hWindow);
	bool loadFontImage(const std::string& imagePath, Texture* fontImage);
	bool prepareModel(StaticModel* model);
	bool prepareHUD(HUDComponent* hudc);
	bool prepareObject(const ShaderType shader, Body* body);
	void renderModel(StaticModel* model);
	void renderHUD(
		HUDComponent* component,
		bool enableColor = false,
		float4 color = COLOR_BLACK);
	void renderText(
		const std::string& chars,
		const float2& startPos,
		float4 color = COLOR_BLACK);
	void prepareFrame(
		const mat4x4& currView,
		const mat4x4& currProj,
		const vec4f& currCamPos,
		const Camera::Frustum& currCamFrustum);
	void completeFrame();

private:
	Renderer();
	Renderer(const Renderer&) = delete;
	void operator=(const Renderer&) = delete;

private:
	bool preInitialization(const HWND& hWindow, uint& outrrNum, uint& outrrDen);
	bool coreInitialization(const HWND& hWindow, const uint rrNum, const uint rrDen);
	bool isVisible(const Body* b, const float3& pos);
	bool shaderInitialization();
	bool layoutInitialization();
	void changeActiveLayout(const ShaderType shader);
	void enableHUDRendering();
	void disableHUDRendering();
	void renderObject(
		const ShaderType shader,
		const float3& pos,
		const float3& rot,
		const bool hud,
		Body* body,
		bool enableColor = false,
		float4 color = COLOR_BLACK);

private:
	comptr<ID3D11Device> m_pDevice;
	comptr<ID3D11DeviceContext> m_pDevcon;
	comptr<IDXGISwapChain> m_pSwapchain;
	comptr<ID3D11RenderTargetView> m_pBackBuffer;
	comptr<ID3D11Texture2D> m_pDepthBuffer;
	comptr<ID3D11DepthStencilState> m_pDepthStencilState;
	comptr<ID3D11DepthStencilState> m_pDisabledDepthState;
	comptr<ID3D11DepthStencilView> m_pDepthStencilView;
	comptr<ID3D11BlendState> m_pBlendState;
	comptr<ID3D11InputLayout> m_pGlobalDefaultLayout;
	comptr<ID3D11InputLayout> m_pGlobalHUDLayout;
	comptr<ID3D11RasterizerState> m_pRastState;
	comptr<ID3D11SamplerState> m_pDefaultSampleState;
	comptr<ID3D11SamplerState> m_pHudSampleState;

	bool m_hudRendering;
	
	uint m_videoCardMemory;
	std::string m_videoCardName;
	
	ShaderType m_activeShaderType;
	std::map<ShaderType, Shader> m_shaders;
	std::map<ShaderType, std::string> m_shaderFiles;
	std::map<ShaderType, comptr<ID3D11InputLayout>> m_shaderLayouts;
	
	mat4x4 m_currView;
	mat4x4 m_currProj;
	vec4f m_currCamPosition;
	Camera::Frustum m_currCamFrustum;
};