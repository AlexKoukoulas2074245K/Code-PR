#include "iomanager.h"

#include <fstream>
#include <vector>

#include "common.h"
#include "ioutils.h"
#include "renderer.h"

#define LEVEL_IGNORE_DIRECTIVE "IGNORE"
#define LEVEL_FURTHER - tileSize/2 - 0.05f
#define LEVEL_CLOSER  - tileSize/2 - 0.005f
#define LEVEL_CLOSEST  - tileSize/2 - 0.0005f
#define LEVEL_SHORT_HEIGHT -0.6f
#define LEVEL_SHORTMEDIUM_HEIGHT -0.2f
#define LEVEL_HOUSE_PADDING 0.6f

#define LEVEL_FLOOR_NAME "floor"
#define LEVEL_WILD_NAME "wild"
#define LEVEL_SHORT_NAME "short"
#define LEVEL_SHORTMEDIUM_NAME "shortmedium"
#define LEVEL_FILL_OBJECT_FULL_NAME "2d_out_empty_floor"
#define LEVEL_LAKE_PIECE_FULL_NAME "2d_out_lake_floor"
#define LEVEL_LEFT_LAKE_PIECE_FULL_NAME "out_lake_left_short"
#define LEVEL_RIGHT_LAKE_PIECE_FULL_NAME "out_lake_right_short"
#define LEVEL_LAKE_PIECE_NAME "lake"
#define LEVEL_CORNER_NAME "corner"

#define LEVEL_ORI_LEFT "left"
#define LEVEL_ORI_RIGHT "right"
#define LEVEL_ORI_TOP "top"
#define LEVEL_ORI_BOTTOM "bottom"
#define LEVEL_ORI_TOPLEFT "topleft"
#define LEVEL_ORI_TOPRIGHT "topright"
#define LEVEL_ORI_BOTLEFT "bottomleft"
#define LEVEL_ORI_BOTRIGHT "bottomright"

IOManager& IOManager::get()
{
	static IOManager iom;
	return iom;
}

IOManager::IOManager()
{
	m_suppFormats[Format::OBJ] = ".obj"; 
	m_suppFormats[Format::PNG] = ".png";
	m_suppFormats[Format::HUD] = ".png";	
	m_suppFormats[Format::FCF] = ".fcf";
	m_suppFormats[Format::DAT] = ".dat";
	m_suppFormats[Format::LVL] = ".lvl";
	m_suppFormats[Format::VER] = ".hlsl";
	m_suppFormats[Format::PIX] = ".hlsl";

	m_formatPaths[Format::OBJ] = "C:/Users/alex/Pictures/projects/pkmnrevo/models/";
	m_formatPaths[Format::PNG] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/materials/";
	m_formatPaths[Format::HUD] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/hud/";
	m_formatPaths[Format::FCF] = "C:/Users/alex/Pictures/projects/pkmnrevo/gamedata/";
	m_formatPaths[Format::DAT] = "C:/Users/alex/Pictures/projects/pkmnrevo/gamedata/";
	m_formatPaths[Format::LVL] = "C:/Users/alex/Pictures/projects/pkmnrevo/levels/";
	m_formatPaths[Format::VER] = "C:/Users/alex/Pictures/projects/pkmnrevo/shaders/";
	m_formatPaths[Format::PIX] = "C:/Users/alex/Pictures/projects/pkmnrevo/shaders/";
	
	m_nBodiesLoaded = 0U;
	m_nBodiesRejected = 0U;
}

bool IOManager::getFileContent(const str& path, str_list* outList)
{
	std::ifstream file;
	file.open(path.c_str());

	if (!file.is_open() || outList == nullptr) return false;

	str line;
	for (;std::getline(file, line);) outList->push_back(line);
	return true;
}


void IOManager::getBody(const str& id, Body& outBody)
{
	str path;
	getPathOf(id, Format::OBJ, path);
	if (m_prelBodies.count(path))
	{
		outBody = m_prelBodies[path];
		return;
	}
	LOGLN(("Body does not exist: " + id).c_str());
}
 
void IOManager::forceGetBody(const str& id, Body& outBody, str matName /* "" */)
{
	str matPath;

	if (!matName.compare("")) getPathOf(id, Format::PNG, matPath);
	else  getPathOf(matName, Format::PNG, matPath);

	outBody.setSingleTexture(matPath);
	loadBody(id, outBody.modTexturesToLoad().front());
	getBody(id, outBody);
}

