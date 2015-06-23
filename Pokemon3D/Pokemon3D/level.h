#pragma once
#include <list>
#include "body.h"
#include "common.h"
#include "renderer.h"
#include "iomanager.h"

class Level
{
public:
	static const float TILE_SIZE;

	typedef struct LevelPos
	{
		float levelPosX, levelPosY, levelPosZ;
	} level_pos;

	typedef IOManager::std_body game_object;
	typedef std::list<game_object> game_object_list;
	typedef game_object_list::iterator go_list_iter;
	typedef game_object_list::const_iterator go_list_const_iter;

	Level();
	~Level();

	bool Initialize(
		const sptr<Renderer> renderer,
		const sptr<IOManager> iomanager);
	void Render();

private:
	sptr<Renderer> mRenderer;
	game_object_list mObjects;
};