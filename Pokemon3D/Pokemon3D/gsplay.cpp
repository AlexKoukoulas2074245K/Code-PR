#include "gsplay.h"

#define PLR_START_LOC D3DXVECTOR3(-11.0f, 0.0f, 25.0f)

GSPlay::GSPlay():
mPlayer(PLR_START_LOC)
{
	mFinished = false;
	LOGLN("GSPlay constructor called"); 
}

bool GSPlay::Initialize()
{
	return mLevel.Initialize(mRenderer, mIOManager);
}

void GSPlay::Update()
{
	mPlayer.Update(mHIDManager);
	mLevel.Update();
}

void GSPlay::Render()
{
	Camera& plrcam = mPlayer.getCamera();
	const D3DXMATRIX& viewMatrix = plrcam.getViewMatrix();
	const D3DXMATRIX& projMatrix = plrcam.getProjMatrix();
	const D3DXVECTOR4 camPos = plrcam.getPosition();
	Camera::Frustum camFrustum = {};
	plrcam.getCameraFrustum(viewMatrix, projMatrix, camFrustum);
	mRenderer->PrepareFrame(viewMatrix, projMatrix, camPos, camFrustum);
	mLevel.Render();
	mRenderer->CompleteFrame();
}