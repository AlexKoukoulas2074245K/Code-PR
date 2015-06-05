#pragma once
#include "common.h"

class GameState
{
public:
	virtual ~GameState(){};
	virtual bool Initialize() = 0;
	virtual void HandleInput() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	bool isFinished() const { return mFinished; }

protected:
	bool mFinished;
};