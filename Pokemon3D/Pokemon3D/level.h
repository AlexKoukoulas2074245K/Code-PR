#pragma once
#include <list>

#include "staticmodel.h"

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

	bool initialize();
	void update();
	void render();

	bool validCoords(const uint x, const uint y) const;
	uint2 getCoords(const vec3f& inPos) const;
	vec3f getPosition(const uint2& inCoords, const float y) const;
	uint getTileType(const uint x, const uint y) const;
	uint getTileType(const uint2& inCoords) const;
	uint getTileType(const vec3f& inPos) const;
	const level_dims& getLevelDims() const;

private:
	static const float LAKE_CURRENT_VEL;
	static const float LAKE_CURRENT_MAX_DISTANCE;
	static const unsigned int LAKE_MAX_FRAMES_PER_CURRENT;

private:
	static_object_list m_objects;
	lake_object_list m_lakePieces;
	level_map m_levelMap;
	level_dims m_levelDims;
	bool m_lakeCurrentLeft;
	unsigned int m_lakeFrameCounter;
};