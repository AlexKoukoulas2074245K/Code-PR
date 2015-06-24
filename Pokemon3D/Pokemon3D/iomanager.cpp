#include "iomanager.h"
#include "common.h"
#include "ioutils.h"
#include "window.h"
#include <fstream>
#include <vector>

#define LEVEL_IGNORE_DIRECTIVE "IGNORE"
#define LEVEL_FILL_OBJECT_NAME "2d_out_empty_floor"
#define LEVEL_SMALL_PADDING 0.001f
#define LEVEL_FLOOR_NAME "floor"
#define LEVEL_WILD_NAME "wild"
#define LEVEL_DESC_SWITCH "#Desc#"
#define LEVEL_COMP_SWITCH "#Comps#"
#define LEVEL_HOUSE_SWITCH "#Houses#"

IOManager::IOManager()
{
	mSuppFormats[Format::BMP] = ".bmp";
	mSuppFormats[Format::OBJ] = ".obj"; 
	mSuppFormats[Format::PNG] = ".png";

	mFormatPaths[Format::OBJ] = "C:/Users/alex/Pictures/projects/pkmnrevo/models/";
	mFormatPaths[Format::PNG] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/materials/";
	mFormatPaths[Format::BMP] = "C:/Users/alex/Pictures/projects/pkmnrevo/textures/hud/";
}
IOManager::~IOManager(){}

/* Attempt to assign to the out parameter the entry value for the given id 
from the preloaded bodies */
void IOManager::GetBody(const str& id, Body& outBody)
{
	str path;
	GetPathOf(id, Format::OBJ, path);
	if (mPrelBodies.count(path))
	{
		outBody = mPrelBodies[path];
		return;
	}
	LOGLN(("Body does not exist: " + id).c_str());
}
 
/* Forces a GetBody by loading first */
void IOManager::ForceGetBody(const str& id, Body& outBody)
{
	LoadBody(id, outBody.modTexturesToLoad().front());
	GetBody(id, outBody);
}

/* Loads a body in the preloaded mmap */
void IOManager::LoadBody(const str& id, const str& mat)
{
	str path;
	GetPathOf(id, Format::OBJ, path);

	if (mPrelBodies.count(path)) return;
	Body resultBody;
	resultBody.setSingleTexture(mat);
	if (LoadOBJFromFile(path, resultBody))
	{
		mRenderer->PrepareBody(resultBody, Renderer::ShaderType::DEFAULT);
		mPrelBodies[path] = resultBody;
	}
}
 
/* Loads all obj files from the directory given */
void IOManager::LoadMultipleBodies(const str& directory)
{
	str_list paths;
	GetAllFilenames(directory, paths);
	for (str_list_iter iter = paths.begin();
		iter != paths.end();
		++iter) LoadBody(*iter, std::string());
}

