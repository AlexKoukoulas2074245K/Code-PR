#include "renderer.h"

#include "hudcomponent.h"
#include "staticmodel.h"
#include "body.h"
#include "ddsloader.h"
#include "fontengine.h"
	
#define WIC_TEX

Renderer& Renderer::get()
{
	static Renderer rend;
	return rend;
}

Renderer::Renderer(){}

bool Renderer::initialize(const HWND hWindow)
{
	m_activeShaderType = ShaderType::NONE;
	uint rrNum, rrDen;
	if (!preInitialization(hWindow, rrNum, rrDen)) return false;
	if (!coreInitialization(hWindow, rrNum, rrDen)) return false;
	if (!shaderInitialization()) return false;
	if (!layoutInitialization()) return false;

	return true;
}

bool Renderer::loadFontImage(const std::string& imagePath, Texture* fontImage)
{
	Texture texture;
	HR(DirectX::CreateWICTextureFromFile(
		m_pDevice.Get(),
		m_pDevcon.Get(),
		util::stringToWString(imagePath.c_str()).c_str(),
		NULL,
		&texture.modTexture(),
		0));
	*fontImage = texture;
	return true;
}

void Renderer::prepareFrame(
	const mat4x4& currView,
	const mat4x4& currProj,
	const vec4f& currCamPos,
	const Camera::Frustum& currCamFrustum)
{
	m_pDevcon->ClearRenderTargetView(m_pBackBuffer.Get(), d3dconst::BG_COLOR);
	m_pDevcon->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	m_currView = currView;
	m_currProj = currProj;
	m_currCamPosition = currCamPos;
	m_currCamFrustum = currCamFrustum;
	m_nObjectsCulled = 0U;
}

void Renderer::completeFrame()
{
	if (AppConfig::VSYNC)
		m_pSwapchain->Present(1, 0);
	else
		m_pSwapchain->Present(0, 0);
}

void Renderer::changeActiveLayout(const ShaderType shaderType)
{
	m_activeShaderType = shaderType;
	m_pDevcon->VSSetShader(P_ACTIVE_VERTEX_SHADER, NULL, NULL);
	m_pDevcon->PSSetShader(P_ACTIVE_PIXEL_SHADER, NULL, NULL);
	m_pDevcon->IASetInputLayout(m_shaderLayouts[m_activeShaderType].Get());
}

void Renderer::enableHUDRendering()
{
	if (m_hudRendering) return;
	m_hudRendering = true;
	m_pDevcon->OMSetDepthStencilState(m_pDisabledDepthState.Get(), 1);
	m_pDevcon->PSSetSamplers(0, 1, m_pHudSampleState.GetAddressOf());
}

void Renderer::disableHUDRendering()
{
	if (!m_hudRendering) return;
	m_hudRendering = false;
	m_pDevcon->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1);
	m_pDevcon->PSSetSamplers(0, 1, m_pDefaultSampleState.GetAddressOf());
}

void Renderer::renderHUD(
	HUDComponent* hudc,
	bool enableColor /* false */,
	float4 color /* COLOR_BLACK */)
{
	enableHUDRendering();
	float2 hudcPos = hudc->getPosition();
	float3 finalPos = float3{hudcPos.x, hudcPos.y, 0.0f};
	float3 finalRot = {};
	renderObject(
		ShaderType::HUD,
		finalPos, 
		finalRot,
		true,
		hudc->getModBodyPointer(),
		enableColor, color);
}

void Renderer::renderText(
	const std::string& chars,
	const float2& startPos,
	float4 color /* COLOR_BLACK */)
{
	float2 posCounter = {};
	posCounter.x = startPos.x;
	posCounter.y = startPos.y;
	for (std::string::const_iterator citer = chars.begin();
		 citer != chars.end();
		 ++citer)
	{
		switch (*citer)
		{
			case ' ':
			{
				posCounter.x += FontEngine::FONT_SIZE / AppConfig::ASPECT;
			}break;

			case '\n':
			{
				posCounter.x = startPos.x;
				posCounter.y -= FontEngine::FONT_SIZE;
			}break;

			default:
			{
				HUDComponent* glyphComp = FontEngine::get().modGlyphCompPointer(*citer);
				glyphComp->setDimensions(glyphComp->getBody().getInitDims().x, glyphComp->getBody().getInitDims().y);
				glyphComp->setPosition(posCounter.x, posCounter.y);
				renderHUD(glyphComp, true, color);
				posCounter.x += FontEngine::FONT_SIZE / AppConfig::ASPECT;
			}break;
		}
	}
}

