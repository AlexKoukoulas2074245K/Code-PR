#include "renderer.h"

#include "hudcomponent.h"
#include "staticmodel.h"
#include "body.h"
#include "ddsloader.h"
#include "fontengine.h"

#define WIC_TEX

Renderer::Renderer(){}
Renderer::~Renderer(){}

bool Renderer::Initialize(const HWND hWindow)
{
	mActiveShaderType = ShaderType::NONE;
	uint rrNum, rrDen;
	if (!PreInitialization(hWindow, rrNum, rrDen)) return false;
	if (!CoreInitialization(hWindow, rrNum, rrDen)) return false;
	if (!ShaderInitialization()) return false;
	if (!LayoutInitialization()) return false;

	return true;
}

bool Renderer::LoadFontImage(const std::string& imagePath, Texture* fontImage)
{
	Texture texture;
	HR(DirectX::CreateWICTextureFromFile(
		mDevice.Get(),
		mDevcon.Get(),
		util::stringToWString(imagePath.c_str()).c_str(),
		NULL,
		&texture.modTexture(),
		0));
	*fontImage = texture;
	return true;
}

void Renderer::PrepareFrame(
	const mat4x4& currView,
	const mat4x4& currProj,
	const vec4f& currCamPos,
	const Camera::Frustum& currCamFrustum)
{
	mDevcon->ClearRenderTargetView(mBackBuffer.Get(), d3dconst::BG_COLOR);
	mDevcon->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	mCurrView = currView;
	mCurrProj = currProj;
	mCurrCamPosition = currCamPos;
	mCurrCamFrustum = currCamFrustum;
}

void Renderer::CompleteFrame()
{
	if (WindowConfig::VSYNC)
		mSwapchain->Present(1, 0);
	else
		mSwapchain->Present(0, 0);
}

void Renderer::ChangeActiveLayout(const ShaderType shaderType)
{
	mActiveShaderType = shaderType;
	mDevcon->VSSetShader(P_ACTIVE_VERTEX_SHADER, NULL, NULL);
	mDevcon->PSSetShader(P_ACTIVE_PIXEL_SHADER, NULL, NULL);
	mDevcon->IASetInputLayout(mShaderLayouts[mActiveShaderType].Get());
}

void Renderer::setDepthRendering(const bool depthRendering)
{
	if (mDepthRendering == depthRendering) return;
	mDepthRendering = depthRendering;
	if (mDepthRendering) mDevcon->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
	else mDevcon->OMSetDepthStencilState(mDisabledDepthState.Get(), 1);
}

void Renderer::RenderHUD(HUDComponent* hudc)
{
	setDepthRendering(false);
	float2 hudcPos = hudc->getPosition();
	float3 finalPos = float3{hudcPos.x, hudcPos.y, 0.0f};
	float3 finalRot = {};
	RenderObject(ShaderType::HUD, finalPos, finalRot, true, hudc->getModBodyPointer());
}

void Renderer::RenderText(const std::list<std::string>& chars, const float2& startPos, FontEngine* font)
{
	float2 posCounter = {};
	posCounter.x = startPos.x;
	posCounter.y = startPos.y;
	for (std::list<std::string>::const_iterator citer = chars.begin();
		 citer != chars.end();
		 ++citer)
	{
		HUDComponent* glyphComp = font->modGlyphCompPointer(*citer);
		glyphComp->setPosition(posCounter.x, posCounter.y);
		RenderHUD(glyphComp);
		posCounter.x += 0.05f;
	}
}

void Renderer::RenderModel(StaticModel* model)
{
	setDepthRendering(true);
	RenderObject(ShaderType::DEFAULT, model->getPos(), model->getRot(), false, model->getModBodyPointer());
}

void Renderer::RenderObject(
	const ShaderType shader,
	const float3& pos,
	const float3& rot,
	const bool hud,
	Body* body)
{
	if (!body->isReady() || (!hud && !isVisible(body, pos))) return;
	if (mActiveShaderType != shader) ChangeActiveLayout(shader);

	UINT stride = sizeof(Body::Vertex);
	UINT offset = 0;
	mDevcon->IASetVertexBuffers(0, 1, body->modVertexBuffer().GetAddressOf(), &stride, &offset);
	mDevcon->IASetIndexBuffer(body->immIndexBuffer().Get(), d3dconst::INDEX_FORMAT, 0U);
	
	
	mat4x4 matTrans, matRotX, matRotY, matRotZ, matScale;
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);
	D3DXMatrixRotationX(&matRotX, rot.x);
	D3DXMatrixRotationY(&matRotY, rot.y);
	D3DXMatrixRotationZ(&matRotZ, rot.z);

	float3 bodyInDims = body->getInitDims();
	float3 bodyAcDims = body->getDimensions();

	if (hud) D3DXMatrixScaling(
				&matScale,
				(bodyAcDims.x / bodyInDims.x) / WindowConfig::ASPECT,
				bodyAcDims.y / bodyInDims.y,
				1.0f);
	else 	D3DXMatrixScaling(
				&matScale,
				bodyAcDims.x / bodyInDims.x,
				1.0f,
				bodyAcDims.z / bodyInDims.z);

	D3DXMATRIX matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
	D3DXMATRIX matFinal = hud ? matWorld : matWorld * mCurrView * mCurrProj;

	Shader::MatrixBuffer mb = {};
	mb.camPosition = mCurrCamPosition;
	mb.finalMatrix = matFinal;
	mb.rotMatrix = matRotX * matRotY * matRotZ;
	mb.worldMatrix = matWorld;

	mDevcon->PSSetShaderResources(0, 1, body->getActiveTexture().immTexture().GetAddressOf());
	mDevcon->VSSetConstantBuffers(0, 1, IACTIVE_SHADER.getMatrixBuffer().GetAddressOf());
	mDevcon->UpdateSubresource(IACTIVE_SHADER.getMatrixBuffer().Get(), NULL, NULL, &mb, NULL, NULL);
	mDevcon->IASetPrimitiveTopology(d3dconst::PRIMITIVES);
	mDevcon->DrawIndexed(body->getIndexCount(), 0, 0);
}

