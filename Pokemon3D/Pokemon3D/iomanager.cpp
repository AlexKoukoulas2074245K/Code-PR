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
#define LEVEL_HOUSE_PADDING 0.6f

#define LEVEL_FLOOR_NAME "floor"
#define LEVEL_WILD_NAME "wild"
#define LEVEL_SHORT_NAME "short"
#define LEVEL_DESC_SWITCH "#Desc#"
#define LEVEL_COMP_SWITCH "#Comps#"
#define LEVEL_HOUSE_SWITCH "#Houses#"
#define LEVEL_REP_SWITCH "#LevelMap#"
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

	m_formatPaths[Format::OBJ] = "C:/Users/alex/Pictures/projects/pkmnrevo/models/";
	m_formatPaths[Format::PNG] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/materials/";
	m_formatPaths[Format::HUD] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/hud/";
	m_formatPaths[Format::FCF] = "C:/Users/alex/Pictures/projects/pkmnrevo/config/";
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

/* Attempt to assign to the out parameter the entry value for the given id 
from the preloaded bodies */
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
 
/* Forces a GetBody by loading first */
void IOManager::forceGetBody(const str& id, Body& outBody)
{
	str matPath;
	getPathOf(id, Format::PNG, matPath);
	outBody.setSingleTexture(matPath);
	loadBody(id, outBody.modTexturesToLoad().front());
	getBody(id, outBody);
}

/* Loads a body in the preloaded mmap */
void IOManager::loadBody(const str& id, const str& mat)
{
	str path;
	getPathOf(id, Format::OBJ, path);

	if (m_prelBodies.count(path)) return;
	Body resultBody;
	resultBody.setSingleTexture(mat);
	if (LoadOBJFromFile(path, &resultBody))
	{
		Renderer::get().prepareObject(Renderer::ShaderType::DEFAULT, &resultBody);
		m_prelBodies[path] = resultBody;
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

/* Loads all bodies specified in a lvl file */
/* Comments: The moving lake pieces create a problem in the level creation.. 
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
	std::ifstream f;
	f.open(lvlFilename.c_str());

	str line;

	/* Level Description parsing */
	std::getline(f, line);
	assert(!line.compare(LEVEL_DESC_SWITCH));

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

	/* Level Component parsing */
	std::getline(f, line);
	assert(!line.compare(LEVEL_COMP_SWITCH));

	bool houseSwitch = false;
	bool levelRepSwitch = false;
	
	for (; std::getline(f, line);)
	{
		/* End of game object description, start of house description */
		if (!line.compare(LEVEL_HOUSE_SWITCH)) { houseSwitch = true; continue; }
		else if (!line.compare(LEVEL_REP_SWITCH)) { levelRepSwitch = true; houseSwitch = false; continue; }

		if (levelRepSwitch)
		{
			std::vector<str> repRow = split(line, ' ');
			for (size_t x = 0; x < outDims.x; ++x)
			{
				ppoutMap[rowCounter][x] = static_cast<uint>(std::stoi(repRow[x]));
			}
			++rowCounter;
		}

		/* House insertion */
		else if (houseSwitch)
		{
			std::vector<str> houseComps = split(line, LEVEL_COMP_SEP);
			std::vector<str> houseLoc = split(houseComps[1], VALUE_SEP);
			str houseName = houseComps[0];

			Body b;
			forceGetBody(houseName, b);
			float houseX = std::stof(houseLoc[0]);
			float houseZ = std::stof(houseLoc[1]);
			float3 rot = {};
			StaticModel res(b, {houseX + 1.0f, tileSize / 2, houseZ}, rot);
			outList.push_back(res);
			continue;
		}
		/* Component insertion */
		else
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
				Body b; str ori, cornerName;


				/* Identify if the object matches a core piece with jsut different 
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
				
				/* Object is a special case so apply the changes described above */
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
					else objPos.y = 0.0f;
					float3 rot = {};

					/* Here we configure the rotation struct passed in the final result according to the initial
					object's orientation info */
					if (!ori.compare(LEVEL_ORI_RIGHT) || !ori.compare(LEVEL_ORI_BOTTOM)) rot.y = static_cast<float>(D3DX_PI);
					else if (!ori.compare(LEVEL_ORI_TOPRIGHT)) rot.y = static_cast<float>(D3DX_PI / 2);
					else if (!ori.compare(LEVEL_ORI_BOTRIGHT)) rot.y = static_cast<float>(D3DX_PI);
					else if (!ori.compare(LEVEL_ORI_BOTLEFT)) rot.y = static_cast<float>(D3DX_PI * 1.5f);

					StaticModel res(b, objPos, rot);
					outList.push_back(res);
				}
				/* No special case, load the object normally */
				else
				{
					forceGetBody(objectName, b);
					if (objectLastName.compare(LEVEL_FLOOR_NAME) == 0 ||
						objectLastName.compare(LEVEL_WILD_NAME) == 0) objPos.y = -tileSize / 2;
					else if (objectLastName.compare(LEVEL_SHORT_NAME) == 0) objPos.y = LEVEL_SHORT_HEIGHT;
					else objPos.y = 0.0f;
					float3 rot = {};
					StaticModel res(b, objPos, rot);
					outList.push_back(res);
				}
				
				/* Check for lake pieces */
				if (nameComps.size() >= 3 && !nameComps[1].compare(LEVEL_LAKE_PIECE_NAME))
				{
					Body lakePiece;
					forceGetBody(LEVEL_LAKE_PIECE_FULL_NAME, lakePiece);
					objPos.y = LEVEL_CLOSER;
					float3 rot = {};
					StaticModel lakeRes(lakePiece, objPos, rot);
					outLakeList.push_back(lakeRes);

					/* Check for side edges of lake pieces (if so to avoid graphical glitches
					an empty floor piece needs to be inserted in between the surface of 
					the lake pieces and the component next to the lake side pieces) */
					bool isLakeLeft = !objectName.compare(LEVEL_LEFT_LAKE_PIECE_FULL_NAME);
					bool isLakeRight = !objectName.compare(LEVEL_RIGHT_LAKE_PIECE_FULL_NAME);
					if (isLakeLeft || isLakeRight)
					{
						Body b; 
						forceGetBody(LEVEL_FILL_OBJECT_FULL_NAME, b);
						float3 pos;
						pos.x = isLakeLeft ? objPos.x + tileSize : objPos.x - tileSize;
						pos.y = LEVEL_CLOSEST;
						pos.z = objPos.z;
						float3 rot = {};
						StaticModel res(b, pos, rot);
						outList.push_back(res);
					}
				}
			}
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


