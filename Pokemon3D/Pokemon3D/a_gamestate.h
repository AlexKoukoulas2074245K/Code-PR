#pragma once
#include "common.h"
#include "renderer.h"

class GameState
{
public:
	virtual ~GameState(){};
	virtual bool Initialize() = 0;
	virtual void HandleInput() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;

	bool isFinished() const { return mFinished; }
	void setRenderer(const Renderer& renderer){ mRend = renderer; }

protected:
	bool mFinished;
	Renderer mRend;
};