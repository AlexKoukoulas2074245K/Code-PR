#pragma once
#include "camera.h"
#include "hidmanager.h"
#include "common.h"

class Player
{
public:
	Player(const D3DXVECTOR3& pos);
	~Player(){};

	void Update(const sptr<HIDManager> mHIDManager);
	Camera& getCamera() { return mCamera; }

private:
	Camera mCamera;
};