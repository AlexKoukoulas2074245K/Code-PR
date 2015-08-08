#pragma once
#include <list>

#include "pokedef.h"

class GameState;
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
	
	bool initialize(const HWND& hWindow);
	void handleInput(const MSG& msg);
	void update();
	void render();

	bool isEmpty() const;

private:
	bool m_isEmpty;
	gs_list m_states;
};