void Renderer::renderModel(StaticModel* model)
{
	disableHUDRendering();
	renderObject(ShaderType::DEFAULT, model->getPos(), model->getRot(), false, model->modBodyPointer());
}

void Renderer::renderBillboard(StaticModel* model)
{
	disableHUDRendering();
	m_pDevcon->PSSetSamplers(0, 1, m_pHudSampleState.GetAddressOf());
	renderObject(
		ShaderType::DEFAULT,
		model->getPos(),
		model->getRot(),
		false,
		model->modBodyPointer(),
		false,
		pokecolors::COLOR_BLACK,
		true);
}

void Renderer::renderObject(
	const ShaderType shader,
	const float3& pos,
	const float3& rot,
	const bool hud,
	Body* body,
	bool enableColor /* false */,
	float4 color /* COLOR_BLACK */,
	bool billboard /* false */)
{
	if (!body->isReady() || (!hud && !billboard && !isVisible(body, pos))) return;
	if (m_activeShaderType != shader) changeActiveLayout(shader);

	UINT stride = sizeof(Body::Vertex);
	UINT offset = 0;
	m_pDevcon->IASetVertexBuffers(0, 1, body->modVertexBuffer().GetAddressOf(), &stride, &offset);
	m_pDevcon->IASetIndexBuffer(body->getIndexBuffer().Get(), d3dconst::INDEX_FORMAT, 0U);
	
	mat4x4 matTrans, matRotX, matRotY, matRotZ, matScale;
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);
	D3DXMatrixRotationX(&matRotX, rot.x);
	D3DXMatrixRotationY(&matRotY, rot.y);
	D3DXMatrixRotationZ(&matRotZ, rot.z);

	float3 bodyInDims = body->getInitDims();
	float3 bodyAcDims = body->getDimensions();

	if (hud || billboard) D3DXMatrixScaling(
				&matScale,
				(bodyAcDims.x / bodyInDims.x) / AppConfig::ASPECT,
				bodyAcDims.y / bodyInDims.y,
				1.0f);
	else 	D3DXMatrixScaling(
				&matScale,
				bodyAcDims.x / bodyInDims.x,
				1.0f,
				bodyAcDims.z / bodyInDims.z);

	D3DXMATRIX matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
	D3DXMATRIX matFinal = hud ? matWorld : matWorld * m_currView * m_currProj;

	Shader::MatrixBuffer mb = {};
	mb.camPosition = m_currCamPosition;
	mb.finalMatrix = matFinal;
	mb.rotMatrix = matRotX * matRotY * matRotZ;
	mb.worldMatrix = matWorld;

	Shader::ColorBuffer cb = {};
	cb.enableColor = enableColor;
	cb.color = color;
	
	m_pDevcon->PSSetShaderResources(0, 1, body->getActiveTexture().immTexture().GetAddressOf());
	m_pDevcon->VSSetConstantBuffers(0, 1, IACTIVE_SHADER.getMatrixBuffer().GetAddressOf());
	m_pDevcon->PSSetConstantBuffers(0, 1, IACTIVE_SHADER.getColorBuffer().GetAddressOf());
	m_pDevcon->UpdateSubresource(IACTIVE_SHADER.getMatrixBuffer().Get(), NULL, NULL, &mb, NULL, NULL);
	m_pDevcon->UpdateSubresource(IACTIVE_SHADER.getColorBuffer().Get(), NULL, NULL, &cb, NULL, NULL);
	m_pDevcon->IASetPrimitiveTopology(d3dconst::PRIMITIVES);
	m_pDevcon->DrawIndexed(body->getIndexCount(), 0, 0);
}

bool Renderer::prepareModel(StaticModel* model)
{
	return prepareObject(ShaderType::DEFAULT, model->modBodyPointer());
}

bool Renderer::prepareHUD(HUDComponent* hudc)
{
	return prepareObject(ShaderType::HUD, hudc->getModBodyPointer());
}

