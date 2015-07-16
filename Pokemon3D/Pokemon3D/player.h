#pragma once
#include "camera.h"
#include "hidmanager.h"
#include "common.h"

class Level;
class Player
{
public:
	Player(const vec3f& pos, const sptr<Level>& level);
	~Player();

	void FindPosition();
	void Update(const sptr<HIDManager> mHIDManager);
	Camera& getCamera() { return mCamera; }

private:

	Camera mCamera;
	const sptr<Level>& mLevel;
	uint2 mLevelCoords;
	vec3f mLevelPos;
};