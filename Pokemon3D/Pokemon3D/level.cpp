#include "level.h"

#include <fstream>

#include "clock.h"
#include "renderer.h"
#include "iomanager.h"
#include "ioutils.h"
#include "npc.h"
#include "player.h"
#include "owobject.h"

const std::string Level::LEVEL_FILE_EXT = ".lvl";
const float Level::TILE_SIZE = 1.6f;
const float Level::LAKE_CURRENT_VEL = 0.001f;
const float Level::LAKE_CURRENT_MAX_DISTANCE = 0.4f;
const unsigned int Level::LAKE_MAX_FRAMES_PER_CURRENT = 
	static_cast<int>(Level::LAKE_CURRENT_MAX_DISTANCE / Level::LAKE_CURRENT_VEL);

Level::Level(){}
Level::~Level()
{
	for (uint i = 0; i < m_levelDims.y; ++i) delete m_levelMap[i];
	delete[] m_levelMap;
}


bool Level::initialize(
	const std::string& area,
	const std::string& subarea,
	const sptr<Player> player)
{
	m_area = area;
	m_subarea = subarea;
	m_lakeFrameCounter = 0;
	m_lakeCurrentLeft = true;
	m_objects.clear();
	m_lakePieces.clear();
	m_npcList.clear();
	m_pPlayer = player;

	Clock c;
	if (!c.initClock()) return false;
	IOManager::get().getAllBodiesFromLevel(
		area + subarea + LEVEL_FILE_EXT,
		TILE_SIZE,
		m_levelDims,
		m_objects,
		m_lakePieces,
		m_levelMap);

	findNpcs();
	findObjects();

	LOGLN(("Level Initialization Completed in: " + std::to_string(c.getCounter()) + " ms.").c_str());
	return true;
}

void Level::findNpcs()
{
	IOManager::str_list contentLines;
	IOManager::str npcPath;
	IOManager::get().getPathOf("npcs", IOManager::DAT, npcPath);
	IOManager::get().getFileContent(
		npcPath,
		&contentLines);

	for (IOManager::str_list_iter iter = contentLines.begin();
		 iter != contentLines.end();
		 ++iter)
	{
		std::vector<std::string> splitInfo = split(*iter, ' ');
		std::string levelArea = splitInfo[0];
		std::string levelSubArea = splitInfo[1];

		if (levelArea.compare(m_area) || levelSubArea.compare(m_subarea)) continue;

		std::vector<std::string> levelCoordstr = split(splitInfo[2], ',');
		uint2 levelCoords = {std::stoi(levelCoordstr[0]), std::stoi(levelCoordstr[1])};

		std::vector<std::string> texCoordstr = split(splitInfo[3], ',');
		uint2 npcTexCoords = {std::stoi(texCoordstr[0]), std::stoi(texCoordstr[1])};

		bool movingNpc = !splitInfo[4].compare("true");
			
		int orientation = std::stoi(splitInfo[5]);
		Orientation npcOri = Orientation::SOUTH;
		switch (orientation)
		{
		case 0: npcOri = Orientation::NORTH; break;
		case 1: npcOri = Orientation::EAST; break;
		case 2: npcOri = Orientation::SOUTH; break;
		case 3: npcOri = Orientation::WEST; break;
		}

		bool hostileNpc = !splitInfo[6].compare("true");

		std::string npcText;
		size_t msgIndexOffset = hostileNpc ? 0 /*TODO*/ : 7;
		npcText += splitInfo[msgIndexOffset];
		for (std::vector<std::string>::const_iterator citer = splitInfo.begin() + msgIndexOffset + 1;
			citer != splitInfo.end();
			++citer) npcText += " " + *citer;

		sptr<Npc> npc(new Npc);
		npc->initialize(
			levelArea,
			levelSubArea,
			getPosition(levelCoords),
			levelCoords,
			npcTexCoords,
			movingNpc,
			npcOri,
			hostileNpc,
			npcText,
			this);

		m_npcList.push_back(npc);
	}
}

void Level::findObjects()
{
	IOManager::str_list contentLines;
	IOManager::str objectPath;
	IOManager::get().getPathOf("objects", IOManager::DAT, objectPath);
	IOManager::get().getFileContent(
		objectPath,
		&contentLines);

	for (IOManager::str_list_iter iter = contentLines.begin();
		iter != contentLines.end();
		++iter)
	{
		std::vector<std::string> splitInfo = split(*iter, ' ');
		std::string levelArea = splitInfo[0];
		std::string levelSubArea = splitInfo[1];

		if (levelArea.compare(m_area) || levelSubArea.compare(m_subarea)) continue;

		std::vector<std::string> levelCoordstr = split(splitInfo[2], ',');
		uint2 levelCoords = {std::stoi(levelCoordstr[0]), std::stoi(levelCoordstr[1])};

		bool obtainable = !splitInfo[4].compare("true");
		bool solid = !splitInfo[5].compare("true");
		bool canBeCut = !splitInfo[6].compare("true");

		uint objAttributes = 0U;
		if (obtainable) objAttributes |= OBJECT_OBT;
		if (solid) objAttributes |= OBJECT_SOLID;
		if (canBeCut) objAttributes |= OBJECT_CUT;

		std::string objMessage;
		objMessage += splitInfo[7];
		for (std::vector<std::string>::const_iterator citer = splitInfo.begin() + 8;
			citer != splitInfo.end();
			++citer) objMessage += " " + *citer;

		sptr<OWObject> obj(new OWObject);
		obj->initialize(
			levelArea,
			levelSubArea,
			getPosition(levelCoords),
			levelCoords,
			objAttributes,
			objMessage);

		m_objList.push_back(obj);
	}
}

