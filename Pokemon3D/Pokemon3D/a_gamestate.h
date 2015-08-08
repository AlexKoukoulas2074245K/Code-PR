#pragma once
#include "common.h"
#include "pokedef.h"

class GameState
{
public:
	virtual ~GameState(){};
	virtual bool initialize() = 0;
	virtual void update() = 0;
	virtual void render() = 0;

	bool isFinished() const { return m_isFinished; }

protected:
	bool m_isFinished;
};