/* Loads all bodies specified in a lvl file */
void IOManager::GetAllBodiesFromLevel(
	const str& lvlFilename,
	const float tileSize,
	std::list<std_body>&  outList)
{
	std::ifstream f;
	f.open(lvlFilename.c_str());
	
	str line;

	/* Level Description parsing */
	std::getline(f, line);
	assert(!line.compare(LEVEL_DESC_SWITCH));
	std::getline(f, line);
	std::vector<str> lvlDesc = split(line, VALUE_SEP);
	float maxWidth = std::stof(lvlDesc[0]);
	float maxDepth = std::stof(lvlDesc[1]);

	/* Level Component parsing */
	std::getline(f, line);
	assert(!line.compare(LEVEL_COMP_SWITCH));

	std_pos posCounter = {};
	bool houseSwitch = false;
	for (; std::getline(f, line);)
	{
		/* End of game object description, start of house description */
		if (!line.compare(LEVEL_HOUSE_SWITCH)) { houseSwitch = true; continue; }

		if (houseSwitch)
		{
			std::vector<str> houseComps = split(line, LEVEL_COMP_SEP);
			std::vector<str> houseLoc   = split(houseComps[1], VALUE_SEP);
			str houseName = houseComps[0];
			Body b; str bTex;

			GetPathOf(houseName, Format::PNG, bTex);
			b.setSingleTexture(bTex);
			ForceGetBody(houseName, b);
			b.setCustomDims({b.getDimensions().maxWidth, b.getDimensions().maxHeight, b.getDimensions().maxDepth});
			float houseX = std::stof(houseLoc[0]);
			float houseZ = std::stof(houseLoc[1]);
			std_body res{b, {houseX, tileSize / 2, houseZ}};
			outList.push_back(res);
			continue;
		}
		else
		{
			std::vector<str> filenames = split(line, LEVEL_COMP_SEP);
			for (std::vector<str>::iterator iter = filenames.begin();
				iter != filenames.end();
				++iter)
			{
				/* Building or house map areas which will be covered last */
				if (!iter->compare(LEVEL_IGNORE_DIRECTIVE))
				{
					posCounter.x -= tileSize;
					continue;
				}

				/* Path comprehension */
				str objectFilename = split(*iter, FILE_PATH_SEP).back();
				str objectName = split(objectFilename, FILE_EXT_SEP).front();
				str objectLastName = split(objectName, FILE_NAME_SEP).back();

				/* Next level game object creation */
				Body b; str bTexPath;
				GetPathOf(objectName, Format::PNG, bTexPath);
				b.setSingleTexture(bTexPath);
				ForceGetBody(objectName, b);
				b.setCustomDims({b.getDimensions().maxWidth, b.getDimensions().maxHeight, b.getDimensions().maxDepth});
				posCounter.y = objectLastName.compare(LEVEL_FLOOR_NAME) &&
					objectLastName.compare(LEVEL_WILD_NAME) ?
					0.0f : -tileSize / 2;
				std_body res{b, posCounter};
				outList.push_back(res);

				/* Horizontal position-cursor advance */
				posCounter.x -= tileSize;
			}
			posCounter.x = 0.0f;
			posCounter.z += tileSize;
		}		
	}

	/* Level Filler creation (for empty or transparent areas) */
	Body uniFloor;
	str floorTexPath;
	GetPathOf(LEVEL_FILL_OBJECT_NAME, Format::PNG, floorTexPath);
	uniFloor.setSingleTexture(floorTexPath);
	ForceGetBody(LEVEL_FILL_OBJECT_NAME, uniFloor);
	uniFloor.setCustomDims({maxWidth, 1.0f, maxDepth});
	std_pos uniFloorPos{
		-maxWidth / 2 + tileSize / 2,
		-tileSize / 2 - LEVEL_SMALL_PADDING,
		 maxDepth / 2 - tileSize / 2};
	std_body res{uniFloor, uniFloorPos};
	outList.push_back(res);
}

void IOManager::GetBmp(const str& id, Bitmap& outBmp)
{
	str path;
	GetPathOf(id, Format::BMP, path);
	if (mPrelBitmaps.count(path))
	{
		outBmp = mPrelBitmaps[path];
		outBmp.setLoaded(true);
		return;
	}
	outBmp.setLoaded(false);
	LOGLN(("Bmp does not exist: " + id).c_str());
}

void IOManager::ForceGetBmp(const str& id, Bitmap& outBmp)
{
	LoadBmp(id);
	GetBmp(id, outBmp);
}

void IOManager::LoadBmp(const str& id)
{
	str path;
	GetPathOf(id, Format::BMP, path);
	Bitmap resultBmp;
	if (LoadBMPFromFile(path, resultBmp)) mPrelBitmaps[path] = resultBmp;
}

void IOManager::LoadMultipleBmps(const str& directory)
{
	str_list paths;
	GetAllFilenames(directory, paths);
	for (str_list_iter iter = paths.begin();
		iter != paths.end();
		++iter) LoadBmp(*iter);
}

/* Returns the number of filenames from the directory supplied */
void IOManager::GetAllFilenames(const str& directory, str_list& outFilenames)
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
bool IOManager::ValidPath(const str& path, const Format frmt)
{
	str format = mSuppFormats[frmt];
	str_sizet pathSize = path.size();
	if (pathSize <= 3 ||
		path[pathSize - 3] != format[1] ||
		path[pathSize - 2] != format[2] ||
		path[pathSize - 1] != format[3]) return false;
	return true;
}

void IOManager::GetPathOf(const str& id, const Format frmt, str& outStr)
{
	if (!ValidPath(id, frmt)) outStr = mFormatPaths[frmt] + id + mSuppFormats[frmt];
	else outStr = id;
}


