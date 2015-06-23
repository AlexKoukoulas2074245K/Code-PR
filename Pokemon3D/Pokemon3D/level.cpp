#include "level.h"
#include <fstream>

const float Level::TILE_SIZE = 1.6f;

Level::Level(){}
Level::~Level(){}

#include "clock.h"
bool Level::Initialize(
	const sptr<Renderer> renderer,
	const sptr<IOManager> iomanager)
{
	mRenderer = renderer;
	mObjects.clear();
	Clock c;
	if (!c.InitClock()) return false;
	iomanager->GetAllBodiesFromLevel("C:/Users/alex/Pictures/projects/pkmnrevo/levels/test.lvl", TILE_SIZE, mObjects);
	LOGLN(("Completed in: " + std::to_string(c.GetCounter()) + " ms.").c_str());
	return true;
}

void Level::Render()
{
	for (go_list_iter iter = mObjects.begin();
		iter != mObjects.end();
		++iter)
	{
		mRenderer->RenderBody(Renderer::ShaderType::DEFAULT, iter->pos, iter->body);
	}
}