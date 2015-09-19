#pragma once
#include "a_gamestate.h"

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
	static const uint2 PLR_START_COORDS;
	sptr<Player> m_pPlayer;
	sptr<Level> m_pLevel;
	sptr<FpsCounter> m_pFpsCounter;
	std::string m_gameInfo[GAME_INFO_SIZE];
};