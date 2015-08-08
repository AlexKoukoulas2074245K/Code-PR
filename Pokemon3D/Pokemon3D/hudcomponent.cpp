#include "hudcomponent.h"

#include "iomanager.h"

HUDComponent::HUDComponent():
m_dimensions(float2{Body::BITMAP_INIT_WIDTH, Body::BITMAP_INIT_HEIGHT})
{
	m_body.setVertices(Body::BITMAP_VERTICES);
	m_body.setIndices(Body::BITMAP_INDICES);
	m_body.setInitDims(float3{Body::BITMAP_INIT_WIDTH, Body::BITMAP_INIT_HEIGHT, 1.0f});
	m_body.setDimensions(m_body.getInitDims());
}
HUDComponent::~HUDComponent(){}

void HUDComponent::changeTexCoords(const float2 coords[4])
{
	for (size_t i = 0; i < 4; ++i)
	{
		Body::vertex_t v = m_body.getVertex(i);
		v.u = coords[i].x;
		v.v = coords[i].y;
		m_body.setVertex(i, v);
	}
}

Body* HUDComponent::getModBodyPointer() { return &m_body; }
const Body& HUDComponent::getBody() const { return m_body; }
const float2& HUDComponent::getPosition() const { return m_position; }
const float2& HUDComponent::getDimensions() const { return m_dimensions; }

void HUDComponent::setPosition(const float x, const float y){ m_position = float2{x, y}; }
void HUDComponent::setDimensions(const float width, const float height)
{
	m_dimensions = float2{width, height};
	m_body.setDimensions(float3{width, height, 1.0f});
}

void HUDComponent::setImage(const std::string& imageName)
{
	std::string s;
	IOManager::get().getPathOf(imageName, IOManager::Format::HUD, s);
	m_body.setSingleTexture(s);
}
