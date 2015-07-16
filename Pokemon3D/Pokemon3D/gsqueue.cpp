#include "gsqueue.h"

#include "renderer.h"
#include "iomanager.h"
#include "hidmanager.h"
#include "fpscounter.h"
#include "gsplay.h"
#include "a_gamestate.h"

GSQueue::GSQueue():
mRenderer(new Renderer),
mIOManager(new IOManager),
mHIDManager(new HIDManager),
mFpsc(new FpsCounter),
mEmpty(false)
{
	LOGLN("GSQueue constructor called"); 
}

GSQueue::~GSQueue()
{
	LOGLN("GSQueue destructor called.");
}

bool GSQueue::Initialize(const HWND& hWindow)
{
	/* Initialize the renderer */
	if (!mRenderer->Initialize(hWindow)) return false;
	mIOManager->SetRenderer(mRenderer);

	/* Add and initialize game states */
	mStates.push_back(value_t(new GSPlay));
	for (cnst_iter iter = mStates.begin();
		 iter != mStates.end();
		 ++iter)
	{
		iter->get()->setRenderer(mRenderer);
		iter->get()->setIOManager(mIOManager);
		iter->get()->setHIDManager(mHIDManager);
		if (!iter->get()->Initialize()) return false;
	}

	return true;
}

void GSQueue::HandleInput(const MSG& msg)
{
	mHIDManager->UpdateState(msg);
}

void GSQueue::Update()
{
	mStates.front()->Update();
	if (mStates.front()->isFinished()) 
	{
		mStates.pop_front();
		mEmpty = mStates.empty();
	}
	mHIDManager->CompleteFrame();
	mFpsc->Update();
}

void GSQueue::Render()
{
	mStates.front()->Render();
}