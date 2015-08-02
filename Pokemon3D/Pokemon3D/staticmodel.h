#pragma once
#include "pokedef.h"
#include "body.h"

class StaticModel
{
public:
	StaticModel(const Body& body, const float3& pos, const float3& rot):
	mBody(body),
	mPos(pos),
	mRot(rot){};
	~StaticModel(){};

	void setPos(const float3& pos) { mPos = pos; }
	void setX(const float x) { mPos.x = x; }
	void setY(const float y) { mPos.y = y; }
	void setZ(const float z) { mPos.z = z; }

	void setRot(const float3& rot) { mRot = rot; }
	void setRotX(const float rotX) { mRot.x = rotX; }
	void setRoty(const float rotY) { mRot.y = rotY; }
	void setRotZ(const float rotZ) { mRot.z = rotZ; }
	
	Body* getModBodyPointer() { return &mBody; }
	const Body& getBody() const { return mBody; }
	const float3& getPos() const { return mPos; }
	const float3& getRot() const { return mRot; }

private:
	Body mBody;
	float3 mPos;
	float3 mRot;
};