bool Renderer::PrepareModel(StaticModel* model)
{
	return PrepareObject(ShaderType::DEFAULT, model->getModBodyPointer());
}

bool Renderer::PrepareHUD(HUDComponent* hudc)
{
	return PrepareObject(ShaderType::HUD, hudc->getModBodyPointer());
}

bool Renderer::PrepareObject(const ShaderType shader, Body* body)
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
	
	HR(mDevice->CreateBuffer(&vbd, &vsrd, &bodyVB));

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

	HR(mDevice->CreateBuffer(&ibd, &isrd, &bodyIB));

	/* Load textures: remove from toload and assign to texlist */
	Body::textoload_list& toload = body->modTexturesToLoad();
	Body::texture_list& texlist = body->modTextures();

	while (!toload.empty())
	{
		Texture texture;

#ifdef WIC_TEX
		HR(DirectX::CreateWICTextureFromFile(
			mDevice.Get(),
			mDevcon.Get(),
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
		if (dispModeList[i].Width == (uint) WindowConfig::WIDTH &&
			dispModeList[i].Height == (uint) WindowConfig::HEIGHT)
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
	scd.BufferDesc.Width = WindowConfig::WIDTH;
	scd.BufferDesc.Height = WindowConfig::HEIGHT;
	scd.BufferDesc.Format = d3dconst::BACK_BUFFER_FORMAT;
	scd.BufferDesc.RefreshRate.Numerator = WindowConfig::VSYNC ? rrNum : 0;
	scd.BufferDesc.RefreshRate.Denominator = WindowConfig::VSYNC ? rrDen : 1;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = d3dconst::MS_COUNT;
	scd.SampleDesc.Quality = d3dconst::MS_QUAL;
	scd.BufferCount = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWindow;
	scd.Windowed = !WindowConfig::FULL_SCREEN;
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
	dbd.Width = WindowConfig::WIDTH;
	dbd.Height = WindowConfig::HEIGHT;
	dbd.MipLevels = 1;
	dbd.ArraySize = 1;
	dbd.Format = d3dconst::DEPTH_BUFFER_FORMAT;
	dbd.SampleDesc.Count = d3dconst::MS_COUNT;
	dbd.SampleDesc.Quality = d3dconst::MS_QUAL;
	dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
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
	depthStencilDesc.DepthEnable = false;
	HR(mDevice->CreateDepthStencilState(&depthStencilDesc, &mDisabledDepthState));
	
	/* Create the depth stencil view */
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = d3dconst::DEPTH_BUFFER_FORMAT;
	dsvd.ViewDimension = d3dconst::DEPTH_BUFFER_DIMENSION;
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

	D3D11_SAMPLER_DESC sd;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 8;
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

	mDevice->CreateSamplerState(&sd, &mSampleState);
	mDevcon->PSSetSamplers(0, 1, mSampleState.GetAddressOf());

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

	mDevice->CreateRasterizerState(&rastDesc, &mRastState);
	mDevcon->RSSetState(mRastState.Get());

	/* Create and set the viewport */
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT>(WindowConfig::WIDTH);
	viewport.Height = static_cast<FLOAT>(WindowConfig::HEIGHT);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDevcon->RSSetViewports(1, &viewport);

	return true;
}

bool Renderer::ShaderInitialization()
{
	mShaderFiles[ShaderType::HUD] = "hud.hlsl";
	mShaderFiles[ShaderType::DEFAULT] = "default.hlsl";

	Shader defShader, hudShader;
	if (!defShader.Initialize(mDevice, mShaderFiles[ShaderType::DEFAULT]) |
		!hudShader.Initialize(mDevice, mShaderFiles[ShaderType::HUD])) return false;
	
	mShaders[ShaderType::HUD] = hudShader;
	mShaders[ShaderType::DEFAULT] = defShader;

	return true;
}

bool Renderer::LayoutInitialization()
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
	HR(mDevice->CreateInputLayout(
		finalLayout,
		ARRAYSIZE(finalLayout),
		mShaders[ShaderType::DEFAULT].getByteCode(),
		mShaders[ShaderType::DEFAULT].getByteCodeLength(),
		&mGlobalDefaultLayout));
	
	//TODO actually differentiate the two layouts
	HR(mDevice->CreateInputLayout(
		finalLayout,
		ARRAYSIZE(finalLayout),
		mShaders[ShaderType::HUD].getByteCode(),
		mShaders[ShaderType::HUD].getByteCodeLength(),
		&mGlobalHUDLayout));

	mShaderLayouts[ShaderType::DEFAULT] = mGlobalDefaultLayout;
	mShaderLayouts[ShaderType::HUD] = mGlobalHUDLayout;

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
			&mCurrCamFrustum.planes[i],
			&vec3f{pos.x, pos.y, pos.z}) < -collSphereRad) return false;
	}

	return true;
}