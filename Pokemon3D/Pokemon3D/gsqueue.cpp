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
	
	IOManager iom;
	iom.ForceGetBody("C:/Users/alex/Pictures/projects/pkmnrevo/models/house.obj", body);
	body.setSingleTexture("C:/Users/alex/Pictures/projects/pkmnrevo/textures/backup/house.png");
	camera.Initialize(D3DXVECTOR3(0.0f, 0.0f, 25.0f));

	mRenderer.PrepareBody(body, Renderer::ShaderType::DEFAULT);

	for (cnst_iter iter = mStates.begin();
		 iter != mStates.end();
		 ++iter)
	{
		if (!iter->get()->Initialize()) return false;
		iter->get()->setRenderer(mRenderer);
	}

	return true;
}

void GSQueue::HandleInput()
{
	mStates.front()->HandleInput();
}

void GSQueue::Update()
{
	mStates.front()->Update();
	if (mStates.front()->isFinished()) 
	{
		mStates.pop_front();
		mEmpty = mStates.empty();
	}
}

void GSQueue::Render()
{
	mRenderer.PrepareFrame(camera.getViewMatrix(), camera.getPosition());
	mRenderer.RenderBody(body, Renderer::ShaderType::DEFAULT);
	mStates.front()->Render();
	mRenderer.CompleteFrame();
}