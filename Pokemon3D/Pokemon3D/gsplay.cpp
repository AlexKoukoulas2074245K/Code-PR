#include "gsplay.h"

#include "player.h"
#include "level.h"
#include "camera.h"
#include "renderer.h"
#include "body.h"
#include "hudcomponent.h"
#include "fontengine.h"

const vec3f GSPlay::PLR_START_LOC(-15.4f, 0.0f, 22.8f);

GSPlay::GSPlay():
mPlayer(new Player(PLR_START_LOC, mLevel)),
mLevel(new Level),
mFont(new FontEngine("stdfont"))
{
	mFinished = false;
	LOGLN("GSPlay constructor called");
}

bool GSPlay::Initialize()
{
	if(!mLevel->Initialize(mRenderer, mIOManager)) return false;
	mPlayer->FindPosition();
	mFont->PrepareFont(mIOManager, mRenderer);
	return true;
}

void GSPlay::Update()
{
	mPlayer->Update(mHIDManager);
	mLevel->Update();
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
	std::list<std::string> phrase;
	phrase.push_back("A");
	phrase.push_back("b");
	phrase.push_back("!");
	phrase.push_back(",");
	mRenderer->RenderText(phrase, float2{0.0f, 0.0f}, mFont.get());
	mRenderer->CompleteFrame();
}