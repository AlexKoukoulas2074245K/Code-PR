#pragma once
#include <list>
#include "body.h"
#include "common.h"
#include "renderer.h"
#include "iomanager.h"

class Level
{
public:
	Level();
	~Level();

	bool Initialize(
		const sptr<Renderer> renderer,
		const sptr<IOManager> iomanager);
	void Render();

private:
	sptr<Renderer> mRenderer;
	std::list<Body> mObjects;
};