#include "player.h"

#include "level.h"

Player::Player(const vec3f& pos, const sptr<Level>& level):
mCamera(pos),
mLevel(level)
{}

Player::~Player(){}

void Player::FindPosition()
{
	mLevelCoords = mLevel->GetCoords(mCamera.getPosition3());
	mLevelPos = mCamera.getPosition3();
}

void Player::Update(const sptr<HIDManager> mHIDManager)
{
	if (mHIDManager->KeyTapped(KEY_LEFT)) mCamera.Turn(Camera::LEFT);
	else if (mHIDManager->KeyTapped(KEY_RIGHT)) mCamera.Turn(Camera::RIGHT);
	else if (mHIDManager->KeyDown(KEY_UP))
	{
		if (
			mCamera.getOrientation() == Camera::NORTH &&
			mLevel->ValidCoords(mLevelCoords.x, mLevelCoords.y - 1))
				mCamera.MoveTo(mLevel->GetPosition(uint2{mLevelCoords.x, mLevelCoords.y - 1}, 0.0f).z);
	
		else if (
			mCamera.getOrientation() == Camera::SOUTH &&
			mLevel->ValidCoords(mLevelCoords.x, mLevelCoords.y + 1))
				mCamera.MoveTo(mLevel->GetPosition(uint2{mLevelCoords.x, mLevelCoords.y + 1}, 0.0f).z);
		
		else if (
			mCamera.getOrientation() == Camera::WEST &&
		    mLevel->ValidCoords(mLevelCoords.x - 1, mLevelCoords.y))
				mCamera.MoveTo(-mLevel->GetPosition(uint2{mLevelCoords.x - 1, mLevelCoords.y}, 0.0f).x);

		else if (
			mCamera.getOrientation() == Camera::EAST &&
			mLevel->ValidCoords(mLevelCoords.x + 1, mLevelCoords.y))
				mCamera.MoveTo(-mLevel->GetPosition(uint2{mLevelCoords.x + 1, mLevelCoords.y}, 0.0f).x);	
	}
	mCamera.Update();
	FindPosition();
}