#include "level.h"

#include <fstream>

#include "clock.h"
#include "renderer.h"
#include "iomanager.h"

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


bool Level::initialize()
{
	m_lakeFrameCounter = 0;
	m_lakeCurrentLeft = true;
	m_objects.clear();
	m_lakePieces.clear();

	Clock c;
	if (!c.initClock()) return false;
	IOManager::get().getAllBodiesFromLevel(
		"C:/Users/alex/Pictures/projects/pkmnrevo/levels/pallet.lvl",
		TILE_SIZE,
		m_levelDims,
		m_objects,
		m_lakePieces,
		m_levelMap);

	LOGLN(("Level Initialization Completed in: " + std::to_string(c.getCounter()) + " ms.").c_str());
	return true;
}

void Level::update()
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
}

bool Level::validCoords(const uint x, const uint y) const
{
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

vec3f Level::getPosition(const uint2& inCoords, const float y) const
{
	return vec3f{inCoords.x * TILE_SIZE, y, inCoords.y * TILE_SIZE};
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

const Level::level_dims& Level::getLevelDims() const {return m_levelDims; }

