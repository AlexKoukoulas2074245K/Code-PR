#pragma once
#include <list>

#include "staticmodel.h"
#include "iomanager.h"

class Renderer;
class Level
{
public:
	static const float TILE_SIZE;
	typedef unsigned int** level_map;
	typedef uint3 level_dims;
	typedef std::list<StaticModel> static_object_list;
	typedef std::list<StaticModel> lake_object_list;
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

	bool ValidCoords(const uint x, const uint y) const;
	uint2 GetCoords(const vec3f& inPos) const;
	vec3f GetPosition(const uint2& inCoords, const float y) const;
	uint GetTileType(const uint x, const uint y) const;
	uint GetTileType(const uint2& inCoords) const;
	uint GetTileType(const vec3f& inPos) const;
	const level_dims& GetLevelDims() const { return mLevelDims; }

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