bool Renderer::prepareObject(const ShaderType shader, Body* body)
{
	comptr<ID3D11Buffer>& bodyVB = body->modVertexBuffer();
	comptr<ID3D11Buffer>& bodyIB = body->modIndexBuffer();
	
	/* Vertex Buffer creation */
	D3D11_BUFFER_DESC vbd = {};
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(Body::vertex_t) * body->getVertexCount();
	
	/* Need to create a placeholder vector for vertex list element traversal */
	D3D11_SUBRESOURCE_DATA vsrd = {};
	std::vector<Body::Vertex> tempVertices(
		body->getVertices().begin(),
		body->getVertices().end());
	vsrd.pSysMem = &tempVertices[0];
	
	HR(m_pDevice->CreateBuffer(&vbd, &vsrd, &bodyVB));

	/* Index Buffer creation */
	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(Body::index_t) * body->getIndexCount();
	
	/* Need to create a placeholder vector for index list element traversal */
	D3D11_SUBRESOURCE_DATA isrd = {};
	std::vector<Body::index_t> tempIndices(
		body->getIndices().begin(),
		body->getIndices().end());
	isrd.pSysMem = &tempIndices[0];

	HR(m_pDevice->CreateBuffer(&ibd, &isrd, &bodyIB));

	/* Load textures: remove from toload and assign to texlist */
	Body::textoload_list& toload = body->modTexturesToLoad();
	Body::texture_list& texlist = body->modTextures();

	while (!toload.empty())
	{
		Texture texture;

#ifdef WIC_TEX
		HR(DirectX::CreateWICTextureFromFile(
			m_pDevice.Get(),
			m_pDevcon.Get(),
			util::stringToWString(toload.front()).c_str(),
			NULL,
			&texture.modTexture(),
			0));
#else
		HR(D3DX11CreateShaderResourceViewFromFile(
			mDevice.Get(),
			toload.front().c_str(),
			NULL, 
			NULL,
			&texture.modTexture(), 
			NULL));
#endif
		texlist.push_back(texture);
		toload.pop_front();
	}

	return true;
}

bool Renderer::preInitialization(const HWND& hWindow, uint& outrrNum, uint& outrrDen)
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
		if (dispModeList[i].Width == (uint) AppConfig::WIDTH &&
			dispModeList[i].Height == (uint) AppConfig::HEIGHT)
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
	m_videoCardMemory = (uint) (adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	/* Store video card name */
	m_videoCardName = util::wstringToString(adapterDesc.Description);
	LOGLN(m_videoCardName);
	LOGLN(m_videoCardMemory);

	return true;
}

bool Renderer::coreInitialization(const HWND& hWindow, const uint rrNum, const uint rrDen)
{
	/* Swap chain description */
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = AppConfig::WIDTH;
	scd.BufferDesc.Height = AppConfig::HEIGHT;
	scd.BufferDesc.Format = d3dconst::BACK_BUFFER_FORMAT;
	scd.BufferDesc.RefreshRate.Numerator = AppConfig::VSYNC ? rrNum : 0;
	scd.BufferDesc.RefreshRate.Denominator = AppConfig::VSYNC ? rrDen : 1;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (AppConfig::MULTISAMPLING) scd.SampleDesc = {4, 0};
	else scd.SampleDesc = {1, 0};
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.Windowed = !AppConfig::FULL_SCREEN;
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
		&m_pSwapchain,
		&m_pDevice,
		NULL,
		&m_pDevcon));
	
	/* Create render target view*/
	comptr<ID3D11Texture2D> backBufferPtr;
	HR(m_pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBufferPtr));
	HR(m_pDevice->CreateRenderTargetView(backBufferPtr.Get(), NULL, &m_pBackBuffer));

	/* Create depth buffer */
	D3D11_TEXTURE2D_DESC dbd = {};
	dbd.Width = AppConfig::WIDTH;
	dbd.Height = AppConfig::HEIGHT;
	dbd.MipLevels = 1;
	dbd.ArraySize = 1;
	dbd.Format = d3dconst::DEPTH_BUFFER_FORMAT;
	if (AppConfig::MULTISAMPLING) dbd.SampleDesc = {4, 0};
	else dbd.SampleDesc = {1, 0};
	dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
	HR(m_pDevice->CreateTexture2D(&dbd, NULL, &m_pDepthBuffer));
	
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
	HR(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState));
	depthStencilDesc.DepthEnable = false;
	HR(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDisabledDepthState));
	
	/* Create the depth stencil view */
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = d3dconst::DEPTH_BUFFER_FORMAT;
	if (AppConfig::MULTISAMPLING) dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	else dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	HR(m_pDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), &dsvd, &m_pDepthStencilView));
	
	/* Set the newly created render target view along with the depth stencil view */
	m_pDevcon->OMSetRenderTargets(1, m_pBackBuffer.GetAddressOf(), m_pDepthStencilView.Get());

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
	HR(m_pDevice->CreateBlendState(&bd, &m_pBlendState));
	m_pDevcon->OMSetBlendState(m_pBlendState.Get(), 0, 0xFFFFFFFF);

	D3D11_SAMPLER_DESC sd;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = AppConfig::ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.BorderColor[0] = 0.0f;
	sd.BorderColor[1] = 0.0f;
	sd.BorderColor[2] = 0.0f;
	sd.BorderColor[3] = 0.0f;
	sd.MinLOD = 0.0f;
	sd.MaxLOD = FLT_MAX;
	sd.MipLODBias = 0.0f;

	m_pDevice->CreateSamplerState(&sd, &m_pDefaultSampleState);
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	m_pDevice->CreateSamplerState(&sd, &m_pHudSampleState);
	m_pDevcon->PSSetSamplers(0, 1, m_pDefaultSampleState.GetAddressOf());

	/* Create the custom rasterizer state */
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FrontCounterClockwise = FALSE;
	rastDesc.DepthClipEnable = TRUE;
	rastDesc.ScissorEnable = FALSE;
	rastDesc.AntialiasedLineEnable = TRUE;
	rastDesc.MultisampleEnable = TRUE;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 0.0f;
	rastDesc.SlopeScaledDepthBias = 0.0f;

	m_pDevice->CreateRasterizerState(&rastDesc, &m_pRastState);
	m_pDevcon->RSSetState(m_pRastState.Get());

	/* Create and set the viewport */
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(AppConfig::WIDTH);
	viewport.Height = static_cast<FLOAT>(AppConfig::HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDevcon->RSSetViewports(1, &viewport);

	return true;
}

