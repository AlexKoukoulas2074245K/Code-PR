#include "camera.h"

Camera::Camera(){}
Camera::~Camera(){}

void Camera::Initialize(const D3DXVECTOR3& position)
{
	mPosition = position;
	mYaw = 3.14f;
	mPitch = 0.0f;
	mRoll = 0.0f;
	mUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	mRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	/* temp */
	moving = false;
	mTargetYaw = mYaw;
}

void Camera::Move(const Direction& dir, const float mag)
{
	if (moving) return;
	switch (dir)
	{
		case FORWARD:
		{
			mPosition += mLook * mag;
		}break;

		case BACKWARD:
		{
			mPosition += mLook * -mag;
		}break;

		case RIGHT:
		{
			mPosition += mRight * mag;
		}break;

		case LEFT:
		{
			mPosition += mRight * -mag;
		}break;

		case UP:
		{
			mPosition += mUp * mag;
		}break;

		case DOWN:
		{
			mPosition += mUp * -mag;
		}break;
	}
}

void Camera::Look(const Direction& dir, const float mag)
{
	if (moving) return;
	switch (dir)
	{
		case RIGHT:
		{
			mYaw += mag;
		}break;

		case LEFT:
		{
			mYaw -= mag;
		}break;

		case UP:
		{
			mPitch -= mag;
		}break;

		case DOWN:
		{
			mPitch += mag;
		}break;
	}
}

void Camera::Turn(const Direction& dir)
{
	if (moving) return;
	moving = true;

	if (dir == Direction::LEFT) mTargetYaw = mYaw - D3DXToRadian(90.0f);
	else if(dir == Direction::RIGHT) mTargetYaw = mYaw + D3DXToRadian(90.0f);
}

const D3DXMATRIX& Camera::getViewMatrix()
{
	if (moving)
	{
		float fdiff = mTargetYaw - mYaw;
		const float goal = 0.1f;
		if (fdiff > goal) mYaw += goal;
		else if (fdiff < -goal) mYaw -= goal;
		else 
		{
			moving = false; mTargetYaw = mYaw; 
		}
	}

	mUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	mRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationAxis(&yawMatrix, &mUp, mYaw);
	D3DXVec3TransformCoord(&mLook, &mLook, &yawMatrix);
	D3DXVec3TransformCoord(&mRight, &mRight, &yawMatrix);

	D3DXMATRIX pitchMatrix;
	D3DXMatrixRotationAxis(&pitchMatrix, &mRight, mPitch);
	D3DXVec3TransformCoord(&mLook, &mLook, &pitchMatrix);
	D3DXVec3TransformCoord(&mUp, &mUp, &pitchMatrix);

	D3DXMATRIX rollMatrix;
	D3DXMatrixRotationAxis(&rollMatrix, &mLook, mRoll);
	D3DXVec3TransformCoord(&mRight, &mRight, &rollMatrix);
	D3DXVec3TransformCoord(&mUp, &mUp, &rollMatrix);

	D3DXMatrixIdentity(&mViewMatrix);

	mViewMatrix._11 = mRight.x;  mViewMatrix._12 = mUp.x;  mViewMatrix._13 = mLook.x;
	mViewMatrix._21 = mRight.y;  mViewMatrix._22 = mUp.y;  mViewMatrix._23 = mLook.y;
	mViewMatrix._31 = mRight.z;  mViewMatrix._32 = mUp.z;  mViewMatrix._33 = mLook.z;

	mViewMatrix._41 = -D3DXVec3Dot(&mPosition, &mRight);
	mViewMatrix._42 = -D3DXVec3Dot(&mPosition, &mUp);
	mViewMatrix._43 = -D3DXVec3Dot(&mPosition, &mLook);

	return mViewMatrix;
}