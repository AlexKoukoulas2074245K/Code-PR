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
	
	IOManager iom;
	iom.LoadMultipleBodies("C:/users/alex/pictures/projects/pkmnrevo/models/*");
	/* Add and initialize game states */
	mStates.push_back(value_t(new GSPlay));
	
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
	mRenderer.ClearFrame();
	mStates.front()->Render();
}