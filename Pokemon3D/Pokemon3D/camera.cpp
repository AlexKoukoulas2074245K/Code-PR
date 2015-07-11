#include "camera.h"
#include "common.h"
#define ANGLE static_cast<float>(D3DXToRadian(90))
#define VALID_ANGLE_LO 1
#define VALID_ANGLE_HI 4
#define N_ANGLES 6
#define TURN_SPEED 0.1f

const float Camera::ZNEAR = 0.1f;
const float Camera::ZFAR = 40.0f;
const float Camera::FOV = (FLOAT) XMConvertToRadians(45);
const float Camera::ASPECT = (FLOAT) window::WIDTH / (FLOAT) window::HEIGHT;
const unsigned int Camera::CAM_FRUST_NSIDES = 6U;

Camera::Camera(): Camera(D3DXVECTOR3()){}
Camera::Camera(const D3DXVECTOR3& position) :
mPosition(position),
mUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f)),
mLook(D3DXVECTOR3(0.0f, 0.0f, 1.0f)),
mRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f)),
mTurning(false),
mCurrAngleIndex(1),
mYaw(0.0f),
mTargetYaw(mYaw),
mPitch(0.0f),
mRoll(0.0f)
{
	mOriMap[1] = Orientation::SOUTH;
	mOriMap[2] = Orientation::WEST;
	mOriMap[3] = Orientation::NORTH;
	mOriMap[4] = Orientation::EAST;
	mValidAngles = {{-ANGLE, 0.0f, ANGLE, 2*ANGLE, 3*ANGLE, 4*ANGLE}};	
	D3DXMatrixPerspectiveFovLH(&mProjMatrix, FOV, ASPECT, ZNEAR, ZFAR);
}
Camera::~Camera(){}

void Camera::Move(const cam_dir& dir, const float mag)
{
	if (mTurning) return;
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

void Camera::Look(const cam_dir& dir, const float mag)
{
	if (mTurning) return;
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

void Camera::Update()
{
	if (mTurning && util::lerp(mYaw, mTargetYaw, TURN_SPEED, mYaw))
	{
		mTurning = false;
		mYaw = mValidAngles[mCurrAngleIndex];
		mTargetYaw = mYaw;
	}
}

void Camera::Turn(const cam_dir& dir)
{
	if (mTurning) return;
	mTurning = true;

	if (dir == Direction::LEFT)
	{
		if (mCurrAngleIndex == VALID_ANGLE_LO) { mCurrAngleIndex = VALID_ANGLE_HI; mTargetYaw = mValidAngles[0]; }
		else { mCurrAngleIndex--; mTargetYaw = mValidAngles[mCurrAngleIndex]; }
	}
	else if (dir == Direction::RIGHT)
	{
		if (mCurrAngleIndex == VALID_ANGLE_HI){ mCurrAngleIndex = VALID_ANGLE_LO; mTargetYaw = mValidAngles[N_ANGLES - 1]; }
		else { mCurrAngleIndex++; mTargetYaw = mValidAngles[mCurrAngleIndex]; }
	}
}

void Camera::getCameraFrustum(
	const D3DXMATRIX& matView,
	D3DXMATRIX matProj,
	Frustum& outFrustum) const
{
	outFrustum = {};

	float r = ZFAR / (ZFAR - ZNEAR);
	matProj._33 = r;
	matProj._43 = -r * ZNEAR;

	D3DXMATRIX matrix = matView * matProj;
	// Calculate near plane of frustum.
	outFrustum.planes[0].a = matrix._14 + matrix._13;
	outFrustum.planes[0].b = matrix._24 + matrix._23;
	outFrustum.planes[0].c = matrix._34 + matrix._33;
	outFrustum.planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&outFrustum.planes[0], &outFrustum.planes[0]);

	// Calculate far plane of frustum.
	outFrustum.planes[1].a = matrix._14 - matrix._13;
	outFrustum.planes[1].b = matrix._24 - matrix._23;
	outFrustum.planes[1].c = matrix._34 - matrix._33;
	outFrustum.planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&outFrustum.planes[1], &outFrustum.planes[1]);

	// Calculate left plane of frustum.
	outFrustum.planes[2].a = matrix._14 + matrix._11;
	outFrustum.planes[2].b = matrix._24 + matrix._21;
	outFrustum.planes[2].c = matrix._34 + matrix._31;
	outFrustum.planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&outFrustum.planes[2], &outFrustum.planes[2]);

	// Calculate right plane of frustum.
	outFrustum.planes[3].a = matrix._14 - matrix._11;
	outFrustum.planes[3].b = matrix._24 - matrix._21;
	outFrustum.planes[3].c = matrix._34 - matrix._31;
	outFrustum.planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&outFrustum.planes[3], &outFrustum.planes[3]);

	// Calculate top plane of frustum.
	outFrustum.planes[4].a = matrix._14 - matrix._12;
	outFrustum.planes[4].b = matrix._24 - matrix._22;
	outFrustum.planes[4].c = matrix._34 - matrix._32;
	outFrustum.planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&outFrustum.planes[4], &outFrustum.planes[4]);

	// Calculate bottom plane of frustum.
	outFrustum.planes[5].a = matrix._14 + matrix._12;
	outFrustum.planes[5].b = matrix._24 + matrix._22;
	outFrustum.planes[5].c = matrix._34 + matrix._32;
	outFrustum.planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&outFrustum.planes[5], &outFrustum.planes[5]);	
}

const D3DXMATRIX& Camera::getViewMatrix()
{
	mUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mLook = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	mRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationAxis(&yawMatrix, &mUp, mYaw);
	D3DXVec3TransformCoord(&mLook, &mLook, &yawMatrix);
	D3DXVec3TransformCoord(&mRight, &mRight, &yawMatrix);

	D3DXMATRIX pitchMatrix;
	D3DXMatrixRotationAxis(&pitchMatrix, &mRight, mPitch);
	//D3DXVec3TransformCoord(&mLook, &mLook, &pitchMatrix);
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

D3DXVECTOR4 Camera::getPosition() const
{
	return D3DXVECTOR4(mPosition.x, mPosition.y, mPosition.z, 1.0f); 
}

Camera::cam_ori Camera::getOrientation() const
{
	return mOriMap.at(mCurrAngleIndex); 
}
