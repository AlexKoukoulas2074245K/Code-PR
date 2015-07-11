#include "level.h"
#include "clock.h"
#include <fstream>

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
		if(mLakeCurrentLeft) iter->pos.x += LAKE_CURRENT_VEL;
		else iter->pos.x -= LAKE_CURRENT_VEL;
	}
}

void Level::Render()
{
	for (so_list_iter iter = mObjects.begin();
		iter != mObjects.end();
		++iter)
	{
		mRenderer->RenderBody(Renderer::ShaderType::DEFAULT, iter->pos, iter->rot, iter->body);
	}
	
	for (lo_list_iter iter = mLakePieces.begin();
		iter != mLakePieces.end();
		++iter)
	{
		mRenderer->RenderBody(Renderer::ShaderType::DEFAULT, iter->pos, iter->rot, iter->body);
	}
}

void Level::GetCoords(const level_pos& inPos, level_coords& outCoords) const
{
	outCoords = {(uint) (-inPos.x / TILE_SIZE), (uint) (inPos.z / TILE_SIZE)};
}

void Level::GetPosition(const level_coords& inCoords, level_pos& outPosition) const
{
	outPosition.x = inCoords.x * TILE_SIZE;
	outPosition.y = inCoords.y * TILE_SIZE;
}

uint Level::GetTileType(const level_coords& inCoords) const
{
	assert(inCoords.x >= 0);
	assert(inCoords.y >= 0);
	assert(inCoords.x < mLevelDims.x);
	assert(inCoords.y < mLevelDims.y);
	return mLevelMap[inCoords.y][inCoords.x];
}

uint Level::GetTileType(const level_pos& inPos) const
{
	level_coords coords = {};
	GetCoords(inPos, coords);
	return GetTileType(coords);
}