void IOManager::loadBody(const str& id, const str& mat)
{
	str path;
	getPathOf(id, Format::OBJ, path);

	if (m_prelBodies.count(path))
	{
		++m_nBodiesRejected;
		return;
	}

	Body resultBody;
	resultBody.setSingleTexture(mat);
	if (LoadOBJFromFile(path, &resultBody))
	{
		Renderer::get().prepareObject(Renderer::ShaderType::DEFAULT, &resultBody);
		m_prelBodies[path] = resultBody;
		++m_nBodiesLoaded;
	}
}
 
/* Loads all obj files from the directory given */
void IOManager::loadMultipleBodies(const str& directory)
{
	str_list paths;
	getAllFilenames(directory, paths);
	for (str_list_iter iter = paths.begin();
		iter != paths.end();
		++iter) loadBody(*iter, std::string());
}


/* Comment/rant : The moving lake pieces create a problem in the level creation.. 
   Up until now the normal components (columns, tiles, houses) of the level were positioned on top 
   of everything else,followed by a white layer covering all of the transparent areas 
   below some set pieces accross the whole level. Now given the lake in pallet town for example: 
   
   111111
   100001
   100001
   Where 1's are the lake boundaries and 0's are the central lake pieces.

   Naturally the lake pieces would cover only the portion filled with zeroes. But with that configuration
   any movement of the lake pieces to the left or right would make the transparency of the opposite side
   visible so clearly the lake pieces need to expand beyond this area.

   The approach I'm currently following is to include lake pieces under the boundaries of the lake as well (under the 1s)
   so that the movement will not reveal any transparency but there is also an issue with this approach.
   Naturally the lake pieces should positioned in between the normal components of the level(lake boundaries, tiles, columns)
   and the white layer (the only logical way; water sitting above everything is not the best idea). This way though
   any movement to the left or right will reveal a portion of the lake piece's texture outside that boundary
   of the lake. So in order to make everything look normal I had to add another white layer below all the components
   next to a lake side piece.

   So in conclusion for all the normal components NOT near the sides of a lake the layers look like the following:
   Component -> WhiteLayer

   For a lake side piece:
   Component -> LakePiece -> WhiteLayer

   For the central lake pieces(not under the lake side pieces):
   LakePiece -> WhiteLayer

   And for the components next to the lake side pieces (to the left of a lake left side piece and
   to the right of a lake right side piece):
   Component -> SingleWhiteLayer -> LakePiece -> WhiteLayer

   Another approach would be to make the lake pieces be smaller than the standard tile size but
   I think that could end up being more complex.
   */
