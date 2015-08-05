#include "gsplay.h"

#include "player.h"
#include "level.h"
#include "camera.h"
#include "renderer.h"
#include "body.h"
#include "hudcomponent.h"
#include "hidmanager.h"

const vec3f GSPlay::PLR_START_LOC(-15.4f, 0.0f, 22.8f);

GSPlay::GSPlay():
m_pPlayer(new Player(PLR_START_LOC, m_pLevel)),
m_pLevel(new Level)
{
	m_isFinished = false;
	LOGLN("GSPlay constructor called");
}

GSPlay::~GSPlay(){}

bool GSPlay::initialize()
{
	if(!m_pLevel->initialize(m_pRenderer, m_pIOManager)) return false;
	m_pPlayer->computePosition();
	return true;
}

void GSPlay::update()
{
	m_pPlayer->update(m_pHIDManager);
	m_pLevel->update();
}

void GSPlay::render()
{
	uptr<Camera>& rPlayerCamera = m_pPlayer->modCameraPointer();
	const mat4x4& rViewMatrix = rPlayerCamera->computeViewMatrix();
	const mat4x4& rProjMatrix = rPlayerCamera->getProjMatrix();
	const vec4f cameraPos = rPlayerCamera->getPosition();
	Camera::Frustum camFrustum = {};
	rPlayerCamera->getCameraFrustum(rViewMatrix, rProjMatrix, camFrustum);
	m_pRenderer->prepareFrame(rViewMatrix, rProjMatrix, cameraPos, camFrustum);
	m_pLevel->render();
}