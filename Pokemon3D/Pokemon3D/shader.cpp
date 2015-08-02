#include "shader.h"

#include <d3dcompiler.h>

#pragma comment (lib, "d3dcompiler.lib")

Shader::Shader(){}
Shader::~Shader(){}

bool Shader::Initialize(
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
		&mVSBuffer,
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
		mVSBuffer->GetBufferPointer(),
		mVSBuffer->GetBufferSize(),
		NULL,
		&mVertexShader));

	/* Pixel shader compilation */
	result = D3DCompileFromFile(
		util::stringToWString(pFilename).c_str(),
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		d3dconst::SHADER_FUNC,
		d3dconst::PS_PROFILE,
		D3D10_SHADER_ENABLE_STRICTNESS,
		NULL,
		&mPSBuffer,
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
		mPSBuffer->GetBufferPointer(),
		mPSBuffer->GetBufferSize(),
		NULL,
		&mPixelShader));


	/* Initialize constant matrix buffer */
	D3D11_BUFFER_DESC mcbd = {};
	mcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mcbd.ByteWidth = sizeof(MatrixBuffer);
	mcbd.CPUAccessFlags = 0;
	mcbd.Usage = D3D11_USAGE_DEFAULT;
	mcbd.StructureByteStride = 0;
	mcbd.MiscFlags = 0;

	HR(device->CreateBuffer(&mcbd, NULL, &mcMatrixBuffer));

	return true;
}