void IOManager::getAllBodiesFromLevel(
	const str& lvlFilename,
	const float tileSize,
	uint3& outDims,
	std::list<StaticModel>& outList,
	std::list<StaticModel>& outLakeList,
	unsigned int**& ppoutMap)
{
	enum LoadingState { COMPONENT, HOUSES, TILEMAP };
	std::ifstream f;
	str fullPath = m_formatPaths[LVL] + lvlFilename;
	f.open(fullPath.c_str());
	
	str line;

	/* Level Description parsing */
	std::getline(f, line);
	assert(!line.compare("#Desc#"));

	/* Level rows and cols extraction */
	std::getline(f, line);
	std::vector<str> lvlRowsCols = split(line, VALUE_SEP);
	outDims.x = static_cast<uint>(std::stoi(lvlRowsCols[0]));
	outDims.y = static_cast<uint>(std::stoi(lvlRowsCols[1]));
	ppoutMap = new uint*[outDims.y];
	for (size_t i = 0; i < outDims.y; ++i) ppoutMap[i] = new uint[outDims.x];
	size_t rowCounter = 0;

	/* World space level dimensions */;
	std::getline(f, line);
	std::vector<str> lvlDesc = split(line, VALUE_SEP);
	float maxWidth = std::stof(lvlDesc[0]);
	float maxDepth = std::stof(lvlDesc[1]);

	LoadingState loadingState;

	for (; std::getline(f, line);)
	{
		/* Loading State switches */
		if (!line.compare("#Comps#")) { loadingState = COMPONENT; continue; }
		else if (!line.compare("#Houses#")) { loadingState = HOUSES; continue; }
		else if (!line.compare("#LevelMap#")) { loadingState = TILEMAP; continue; }

		if (loadingState == COMPONENT)
		{
			std::vector<str> filenames = split(line, LEVEL_COMP_SEP);
			for (std::vector<str>::iterator iter = filenames.begin();
				iter != filenames.end();
				++iter)
			{
				/* Building or house map areas which will be covered seperately */
				if (!iter->compare(LEVEL_IGNORE_DIRECTIVE))	continue;

				/* Component path comprehension */
				str objectName = split(*iter, NAME_POS_SEP).front();
				str objectDetails = split(*iter, NAME_POS_SEP).back();
				str objectLastName = split(objectName, FILE_NAME_SEP).back();
				std::vector<str> nameComps = split(objectName, FILE_NAME_SEP);

				float3 objPos;
				objPos.x = std::stof(split(objectDetails, VALUE_SEP)[0]);
				objPos.z = std::stof(split(objectDetails, VALUE_SEP)[1]);

				/* Next level game object creation */
				Body b; 
				str ori, cornerName;
				float3 rot = {};


				/* Identify if the object matches a core piece with just different
				orientation. In that case we need to change the name of the object
				to the core object's name, and based on the first object's name
				extract the orientation and apply the corresponding transformation to
				the core object */
				bool specialOrientation = false;
				if (nameComps.size() > 3)
				{
					ori = nameComps[2];
					cornerName = nameComps[3];

					specialOrientation = !ori.compare(LEVEL_ORI_RIGHT) ||
						!ori.compare(LEVEL_ORI_BOTTOM) ||
						!cornerName.compare(LEVEL_CORNER_NAME);
				}

				if (specialOrientation)
				{

					/* Create a new name for the object which is
					the same as the first name but with changed orientation
					(the orientation information is in third place from the start
					of the objects name (a_b_c.ext  orientation info is c)) */
					str newObjectName;
					for (std::vector<str>::size_type i = 0;
						i < nameComps.size();
						++i)
					{
						if (i != 2)
						{
							newObjectName += nameComps[i];
							newObjectName += FILE_NAME_SEP;
							continue;
						}

						if (!ori.compare(LEVEL_ORI_RIGHT)) newObjectName += LEVEL_ORI_LEFT;
						else if (!ori.compare(LEVEL_ORI_BOTTOM)) newObjectName += LEVEL_ORI_TOP;
						else newObjectName += LEVEL_ORI_TOPLEFT;
						newObjectName += FILE_NAME_SEP;
					}

					/* Erase the last underscore created by the last name component */
					newObjectName.erase(newObjectName.end() - 1);

					forceGetBody(newObjectName, b);
					if (objectLastName.compare(LEVEL_FLOOR_NAME) == 0 ||
						objectLastName.compare(LEVEL_WILD_NAME) == 0) objPos.y = -tileSize / 2;
					else if (objectLastName.compare(LEVEL_SHORT_NAME) == 0) objPos.y = LEVEL_SHORT_HEIGHT;
					else if (objectLastName.compare(LEVEL_SHORTMEDIUM_NAME) == 0) objPos.y = LEVEL_SHORTMEDIUM_HEIGHT;
					else objPos.y = 0.0f;
					
					/* Here we configure the rotation struct passed in the final result according to the initial
					object's orientation info */
					if (!ori.compare(LEVEL_ORI_RIGHT) || !ori.compare(LEVEL_ORI_BOTTOM)) rot.y = static_cast<float>(D3DX_PI);
					else if (!ori.compare(LEVEL_ORI_TOPRIGHT)) rot.y = static_cast<float>(D3DX_PI / 2);
					else if (!ori.compare(LEVEL_ORI_BOTRIGHT)) rot.y = static_cast<float>(D3DX_PI);
					else if (!ori.compare(LEVEL_ORI_BOTLEFT)) rot.y = static_cast<float>(D3DX_PI * 1.5f);
					outList.push_back(StaticModel(b, objPos, rot));
				}
				/* No special case, load the object normally */
				else
				{
					forceGetBody(objectName, b);
					if (objectLastName.compare(LEVEL_FLOOR_NAME) == 0 ||
						objectLastName.compare(LEVEL_WILD_NAME) == 0) objPos.y = -tileSize / 2;
					else if (objectLastName.compare(LEVEL_SHORT_NAME) == 0) objPos.y = LEVEL_SHORT_HEIGHT;
					else if (objectLastName.compare(LEVEL_SHORTMEDIUM_NAME) == 0) objPos.y = LEVEL_SHORTMEDIUM_HEIGHT;
					else objPos.y = 0.0f;

					outList.push_back(StaticModel(b, objPos, rot));
				}

				/* Check for lake pieces */
				if (nameComps.size() >= 3 && !nameComps[1].compare(LEVEL_LAKE_PIECE_NAME))
				{
					forceGetBody(LEVEL_LAKE_PIECE_FULL_NAME, b);
					objPos.y = LEVEL_CLOSER;
					outLakeList.push_back(StaticModel(b, objPos, rot));

					/* Check for side edges of lake pieces (if so to avoid graphical glitches
					an empty floor piece needs to be inserted in between the surface of
					the lake pieces and the component next to the lake side pieces) */
					bool isLakeLeft = !objectName.compare(LEVEL_LEFT_LAKE_PIECE_FULL_NAME);
					bool isLakeRight = !objectName.compare(LEVEL_RIGHT_LAKE_PIECE_FULL_NAME);
					if (isLakeLeft || isLakeRight)
					{
						forceGetBody(LEVEL_FILL_OBJECT_FULL_NAME, b);
						float3 pos;
						pos.x = isLakeLeft ? objPos.x + tileSize : objPos.x - tileSize;
						pos.y = LEVEL_CLOSEST;
						pos.z = objPos.z;
						outList.push_back(StaticModel(b, pos, rot));
					}
				}
			}
		}
		else if (loadingState == TILEMAP)
		{
			std::vector<str> repRow = split(line, ' ');
			for (size_t x = 0; x < outDims.x; ++x)
			{
				ppoutMap[rowCounter][x] = static_cast<uint>(std::stoi(repRow[x]));
			}
			++rowCounter;
		}

		/* House insertion */
		else if (loadingState == HOUSES)
		{
			std::vector<str> houseComps = split(line, LEVEL_COMP_SEP);
			std::vector<str> houseLoc = split(houseComps[1], VALUE_SEP);
			str houseName = houseComps[0];

			Body b;
			forceGetBody(houseName, b);
			float houseX = std::stof(houseLoc[0]);
			float houseZ = std::stof(houseLoc[1]);
			StaticModel res(b, {houseX + 1.0f, tileSize / 2, houseZ}, float3{});
			outList.push_back(res);
			continue;
		}		
	}
	
	/* Level filler piece creation (for empty or transparent areas) */
	Body uniFloor;
	forceGetBody(LEVEL_FILL_OBJECT_FULL_NAME, uniFloor);
	uniFloor.setDimensions({maxWidth, 1.0f, maxDepth});
	float3 uniFloorPos{
		-maxWidth / 2 + tileSize / 2,
		 LEVEL_FURTHER,
		 maxDepth / 2 - tileSize / 2};
	float3 rot = {};
	StaticModel res(uniFloor, uniFloorPos, rot);
	outList.push_back(res);
}

