#pragma once
#include "d3dcommon.h"
#include "pokedef.h"
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

	Camera(const vec3f& cameraPos);
	Camera();
	~Camera();
	
	void MoveTo(const float targetPos);
	void Turn(const cam_dir& dir);
	void Update();

	void getCameraFrustum(const mat4x4& matView,
						  mat4x4 matProj,
						  Frustum& outFrustum) const;

	const mat4x4& getViewMatrix();
	const mat4x4& getProjMatrix() const { return mProjMatrix; }
	vec4f getPosition() const { return vec4f{mPosition.x, mPosition.y, mPosition.z, 1.0f}; }
	vec3f getPosition3() const { return mPosition; }
	cam_ori getOrientation() const { return mOriMap.at(mCurrAngleIndex); }
	bool getMoving() const { return mMoving; }
	bool getTurning() const { return mTurning; }

	void setPosition(const vec3f& pos) { mPosition = pos; }
	void setPosition(const float x, const float y, const float z) { mPosition = vec3f{x, y, z}; }

private:
	std::map<size_t, Orientation> mOriMap;
	mat4x4 mViewMatrix, mProjMatrix;
	vec3f mPosition, mUp, mLook, mRight;
	float mYaw, mPitch, mRoll, mTargetYaw, mTargetPos;
	bool mTurning, mMoving;
	size_t mCurrAngleIndex;
	std::array<float, 6> mValidAngles;

};