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
		float x, y, z;
	} level_pos;

	typedef struct LevelCoords
	{
		uint x, y;
	} level_coords;

	typedef unsigned int** level_map;
	typedef IOManager::static_geometry static_object;
	typedef IOManager::std_dims level_dims;
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
	void GetCoords(const level_pos& inPos, level_coords& outCoords) const;
	void GetPosition(const level_coords& inCoords, level_pos& outPosition) const;
	uint GetTileType(const level_coords& inCoords) const;
	uint GetTileType(const level_pos& inPos) const;

private:
	static const float LAKE_CURRENT_VEL;
	static const float LAKE_CURRENT_MAX_DISTANCE;
	static const unsigned int LAKE_MAX_FRAMES_PER_CURRENT;

	sptr<Renderer> mRenderer;
	static_object_list mObjects;
	lake_object_list mLakePieces;
	level_map mLevelMap;
	level_dims mLevelDims;
	bool mLakeCurrentLeft;
	unsigned int mLakeFrameCounter;
};