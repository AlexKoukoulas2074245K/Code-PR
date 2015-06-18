#include "level.h"
#include <fstream>

Level::Level(){}
Level::~Level(){}

bool Level::Initialize(
	const sptr<Renderer> renderer,
	const sptr<IOManager> iomanager)
{
	mRenderer = renderer;
	iomanager->GetAllBodiesFromLevel("C:/Users/alex/Pictures/projects/pkmnrevo/levels/test.lvl", mObjects);
	for (std::list<Body>::iterator iter = mObjects.begin();
		iter != mObjects.end();
		++iter)
	{
		mRenderer->PrepareBody(*iter, Renderer::ShaderType::DEFAULT);
	}

	return true;
}

void Level::Render()
{
	for (std::list<Body>::iterator iter = mObjects.begin();
		iter != mObjects.end();
		++iter)
	{
		mRenderer->RenderBody(*iter, Renderer::ShaderType::DEFAULT);
	}
}