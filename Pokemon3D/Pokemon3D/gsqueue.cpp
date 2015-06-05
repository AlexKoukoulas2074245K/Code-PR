#include "gsqueue.h"

GSQueue::GSQueue(): mEmpty(false){ LOGLN("GSQueue constructor called"); }

GSQueue::~GSQueue()
{
	LOGLN("GSQueue destructor called.");
}

bool GSQueue::Initialize()
{
	mStates.push_back(value_t(new GSPlay));
	
	for (cnst_iter iter = mStates.begin();
		iter != mStates.end();
		++iter)
	{
		if (!iter->get()->Initialize()) return false;
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
	mStates.front()->Render();
}