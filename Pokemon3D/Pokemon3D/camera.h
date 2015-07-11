#pragma once
#include "d3dcommon.h"
#include <array>
#include <map>

class Camera
{
public:

	static const float ZNEAR;
	static const float ZFAR;
	static const float FOV;
	static const float ASPECT;
	static const unsigned int CAM_FRUST_NSIDES;

	typedef enum Direction
	{
		FORWARD, BACKWARD,
		RIGHT, LEFT,
		UP, DOWN
	} cam_dir;

	typedef enum Orientation
	{
		NORTH, SOUTH, EAST, WEST
	} cam_ori;

	typedef struct Frustum
	{
		D3DXPLANE planes[6];
	};

	Camera(const D3DXVECTOR3& cameraPos);
	Camera();
	~Camera();
	
	void Move(const cam_dir& dir, const float mag);
	void Look(const cam_dir& dir, const float mag);
	void Turn(const cam_dir& dir);
	void Update();

	void getCameraFrustum(const D3DXMATRIX& matView,
						  D3DXMATRIX matProj,
						  Frustum& outFrustum) const;

	const D3DXMATRIX& getViewMatrix();
	const D3DXMATRIX& getProjMatrix() const { return mProjMatrix; }
	D3DXVECTOR4 getPosition() const; 
	cam_ori getOrientation() const;

	void setPosition(const D3DXVECTOR3& pos) { mPosition = pos; }

private:
	std::map<size_t, Orientation> mOriMap;
	D3DXMATRIX mViewMatrix;
	D3DXMATRIX mProjMatrix;
	D3DXVECTOR3 mPosition; 
	D3DXVECTOR3 mUp, mLook, mRight;
	float mYaw, mPitch, mRoll;
	std::array<float, 6> mValidAngles;
	float mTargetYaw;
	bool mTurning;
	size_t mCurrAngleIndex;

};