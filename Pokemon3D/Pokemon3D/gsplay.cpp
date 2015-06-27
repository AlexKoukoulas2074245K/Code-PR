#include "gsplay.h"

GSPlay::GSPlay(){ mFinished = false; LOGLN("GSPlay constructor called"); }
GSPlay::~GSPlay(){ LOGLN("GSPlay destructor called"); }

bool GSPlay::Initialize()
{
	//mIOManager->ForceGetBody("C:/Users/alex/Pictures/projects/pkmnrevo/models/house.obj", mBody);
	//mBody.setSingleTexture("C:/Users/alex/Pictures/projects/pkmnrevo/textures/materials/house.png");
	//mRenderer->PrepareBody(mBody, Renderer::ShaderType::DEFAULT);
	mCamera.Initialize(D3DXVECTOR3(-11.0f, 0.0f, 25.0f));
	return mLevel.Initialize(mRenderer, mIOManager);
}

void GSPlay::Update()
{
	if (mHIDManager->KeyTapped(KEY_LEFT)) mCamera.Turn(Camera::LEFT);
	if (mHIDManager->KeyTapped(KEY_RIGHT)) mCamera.Turn(Camera::RIGHT);
	if (mHIDManager->KeyDown(KEY_UP)) mCamera.Move(Camera::FORWARD, 0.1f);
	if (mHIDManager->KeyDown(KEY_DOWN)) mCamera.Move(Camera::BACKWARD, 0.1f);
	if (mHIDManager->KeyDown(KEY_A)) mCamera.Look(Camera::UP, 0.01f);
	if (mHIDManager->KeyDown(KEY_B)) mCamera.Look(Camera::DOWN, 0.01f);
	mCamera.Update();
	mLevel.Update();
}

void GSPlay::Render()
{
	mRenderer->PrepareFrame(mCamera.getViewMatrix(), mCamera.getPosition());
	mLevel.Render();
	mRenderer->CompleteFrame();
}