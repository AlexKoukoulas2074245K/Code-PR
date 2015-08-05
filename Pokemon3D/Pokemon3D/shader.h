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
		int enableColor;
		float3 padding;
		float4 color;
	};

	Shader();
	~Shader();

	bool initialize(
		comptr<ID3D11Device> const device,
		const std::string& coreFname);

	comptr<ID3D11Buffer> getMatrixBuffer() const;
	comptr<ID3D11Buffer> getColorBuffer() const;
	comptr<ID3D11VertexShader> getVertexShader() const;
	comptr<ID3D11PixelShader> getPixelShader() const;
	void* getByteCode() const;
	size_t getByteCodeLength() const;

private:
	comptr<ID3D10Blob> m_pVSBuffer;
	comptr<ID3D10Blob> m_pPSBuffer;
	comptr<ID3D11VertexShader> m_pVertexShader;
	comptr<ID3D11PixelShader> m_pPixelShader;
	comptr<ID3D11Buffer> m_pMatrixBuffer;
	comptr<ID3D11Buffer> m_pColorBuffer;
};