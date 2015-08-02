#include "level.h"

#include <fstream>

#include "clock.h"
#include "renderer.h"

const float Level::TILE_SIZE = 1.6f;
const float Level::LAKE_CURRENT_VEL = 0.001f;
const float Level::LAKE_CURRENT_MAX_DISTANCE = 0.4f;
const unsigned int Level::LAKE_MAX_FRAMES_PER_CURRENT = 
	static_cast<int>(Level::LAKE_CURRENT_MAX_DISTANCE / Level::LAKE_CURRENT_VEL);

Level::Level(){}
Level::~Level()
{
	for (uint i = 0; i < mLevelDims.y; ++i) delete mLevelMap[i];
	delete[] mLevelMap;
}


bool Level::Initialize(
	const sptr<Renderer> renderer,
	const sptr<IOManager> iomanager)
{
	mLakeFrameCounter = 0;
	mLakeCurrentLeft = true;

	mRenderer = renderer;
	mObjects.clear();
	mLakePieces.clear();

	Clock c;
	if (!c.InitClock()) return false;
	iomanager->GetAllBodiesFromLevel(
		"C:/Users/alex/Pictures/projects/pkmnrevo/levels/pallet.lvl",
		TILE_SIZE,
		mLevelDims,
		mObjects,
		mLakePieces,
		mLevelMap);

	LOGLN(("Level Initialization Completed in: " + std::to_string(c.GetCounter()) + " ms.").c_str());
	return true;
}

void Level::Update()
{
	mLakeFrameCounter = (mLakeFrameCounter + 1) % LAKE_MAX_FRAMES_PER_CURRENT;
	if (!mLakeFrameCounter) mLakeCurrentLeft = !mLakeCurrentLeft;

	for (lo_list_iter iter = mLakePieces.begin();
		iter != mLakePieces.end();
		++iter)
	{
		if(mLakeCurrentLeft) iter->setX(iter->getPos().x + LAKE_CURRENT_VEL);
		else iter->setX(iter->getPos().x - LAKE_CURRENT_VEL);
	}
}

void Level::Render()
{
	for (so_list_iter iter = mObjects.begin();
		iter != mObjects.end();
		++iter)
	{
		mRenderer->RenderModel(&*iter);
	}
	
	for (lo_list_iter iter = mLakePieces.begin();
		iter != mLakePieces.end();
		++iter)
	{
		mRenderer->RenderModel(&*iter);
	}
}

bool Level::ValidCoords(const uint x, const uint y) const
{
	return x >= 0 &&
		   y >= 0 &&
		   x < mLevelDims.x &&
		   y < mLevelDims.y && 
		   GetTileType(x, y);
}

uint2 Level::GetCoords(const vec3f& inPos) const
{
	return uint2{(uint) (-inPos.x / TILE_SIZE), (uint) (inPos.z / TILE_SIZE)};
}

vec3f Level::GetPosition(const uint2& inCoords, const float y) const
{
	return vec3f{inCoords.x * TILE_SIZE, y, inCoords.y * TILE_SIZE};
}

uint Level::GetTileType(const uint x, const uint y) const { return GetTileType(uint2{x, y});}
uint Level::GetTileType(const uint2& inCoords) const
{
	assert(inCoords.x >= 0);
	assert(inCoords.y >= 0);
	assert(inCoords.x < mLevelDims.x);
	assert(inCoords.y < mLevelDims.y);
	return mLevelMap[inCoords.y][inCoords.x];
}

uint Level::GetTileType(const vec3f& inPos) const
{
	return GetTileType(GetCoords(inPos));
}

