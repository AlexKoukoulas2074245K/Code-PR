#pragma once
#include "a_gamestate.h"
#include "menuboxbody.h"

class Player;
class Level;
class FpsCounter;
class GSPlay: public GameState
{
public:
	GSPlay();
	bool initialize();
	void update();
	void render();

private:
	static const vec3f PLR_START_LOC;
	MenuBoxBody mbb;
	sptr<Player> m_pPlayer;
	sptr<Level> m_pLevel;
	sptr<FpsCounter> m_pFpsCounter;
};