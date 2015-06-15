#include "gsqueue.h"

GSQueue::GSQueue(): mEmpty(false){ LOGLN("GSQueue constructor called"); }

GSQueue::~GSQueue()
{
	LOGLN("GSQueue destructor called.");
}

bool GSQueue::Initialize(const HWND& hWindow)
{
	/* Initialize the renderer */
	if (!mRenderer.Initialize(hWindow)) return false;
	
	/* Add and initialize game states */
	mStates.push_back(value_t(new GSPlay));
	
	mIOManager.ForceGetBody("C:/Users/alex/Pictures/projects/pkmnrevo/models/house.obj", mBody);
	mBody.setSingleTexture("C:/Users/alex/Pictures/projects/pkmnrevo/textures/backup/house.png");
	mRenderer.PrepareBody(mBody, Renderer::ShaderType::DEFAULT);

	mCamera.Initialize(D3DXVECTOR3(0.0f, 0.0f, 25.0f));

	for (cnst_iter iter = mStates.begin();
		 iter != mStates.end();
		 ++iter)
	{
		if (!iter->get()->Initialize()) return false;
		iter->get()->setRenderer(mRenderer);
	}

	return true;
}

void GSQueue::HandleInput(const MSG& msg)
{
	mHIDManager.UpdateState(msg);
}

void GSQueue::Update()
{
	mStates.front()->Update();
	if (mStates.front()->isFinished()) 
	{
		mStates.pop_front();
		mEmpty = mStates.empty();
	}

	if (mHIDManager.KeyTapped(KEY_LEFT)) mCamera.Turn(Camera::LEFT);
	if (mHIDManager.KeyTapped(KEY_RIGHT)) mCamera.Turn(Camera::RIGHT);
	if (mHIDManager.KeyDown(KEY_UP)) mCamera.Move(Camera::FORWARD, 0.1f);
	if (mHIDManager.KeyDown(KEY_DOWN)) mCamera.Move(Camera::BACKWARD, 0.1f);

	mHIDManager.CompleteFrame();
}

void GSQueue::Render()
{
	mRenderer.PrepareFrame(mCamera.getViewMatrix(), mCamera.getPosition());
	mRenderer.RenderBody(mBody, Renderer::ShaderType::DEFAULT);
	mStates.front()->Render();
	mRenderer.CompleteFrame();
}