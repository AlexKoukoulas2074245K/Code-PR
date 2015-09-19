#include "camera.h"
#include "common.h"


const float Camera::ZNEAR = 0.1f;
const float Camera::ZFAR = 30.0f;
const float Camera::FOV = (FLOAT) XMConvertToRadians(45);
const unsigned int Camera::CAM_FRUST_NSIDES = 6U;

const float Camera::ANGLE = static_cast<float>(D3DXToRadian(90));
const size_t Camera::VALID_ANGLE_LO = 1;
const size_t Camera::VALID_ANGLE_HI = 4;
const size_t Camera::N_ANGLES = 6;
const float Camera::TURN_SPEED = 0.1f;
const float Camera::MOVE_SPEED = 0.1f;

Camera::Camera(): Camera(vec3f()){}
Camera::Camera(const vec3f& position) :
m_position(position),
m_up(vec3f(0.0f, 1.0f, 0.0f)),
m_forward(vec3f(0.0f, 0.0f, 1.0f)),
m_right(vec3f(1.0f, 0.0f, 0.0f)),
m_isTurning(false),
m_isMoving(false),
m_currAngleIndex(1),
m_yaw(0.0f),
m_targetYaw(m_yaw),
m_pitch(0.0f),
m_roll(0.0f),
m_targetPos(0.0f)
{
	m_oriMap[1] = Orientation::SOUTH;
	m_oriMap[2] = Orientation::WEST;
	m_oriMap[3] = Orientation::NORTH;
	m_oriMap[4] = Orientation::EAST;
	m_validAngles = {{-ANGLE, 0.0f, ANGLE, 2*ANGLE, 3*ANGLE, 4*ANGLE}};	
	D3DXMatrixPerspectiveFovLH(&m_projMatrix, FOV, AppConfig::ASPECT, ZNEAR, ZFAR);
}
Camera::~Camera(){}

void Camera::moveTo(const float targetPos)
{
	if (m_isMoving || m_isTurning) return;
	m_targetPos = targetPos;
	m_isMoving = true;
}

void Camera::turn(const Direction& dir)
{
	if (m_isTurning || m_isMoving) return;
	m_isTurning = true;

	if (dir == Direction::LEFT)
	{
		if (m_currAngleIndex == VALID_ANGLE_LO)
		{
			m_currAngleIndex = VALID_ANGLE_HI;
			m_targetYaw = m_validAngles[0]; 
		}
		else 
		{
			m_currAngleIndex--;
			m_targetYaw = m_validAngles[m_currAngleIndex]; 
		}
	}
	else if (dir == Direction::RIGHT)
	{
		if (m_currAngleIndex == VALID_ANGLE_HI)
		{
			m_currAngleIndex = VALID_ANGLE_LO;
			m_targetYaw = m_validAngles[N_ANGLES - 1]; 
		}
		else 
		{
			m_currAngleIndex++;
			m_targetYaw = m_validAngles[m_currAngleIndex]; 
		}
	}
}

void Camera::update(float& outDy)
{
	outDy = 0.0f;
	if (m_isTurning)
	{
		float prevYaw = m_yaw;
		if (util::lerp(m_yaw, m_targetYaw, TURN_SPEED, m_yaw))
		{
			m_isTurning = false;
			m_yaw = m_validAngles[m_currAngleIndex];
			m_targetYaw = m_yaw;
		}
		outDy = m_yaw - prevYaw;
	}
	else if (m_isMoving)
	{
		m_position += m_forward * MOVE_SPEED;
		if (m_oriMap[m_currAngleIndex] == NORTH && m_position.z <= m_targetPos)
		{
			m_isMoving = false;
			m_position.z = m_targetPos;
		}
		else if (m_oriMap[m_currAngleIndex] == SOUTH && m_position.z >= m_targetPos)
		{		
			m_isMoving = false;
			m_position.z = m_targetPos;
		}
		else if (m_oriMap[m_currAngleIndex] == WEST && m_position.x >= m_targetPos)
		{
			m_isMoving = false;
			m_position.x = m_targetPos; 
		}
		else if (m_oriMap[m_currAngleIndex] == EAST&& m_position.x <= m_targetPos)
		{
			m_isMoving = false;
			m_position.x = m_targetPos;
		}
	}
}

const mat4x4& Camera::computeViewMatrix()
{
	m_up = vec3f(0.0f, 1.0f, 0.0f);
	m_forward = vec3f(0.0f, 0.0f, 1.0f);
	m_right = vec3f(1.0f, 0.0f, 0.0f);

	mat4x4 yawMatrix;
	D3DXMatrixRotationAxis(&yawMatrix, &m_up, m_yaw);
	D3DXVec3TransformCoord(&m_forward, &m_forward, &yawMatrix);
	D3DXVec3TransformCoord(&m_right, &m_right, &yawMatrix);

	mat4x4 pitchMatrix;
	D3DXMatrixRotationAxis(&pitchMatrix, &m_right, m_pitch);
	//D3DXVec3TransformCoord(&mLook, &mLook, &pitchMatrix);
	D3DXVec3TransformCoord(&m_up, &m_up, &pitchMatrix);

	mat4x4 rollMatrix;
	D3DXMatrixRotationAxis(&rollMatrix, &m_forward, m_roll);
	D3DXVec3TransformCoord(&m_right, &m_right, &rollMatrix);
	D3DXVec3TransformCoord(&m_up, &m_up, &rollMatrix);

	D3DXMatrixIdentity(&m_viewMatrix);

	m_viewMatrix._11 = m_right.x; 
	m_viewMatrix._21 = m_right.y;  
	m_viewMatrix._31 = m_right.z;  

	m_viewMatrix._12 = m_up.x;  
	m_viewMatrix._22 = m_up.y;
	m_viewMatrix._32 = m_up.z;  

	m_viewMatrix._13 = m_forward.x;
	m_viewMatrix._23 = m_forward.y;
	m_viewMatrix._33 = m_forward.z;

	m_viewMatrix._41 = -D3DXVec3Dot(&m_position, &m_right);
	m_viewMatrix._42 = -D3DXVec3Dot(&m_position, &m_up);
	m_viewMatrix._43 = -D3DXVec3Dot(&m_position, &m_forward);

	return m_viewMatrix;
}

void Camera::getCameraFrustum(
	const mat4x4& matView,
	mat4x4 matProj,
	Frustum& outFrustum) const
{
	outFrustum = {};

	float r = ZFAR / (ZFAR - ZNEAR);
	matProj._33 = r;
	matProj._43 = -r * ZNEAR;

	mat4x4 matrix = matView * matProj;
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

const mat4x4& Camera::getProjMatrix() const { return m_projMatrix; }
const vec3f& Camera::getPosition() const { return m_position; }
const vec3f& Camera::getForward() const { return m_forward; }
const vec3f& Camera::getRight() const { return m_right; }
const vec3f& Camera::getUp() const { return m_up; }

Orientation Camera::getOrientation() const { return m_oriMap.at(m_currAngleIndex); }
bool Camera::isMoving() const { return m_isMoving; }
bool Camera::isTurning() const { return m_isTurning; }

void Camera::setPosition(const vec3f& pos) { m_position = pos; }
void Camera::setPosition(const float x, const float y, const float z) { m_position = vec3f{x, y, z}; }