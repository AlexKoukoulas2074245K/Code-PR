#pragma once
#include <array>
#include <map>

#include "d3dcommon.h"
#include "pokedef.h"

class Camera
{
public:
	static const float ZNEAR;
	static const float ZFAR;
	static const float FOV;
	static const unsigned int CAM_FRUST_NSIDES;

public:
	enum Direction
	{
		FORWARD, BACKWARD,
		RIGHT, LEFT,
		UP, DOWN
	};

	enum Orientation
	{
		NORTH, SOUTH, EAST, WEST
	};

	struct Frustum
	{
		D3DXPLANE planes[6];
	};

	Camera(const vec3f& cameraPos);
	Camera();
	~Camera();
	
	void moveTo(const float targetPos);
	void turn(const Direction& dir);
	void update();

	const mat4x4& computeViewMatrix();
	void getCameraFrustum(const mat4x4& matView,
						  mat4x4 matProj,
						  Frustum& outFrustum) const;
	const mat4x4& getProjMatrix() const;
	vec4f getPosition() const;
	vec3f getPosition3() const;
	Orientation getOrientation() const;
	bool isMoving() const;
	bool isTurning() const;

	void setPosition(const vec3f& pos);
	void setPosition(const float x, const float y, const float z);

private:
	static const float ANGLE;
	static const float TURN_SPEED;
	static const float MOVE_SPEED;
	static const size_t VALID_ANGLE_LO;
	static const size_t VALID_ANGLE_HI;
	static const size_t N_ANGLES;

private:
	std::map<size_t, Orientation> m_oriMap;
	mat4x4 m_viewMatrix, m_projMatrix;
	vec3f m_position, m_up, m_look, m_right;
	float m_yaw, m_pitch, m_roll, m_targetYaw, m_targetPos;
	bool m_isTurning, m_isMoving;
	size_t m_currAngleIndex;
	std::array<float, 6> m_validAngles;

};