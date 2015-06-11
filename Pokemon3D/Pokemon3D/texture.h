#pragma once
#include "d3dcommon.h"

class Texture
{
public:
	typedef comptr<ID3D11ShaderResourceView> texture_t;
	Texture(){}
	~Texture(){}
	texture_t& modTexture() { return mTexture; }
	texture_t immTexture() const { return mTexture; }

private:
	texture_t mTexture;
};