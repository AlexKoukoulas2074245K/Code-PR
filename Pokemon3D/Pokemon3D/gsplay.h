#pragma once
#include "a_gamestate.h"

class Player;
class Level;
class GSPlay: public GameState
{
public:
	GSPlay();
	~GSPlay();

	bool initialize();
	void update();
	void render();

private:
	static const vec3f PLR_START_LOC;

	sptr<Player> m_pPlayer;
	sptr<Level> m_pLevel;
};