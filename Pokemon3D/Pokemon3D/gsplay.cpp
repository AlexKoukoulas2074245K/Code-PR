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
mPlayer(new Player(PLR_START_LOC, mLevel)),
mLevel(new Level)
{
	mFinished = false;
	LOGLN("GSPlay constructor called");
}

bool GSPlay::Initialize()
{
	if(!mLevel->Initialize(mRenderer, mIOManager)) return false;
	mPlayer->FindPosition();
	return true;
}

void GSPlay::Update()
{
	mPlayer->Update(mHIDManager);
	mLevel->Update();
	/*if (mHIDManager->KeyTapped(KEY_UP)) Renderer::n++;
	else if (mHIDManager->KeyTapped(KEY_DOWN)) Renderer::n--;*/
}

void GSPlay::Render()
{
	uptr<Camera>& plrcam = mPlayer->getCamera();
	const mat4x4& viewMatrix = plrcam->getViewMatrix();
	const mat4x4& projMatrix = plrcam->getProjMatrix();
	const vec4f camPos = plrcam->getPosition();
	Camera::Frustum camFrustum = {};
	plrcam->getCameraFrustum(viewMatrix, projMatrix, camFrustum);
	mRenderer->PrepareFrame(viewMatrix, projMatrix, camPos, camFrustum);
	mLevel->Render();
}