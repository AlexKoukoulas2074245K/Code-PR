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

	void computePosition();
	void update(const sptr<HIDManager> HIDManager);
	uptr<Camera>& modCameraPointer();

private:

	uptr<Camera> m_pCamera;
	const sptr<Level>& m_pLevel;
	uint2 m_levelCoords;
	vec3f m_levelPos;
};