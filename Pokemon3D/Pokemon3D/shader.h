#pragma once
#include "d3dcommon.h"
#include "pokedef.h"
#include "common.h"

#define SHADER_PATH std::string("C:/Users/alex/Pictures/projects/pkmnrevo/shaders/")

class Shader
{
public:
	
	struct MatrixBuffer
	{
		mat4x4 finalMatrix;
		mat4x4 worldMatrix;
		mat4x4 rotMatrix;
		vec4f camPosition;
	};

	struct ColorBuffer
	{
		float4 color;
	};

	Shader();
	~Shader();

	bool Initialize(
		comptr<ID3D11Device> const device,
		const std::string& coreFname);

	comptr<ID3D11Buffer> getMatrixBuffer() const { return mcMatrixBuffer; }
	comptr<ID3D11Buffer> getColorBuffer() const { return mcColorBuffer; }
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
	comptr<ID3D11Buffer> mcColorBuffer;
};