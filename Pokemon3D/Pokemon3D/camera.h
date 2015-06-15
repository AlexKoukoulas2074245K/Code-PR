#pragma once
#include "d3dcommon.h"

class Camera
{
public:
	enum Direction
	{
		FORWARD, BACKWARD,
		RIGHT, LEFT,
		UP, DOWN
	};

	Camera();
	~Camera();
	
	void Initialize(const D3DXVECTOR3& cameraPos);
	void Move(const Direction& dir, const float mag);
	void Look(const Direction& dir, const float mag);
	/* temp */
	void Turn(const Direction& dir);

	const D3DXMATRIX& getViewMatrix();
	D3DXVECTOR4 getPosition() const { return D3DXVECTOR4(mPosition.x, mPosition.y, mPosition.z, 1.0f); }

private:
	D3DXMATRIX mViewMatrix;
	D3DXVECTOR3 mPosition; 
	D3DXVECTOR3 mUp, mLook, mRight;
	float mYaw, mPitch, mRoll;
	
	/* temp*/
	float mTargetYaw;
	bool moving;
};