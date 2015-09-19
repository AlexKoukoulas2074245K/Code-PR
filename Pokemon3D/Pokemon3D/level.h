#pragma once
#include <list>

#include "staticmodel.h"

class Npc;
class OWObject;
class Player;
class Level
{
public:
	static const float TILE_SIZE;
	typedef uint3 level_dims;
	typedef std::list<StaticModel> static_object_list;
	typedef std::list<StaticModel> lake_object_list;
	typedef std::list<sptr<Npc>> npc_list;
	typedef std::list<sptr<OWObject>> obj_list;
	typedef static_object_list::iterator so_list_iter;
	typedef static_object_list::const_iterator so_list_const_iter;
	typedef lake_object_list::iterator lo_list_iter;
	typedef lake_object_list::const_iterator lo_list_const_iter;
	

	Level();
	~Level();

	bool initialize(
		const std::string& area,
		const std::string& subarea,
		const sptr<Player> player);
	void update(const float drot);
	void render();

	bool validCoords(const uint x, const uint y) const;
	bool validCoords(const uint2& xy) const;
	uint2 getCoords(const vec3f& inPos) const;
	vec3f getPosition(const uint2& inCoords) const;
	uint getTileType(const uint x, const uint y) const;
	uint getTileType(const uint2& inCoords) const;
	uint getTileType(const vec3f& inPos) const;
	const level_dims& getLevelDims() const;
	Npc* getNpcAt(const uint2& inCoords);
	OWObject* getObjectAt(const uint2& inCoords);

private:
	void findNpcs();
	void findObjects();

private:
	static const float LAKE_CURRENT_VEL;
	static const float LAKE_CURRENT_MAX_DISTANCE;
	static const unsigned int LAKE_MAX_FRAMES_PER_CURRENT;
	static const std::string LEVEL_FILE_EXT;

private:
	static_object_list m_objects;
	lake_object_list m_lakePieces;
	npc_list m_npcList;
	obj_list m_objList;
	level_dims m_levelDims;
	bool m_lakeCurrentLeft;
	unsigned int m_lakeFrameCounter;
	unsigned int** m_levelMap;
	std::string m_area;
	std::string m_subarea;
	sptr<Player> m_pPlayer;
};