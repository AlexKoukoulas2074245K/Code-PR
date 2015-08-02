#include "hudcomponent.h"

#include "iomanager.h"

HUDComponent::HUDComponent():
mDimensions(float2{Body::BITMAP_INIT_WIDTH, Body::BITMAP_INIT_HEIGHT})
{
	mBody.setVertices(Body::BITMAP_VERTICES);
	mBody.setIndices(Body::BITMAP_INDICES);
	mBody.setInitDims(float3{
		Body::BITMAP_INIT_WIDTH,
		Body::BITMAP_INIT_HEIGHT,
		1.0f});
	mBody.setDimensions(mBody.getInitDims());
}
HUDComponent::~HUDComponent(){}

void HUDComponent::setPosition(const float x, const float y)
{
	mPosition = float2{x, y};
}

void HUDComponent::setDimensions(const float width, const float height)
{
	mDimensions = float2{width, height};
	mBody.setDimensions(float3{width, height, 1.0f});
}

void HUDComponent::setImage(const std::string& imageName, const sptr<IOManager> iom)
{
	std::string s;
	iom->GetPathOf(imageName, IOManager::Format::HUD, s);
	mBody.setSingleTexture(s);
}

void HUDComponent::ChangeTexCoords(const float2 coords[4])
{
	for (size_t i = 0; i < 4; ++i)
	{
		Body::vertex_t v = mBody.getVertex(i);
		v.u = coords[i].x;
		v.v = coords[i].y;
		mBody.setVertex(i, v);
	}
}