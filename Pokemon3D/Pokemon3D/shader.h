#pragma once
#include "d3dcommon.h"
#include "common.h"

#define SHADER_PATH std::string("C:/Users/alex/Pictures/projects/pkmnrevo/shaders/")

class Shader
{
public:
	
	struct MatrixBuffer
	{
		D3DXMATRIX finalMatrix;
		D3DXMATRIX worldMatrix;
		D3DXMATRIX rotMatrix;
		D3DXVECTOR4 camPosition;
	};

	Shader();
	~Shader();

	bool Initialize(
		comptr<ID3D11Device> const device,
		const std::string& coreFname);

	comptr<ID3D11Buffer> getMatrixBuffer() const { return mcMatrixBuffer; }
	comptr<ID3D11VertexShader> getVertexShader() const { return mVertexShader; }
	comptr<ID3D11PixelShader> getPixelShader() const { return mPixelShader; }
	void* getByteCode() const { return mVSBuffer->GetBufferPointer(); }
	size_t getByteCodeLength() const { return mVSBuffer->GetBufferSize(); }

private:
	comptr<ID3D10Blob> mVSBuffer;
	comptr<ID3D10Blob> mPSBuffer;
	comptr<ID3D11VertexShader> mVertexShader;
	comptr<ID3D11PixelShader> mPixelShader;
	comptr<ID3D11Buffer> mcMatrixBuffer;
};