/* Returns the number of filenames from the directory supplied */
void IOManager::getAllFilenames(const str& directory, str_list& outFilenames)
{
	str dirWithoutPostfix(directory.begin(), directory.end() - 1);
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(directory.c_str(), &findData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		outFilenames.push_back(dirWithoutPostfix + findData.cFileName);
		if (!FindNextFile(hFind, &findData))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}
}

const IOManager::str& IOManager::getFormatPath(const Format& format) const
{
	return m_formatPaths.at(format);
}

/* Tests wether the path given ends with the given suffix */
bool IOManager::validPath(const str& path, const Format frmt)
{
	str format = m_suppFormats[frmt];
	str_sizet pathSize = path.size();
	if (pathSize <= 3 ||
		path[pathSize - 3] != format[1] ||
		path[pathSize - 2] != format[2] ||
		path[pathSize - 1] != format[3]) return false;
	return true;
}

void IOManager::getPathOf(const str& id, const Format frmt, str& outStr)
{
	if (!validPath(id, frmt)) outStr = m_formatPaths[frmt] + id + m_suppFormats[frmt];
	else outStr = id;
}

uint IOManager::getNBodiesRejected() const { return m_nBodiesRejected; }
uint IOManager::getNBodiesLoaded() const { return m_nBodiesLoaded; }

