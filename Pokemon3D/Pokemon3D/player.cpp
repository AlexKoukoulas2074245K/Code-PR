#include "player.h"

Player::Player(const D3DXVECTOR3& pos):
mCamera(pos)
{}

void Player::Update(const sptr<HIDManager> mHIDManager)
{
	if (mHIDManager->KeyTapped(KEY_LEFT)) mCamera.Turn(Camera::LEFT);
	if (mHIDManager->KeyTapped(KEY_RIGHT)) mCamera.Turn(Camera::RIGHT);
	if (mHIDManager->KeyDown(KEY_UP)) mCamera.Move(Camera::FORWARD, 0.1f);
	if (mHIDManager->KeyDown(KEY_DOWN)) mCamera.Move(Camera::BACKWARD, 0.1f);
	if (mHIDManager->KeyDown(KEY_A)) mCamera.Look(Camera::UP, 0.01f);
	if (mHIDManager->KeyDown(KEY_B)) mCamera.Look(Camera::DOWN, 0.01f);
	mCamera.Update();
}