void Level::update(const float drot)
{
	m_lakeFrameCounter = (m_lakeFrameCounter + 1) % LAKE_MAX_FRAMES_PER_CURRENT;
	if (!m_lakeFrameCounter) m_lakeCurrentLeft = !m_lakeCurrentLeft;

	for (lo_list_iter iter = m_lakePieces.begin();
		iter != m_lakePieces.end();
		++iter)
	{
		if(m_lakeCurrentLeft) iter->setX(iter->getPos().x + LAKE_CURRENT_VEL);
		else iter->setX(iter->getPos().x - LAKE_CURRENT_VEL);
	}

	for (npc_list::iterator iter = m_npcList.begin();
		iter != m_npcList.end();
		++iter)
	{
		(*iter)->update(drot, m_pPlayer->modCameraPointer()->getOrientation());
	}
}

void Level::render()
{
	for (so_list_iter iter = m_objects.begin();
		iter != m_objects.end();
		++iter)
	{
		Renderer::get().renderModel(&*iter);
	}
	
	for (lo_list_iter iter = m_lakePieces.begin();
		iter != m_lakePieces.end();
		++iter)
	{
		Renderer::get().renderModel(&*iter);
	}

	for (npc_list::iterator iter = m_npcList.begin();
		iter != m_npcList.end();
		++iter)
	{
		(*iter)->render();
	}
}

bool Level::validCoords(const uint2& xy) const { return validCoords(xy.x, xy.y); }
bool Level::validCoords(const uint x, const uint y) const
{
	for (npc_list::const_iterator citer = m_npcList.begin();
		 citer != m_npcList.end();
		 ++citer)
	{
		uint2 npcCoords = (*citer)->getLevelCoords();
		if (npcCoords.x == x && npcCoords.y == y) return false;

		uint2 npcTargetCoords = (*citer)->getTargetCoords();
		if (npcTargetCoords.x == x && npcTargetCoords.y == y) return false;
	}

	for (obj_list::const_iterator citer = m_objList.begin();
	 	 citer != m_objList.end();
		 ++citer)
	{
		if (!(*citer)->isSolid()) continue;
		uint2 objCoords = (*citer)->getLevelCoords();
		if (objCoords.x == x && objCoords.y == y) return false;
	}

	uint2 plrCoords = m_pPlayer->getLevelCoords();
	if (plrCoords.x == x && plrCoords.y == y) return false;

	return x >= 0 &&
		   y >= 0 &&
		   x < m_levelDims.x &&
		   y < m_levelDims.y && 
		   getTileType(x, y);
}

uint2 Level::getCoords(const vec3f& inPos) const
{
	return uint2{(uint) (-inPos.x / TILE_SIZE), (uint) (inPos.z / TILE_SIZE)};
}

vec3f Level::getPosition(const uint2& inCoords) const
{
	return vec3f{inCoords.x * TILE_SIZE, 0.0f, inCoords.y * TILE_SIZE};
}

uint Level::getTileType(const uint x, const uint y) const { return getTileType(uint2{x, y});}
uint Level::getTileType(const uint2& inCoords) const
{
	assert(inCoords.x >= 0);
	assert(inCoords.y >= 0);
	assert(inCoords.x < m_levelDims.x);
	assert(inCoords.y < m_levelDims.y);
	return m_levelMap[inCoords.y][inCoords.x];
}

uint Level::getTileType(const vec3f& inPos) const
{
	return getTileType(getCoords(inPos));
}

Npc* Level::getNpcAt(const uint2& inCoords)
{
	for (npc_list::const_iterator citer = m_npcList.begin();
		citer != m_npcList.end();
		++citer)
	{
		uint2 npcCoords = (*citer)->getLevelCoords();
		if (npcCoords == inCoords && !(*citer)->isMoving()) return citer->get();
	}
	return nullptr;
}

OWObject* Level::getObjectAt(const uint2& inCoords)
{
	for (obj_list::const_iterator citer = m_objList.begin();
		 citer != m_objList.end();
		 ++citer)
	{
		uint2 objCoords = (*citer)->getLevelCoords();
		if (objCoords == inCoords) return citer->get();
	}

	return nullptr;
}

const Level::level_dims& Level::getLevelDims() const {return m_levelDims; }

