#pragma once

#include "a_gamestate.h"

class GSPlay: public GameState
{
public:
	GSPlay();
	~GSPlay();
	bool Initialize();
	void HandleInput();
	void Update();
	void Render();
};