#include "gsplay.h"

#include "player.h"
#include "level.h"
#include "camera.h"
#include "renderer.h"
#include "fontengine.h"
#include "hidmanager.h"
#include "fontengine.h"
#include "fpscounter.h"
#include "menuboxbody.h"
#include "iomanager.h"

const uint2 GSPlay::PLR_START_COORDS{4, 4};
float x = -0.95f;

GSPlay::GSPlay():
m_pPlayer(new Player(PLR_START_COORDS, m_pLevel)),
m_pLevel(new Level),
m_pFpsCounter(new FpsCounter)
{
	m_isFinished = false;
}

bool GSPlay::initialize()
{
	m_gameInfo[0] = "ASH";
	m_gameInfo[1] = "GARRY";

	if(!m_pLevel->initialize("pallet", "playerhouse2", m_pPlayer)) return false;
	m_pPlayer->initialize(m_gameInfo);

	//m_pMenuBox->fill("\nPOK^MON\n\nITEM\n\nAA\n\nSAVE\n\nOPTION\n\nEXIT", float2{0.52f, 0.85f}, true);

	return true;
}

void GSPlay::update()
{
	m_pPlayer->update();
	m_pLevel->update(m_pPlayer->getCameraRotDy());
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
	m_pPlayer->render();

	/* -------------Debugging--------------- */
	Renderer::get().renderText("Fps: " + std::to_string(m_pFpsCounter->getFPS()), {-0.95f, 0.8f}, pokecolors::COLOR_BLACK);
	Renderer::get().renderText("Plr coll: " + std::to_string(m_pPlayer->canCollide()), {-0.95f, 0.7f}, pokecolors::COLOR_BLACK);
	Renderer::get().renderText("Plr x: " + std::to_string(m_pPlayer->getLevelPos().x), {-0.95f, 0.6f}, pokecolors::COLOR_BLACK);
	Renderer::get().renderText("Plr z: " + std::to_string(m_pPlayer->getLevelPos().z), {-0.95f, 0.5f}, pokecolors::COLOR_BLACK);
	Renderer::get().renderText(
		"Plr coords: " + std::to_string(m_pPlayer->getLevelCoords().x) + ", " + 
						 std::to_string(m_pPlayer->getLevelCoords().y) ,
						 {-0.95f, 0.4f},
						 pokecolors::COLOR_BLACK);
	if (m_pPlayer->modCameraPointer()->getOrientation() == NORTH)
		Renderer::get().renderText("Cam ori: NORTH", {-0.95f, 0.3f}, pokecolors::COLOR_BLACK);
	else if (m_pPlayer->modCameraPointer()->getOrientation() == EAST)
		Renderer::get().renderText("Cam ori: EAST", {-0.95f, 0.3f}, pokecolors::COLOR_BLACK);
	else if (m_pPlayer->modCameraPointer()->getOrientation() == SOUTH)
		Renderer::get().renderText("Cam ori: SOUTH", {-0.95f, 0.3f}, pokecolors::COLOR_BLACK);
	else 
		Renderer::get().renderText("Cam ori: WEST", {-0.95f, 0.3f}, pokecolors::COLOR_BLACK);
}