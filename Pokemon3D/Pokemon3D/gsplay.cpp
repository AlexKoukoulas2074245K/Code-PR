#include "gsplay.h"

#include "player.h"
#include "level.h"
#include "camera.h"
#include "renderer.h"
#include "fontengine.h"
#include "hidmanager.h"
#include "menuboxbody.h"
#include "fontengine.h"
#include "fpscounter.h"

const vec3f GSPlay::PLR_START_LOC(-15.4f, 0.0f, 22.8f);
float x = -0.95f;

GSPlay::GSPlay():
m_pPlayer(new Player(PLR_START_LOC, m_pLevel)),
m_pLevel(new Level),
m_pFpsCounter(new FpsCounter)
{
	m_isFinished = false;
	LOGLN("GSPlay constructor called");
}

bool GSPlay::initialize()
{
	if(!m_pLevel->initialize()) return false;
	m_pPlayer->computePosition();
	
	return true;
}

void GSPlay::update()
{
	m_pPlayer->update();
	m_pLevel->update();
	if (HIDManager::get().keyDown(KEY_A)) x += 0.01f;
	if (HIDManager::get().keyDown(KEY_B)) x -= 0.01f;
	m_pFpsCounter->update();
}

void GSPlay::render()
{
	uptr<Camera>& rPlayerCamera = m_pPlayer->modCameraPointer();
	const mat4x4& rViewMatrix = rPlayerCamera->computeViewMatrix();
	const mat4x4& rProjMatrix = rPlayerCamera->getProjMatrix();
	const vec4f cameraPos = rPlayerCamera->getPosition();
	Camera::Frustum camFrustum = {};
	rPlayerCamera->getCameraFrustum(rViewMatrix, rProjMatrix, camFrustum);
	Renderer::get().prepareFrame(rViewMatrix, rProjMatrix, cameraPos, camFrustum);
	m_pLevel->render();
	Renderer::get().renderText("Fps: " + std::to_string(m_pFpsCounter->getFPS()), {-0.95f, 0.8f}, COLOR_BLACK);
	Renderer::get().renderText("Player coll: " + std::to_string(m_pPlayer->canCollide()), {-0.95, 0.7f}, COLOR_BLACK);
}