#pragma once

#include <list>
#include "a_gamestate.h"
#include "gsplay.h"
#include "iomanager.h"


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
	void HandleInput();
	void Update();
	void Render();
	bool isEmpty() const { return mEmpty; }

private:
	bool mEmpty;
	gs_list mStates;
	Renderer mRenderer;
};