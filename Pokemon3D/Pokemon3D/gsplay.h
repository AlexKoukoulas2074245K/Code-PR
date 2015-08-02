#pragma once
#include "a_gamestate.h"

class Player;
class Level;
class FontEngine;
class GSPlay: public GameState
{
public:
	GSPlay();
	~GSPlay(){};
	bool Initialize();
	void Update();
	void Render();

private:
	static const vec3f PLR_START_LOC;
	sptr<Player> mPlayer;
	sptr<Level> mLevel;
	sptr<FontEngine> mFont;
};