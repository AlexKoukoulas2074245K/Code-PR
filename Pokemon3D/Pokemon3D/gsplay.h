#pragma once

#include "a_gamestate.h"
#include "camera.h"
#include "body.h"
#include "level.h"

class GSPlay: public GameState
{
public:
	GSPlay();
	~GSPlay();
	bool Initialize();
	void Update();
	void Render();

private:
	Body mBody;
	Camera mCamera;
	Level mLevel;
};