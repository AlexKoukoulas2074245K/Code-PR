#pragma once

#include "a_gamestate.h"
#include "body.h"
#include "level.h"
#include "player.h"

class GSPlay: public GameState
{
public:
	GSPlay();
	~GSPlay(){};
	bool Initialize();
	void Update();
	void Render();

private:
	Player mPlayer;
	Level mLevel;
};