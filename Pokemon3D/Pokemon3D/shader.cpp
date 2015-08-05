#include "shader.h"

#include <d3dcompiler.h>

#pragma comment (lib, "d3dcompiler.lib")

Shader::Shader(){}
Shader::~Shader(){}

bool Shader::initialize(
	comptr<ID3D11Device> const device,
	const std::string& coreFname)
{
	comptr<ID3D10Blob> errorMessage;
	std::string vFilename = SHADER_PATH + "v_" + coreFname;
	std::string pFilename = SHADER_PATH + "p_" + coreFname;

	/* Vertex shader compilation */
	HRESULT result;
	result = D3DCompileFromFile(
		util::stringToWString(vFilename).c_str(),
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		d3dconst::SHADER_FUNC,
		d3dconst::VS_PROFILE,
		D3D10_SHADER_ENABLE_STRICTNESS,
		NULL,
		&m_pVSBuffer,
		&errorMessage);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			std::wstring compileError = L"Shader compile error: " + util::stringToWString(vFilename);
			char* errorDesc = static_cast<char*>(errorMessage->GetBufferPointer());
			MessageBox(NULL, errorDesc, "Fatal Error", MB_ICONERROR);
		}
		else
		{
			std::string missingFile = "Missing shader file: " + vFilename;
			MessageBox(NULL, missingFile.c_str(), "Fatal Error", MB_ICONERROR);
		}
		return false;
	}

	/* Vertex shader creation */
	HR(device->CreateVertexShader(
		m_pVSBuffer->GetBufferPointer(),
		m_pVSBuffer->GetBufferSize(),
		NULL,
		&m_pVertexShader));

	/* Pixel shader compilation */
	result = D3DCompileFromFile(
		util::stringToWString(pFilename).c_str(),
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		d3dconst::SHADER_FUNC,
		d3dconst::PS_PROFILE,
		D3D10_SHADER_ENABLE_STRICTNESS,
		NULL,
		&m_pPSBuffer,
		&errorMessage);
	
	if (FAILED(result))
	{
		if (errorMessage)
		{
			std::wstring compileError = L"Shader compile error: " + util::stringToWString(pFilename);
			char* errorDesc = static_cast<char*>(errorMessage->GetBufferPointer());
			MessageBox(NULL, errorDesc, "Fatal Error", MB_ICONERROR);
		}
		else
		{
			std::string missingFile = "Missing shader file: " + pFilename;
			MessageBox(NULL, missingFile.c_str(), "Fatal Error", MB_ICONERROR);
		}
		return false;
	}

	/* Pixel shader creation */
	HR(device->CreatePixelShader(
		m_pPSBuffer->GetBufferPointer(),
		m_pPSBuffer->GetBufferSize(),
		NULL,
		&m_pPixelShader));


	/* Initialize constant matrix buffer */
	D3D11_BUFFER_DESC mcbd = {};
	mcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mcbd.ByteWidth = sizeof(MatrixBuffer);
	mcbd.CPUAccessFlags = 0;
	mcbd.Usage = D3D11_USAGE_DEFAULT;
	mcbd.StructureByteStride = 0;
	mcbd.MiscFlags = 0;

	HR(device->CreateBuffer(&mcbd, NULL, &m_pMatrixBuffer));
	
	/* Initialize constant color buffer */
	mcbd.ByteWidth = sizeof(ColorBuffer);
	HR(device->CreateBuffer(&mcbd, NULL, &m_pColorBuffer));
	
	return true;
}

comptr<ID3D11Buffer> Shader::getMatrixBuffer() const { return m_pMatrixBuffer; }
comptr<ID3D11Buffer> Shader::getColorBuffer() const { return m_pColorBuffer; }
comptr<ID3D11VertexShader> Shader::getVertexShader() const { return m_pVertexShader; }
comptr<ID3D11PixelShader> Shader::getPixelShader() const { return m_pPixelShader; }
void* Shader::getByteCode() const { return m_pVSBuffer->GetBufferPointer(); }
size_t Shader::getByteCodeLength() const { return m_pVSBuffer->GetBufferSize(); }

