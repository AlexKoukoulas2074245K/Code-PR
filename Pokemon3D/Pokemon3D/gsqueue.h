#pragma once
#include <list>

#include "pokedef.h"

class GameState;
class Renderer;
class HIDManager;
class IOManager;
class FpsCounter;
class GSQueue
{
public:
	/* Convenient typedefs */
	typedef uptr<GameState> value_t;
	typedef std::list<value_t> gs_list;
	typedef gs_list::const_iterator cnst_iter;
	typedef gs_list::iterator iter;

	/* Standard gamestate methods and queue accessors */
	GSQueue();	
	~GSQueue();
	bool Initialize(const HWND& hWindow);
	void HandleInput(const MSG& msg);
	void Update();
	void Render();
	bool isEmpty() const { return mEmpty; }

private:
	bool mEmpty;
	gs_list mStates;
	sptr<Renderer> mRenderer;
	sptr<IOManager> mIOManager;
	sptr<HIDManager> mHIDManager;
	uptr<FpsCounter> mFpsc;
};