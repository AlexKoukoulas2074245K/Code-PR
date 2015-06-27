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

	typedef IOManager::static_geometry static_object;
	typedef std::list<static_object> static_object_list;
	typedef std::list<static_object> lake_object_list;
	typedef static_object_list::iterator so_list_iter;
	typedef static_object_list::const_iterator so_list_const_iter;
	typedef lake_object_list::iterator lo_list_iter;
	typedef lake_object_list::const_iterator lo_list_const_iter;

	Level();
	~Level();

	bool Initialize(
		const sptr<Renderer> renderer,
		const sptr<IOManager> iomanager);
	void Update();
	void Render();

private:
	static const float LAKE_CURRENT_VEL;
	static const float LAKE_CURRENT_MAX_DISTANCE;
	static const unsigned int LAKE_MAX_FRAMES_PER_CURRENT;

	sptr<Renderer> mRenderer;
	static_object_list mObjects;
	lake_object_list mLakePieces;
	
	bool mLakeCurrentLeft;
	unsigned int mLakeFrameCounter;
};