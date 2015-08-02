#pragma once

#include <string>

#include "pokedef.h"
#include "body.h"

class IOManager;
class HUDComponent
{
public:
	HUDComponent();
	~HUDComponent();

	Body* getModBodyPointer() { return &mBody; }
	const Body& getBody() const { return mBody; }
	const float2& getPosition() const { return mPosition; }
	const float2& getDimensions() const { return mDimensions; }
	void setPosition(const float x, const float y);
	void setDimensions(const float width, const float height);
	void setImage(const std::string& imageName, const sptr<IOManager> iom);
	void ChangeTexCoords(const float2 coords[4]);

private:
	Body mBody;
	float2 mPosition;
	float2 mDimensions;
};