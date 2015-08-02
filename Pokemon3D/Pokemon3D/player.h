#pragma once
#include "common.h"
#include "pokedef.h"

class Level;
class Camera;
class HIDManager;
class Player
{
public:
	Player(const vec3f& pos, const sptr<Level>& level);
	~Player();

	void FindPosition();
	void Update(const sptr<HIDManager> HIDManager);
	uptr<Camera>& getCamera() { return mCamera; }

private:

	uptr<Camera> mCamera;
	const sptr<Level>& mLevel;
	uint2 mLevelCoords;
	vec3f mLevelPos;
};