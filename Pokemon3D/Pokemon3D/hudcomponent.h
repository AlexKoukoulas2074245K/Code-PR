#pragma once
#include <string>

#include "pokedef.h"
#include "body.h"

class HUDComponent
{
public:
	HUDComponent();
	~HUDComponent();

	void changeTexCoords(const float2 coords[4]);

	Body* getModBodyPointer();
	const Body& getBody() const;
	const float2& getPosition() const;
	const float2& getDimensions() const;

	void setPosition(const float x, const float y);
	void setDimensions(const float width, const float height);
	void setImage(const std::string& imageName);

private:
	Body m_body;
	float2 m_position;
	float2 m_dimensions;
};