bool Renderer::shaderInitialization()
{
	m_shaderFiles[ShaderType::HUD] = "hud.hlsl";
	m_shaderFiles[ShaderType::DEFAULT] = "default.hlsl";

	Shader defShader, hudShader;
	if (!defShader.initialize(m_pDevice, m_shaderFiles[ShaderType::DEFAULT]) |
		!hudShader.initialize(m_pDevice, m_shaderFiles[ShaderType::HUD])) return false;
	
	m_shaders[ShaderType::HUD] = hudShader;
	m_shaders[ShaderType::DEFAULT] = defShader;

	return true;
}

bool Renderer::layoutInitialization()
{

	/* Global default layout creation */
	D3D11_INPUT_ELEMENT_DESC vied = {};
	vied.SemanticName = "POSITION";
	vied.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vied.Format = d3dconst::POSITION_FORMAT;
	vied.AlignedByteOffset = 0;

	D3D11_INPUT_ELEMENT_DESC tied = {};
	tied.SemanticName = "TEXCOORD";
	tied.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	tied.Format = d3dconst::TEXCOORD_FORMAT;
	tied.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	D3D11_INPUT_ELEMENT_DESC nied = {};
	nied.SemanticName = "NORMAL";
	nied.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	nied.Format = d3dconst::NORMAL_FORMAT;
	nied.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	D3D11_INPUT_ELEMENT_DESC finalLayout[] = {vied, tied, nied};
	HR(m_pDevice->CreateInputLayout(
		finalLayout,
		ARRAYSIZE(finalLayout),
		m_shaders[ShaderType::DEFAULT].getByteCode(),
		m_shaders[ShaderType::DEFAULT].getByteCodeLength(),
		&m_pGlobalDefaultLayout));
	
	HR(m_pDevice->CreateInputLayout(
		finalLayout,
		ARRAYSIZE(finalLayout),
		m_shaders[ShaderType::HUD].getByteCode(),
		m_shaders[ShaderType::HUD].getByteCodeLength(),
		&m_pGlobalHUDLayout));

	m_shaderLayouts[ShaderType::DEFAULT] = m_pGlobalDefaultLayout;
	m_shaderLayouts[ShaderType::HUD] = m_pGlobalHUDLayout;

	return true;
}

bool Renderer::isVisible(const Body* b, const float3& pos)
{
	float3 bdims = b->getDimensions();
	float collSphereRad = util::maxf(bdims.z,
						  util::maxf(bdims.y, bdims.x));
	
	for (size_t i = 0;
	     i < Camera::CAM_FRUST_NSIDES;
		 ++i)
	{
		if (D3DXPlaneDotCoord(
			&m_currCamFrustum.planes[i],
			&vec3f{pos.x, pos.y, pos.z}) < -collSphereRad)
		{
			++m_nObjectsCulled;
			return false;
		}
	}

	return true;
}