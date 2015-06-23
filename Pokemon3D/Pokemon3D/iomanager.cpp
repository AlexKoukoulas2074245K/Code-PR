#include "iomanager.h"
#include "common.h"
#include "ioutils.h"
#include "window.h"
#include <fstream>
#include <vector>

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
	std_pos posCounter = {};

	for (; std::getline(f, line);)
	{
		std::vector<str> filenames = split(line, LEVEL_COMP_SEP);
		for (std::vector<str>::iterator iter = filenames.begin();
			iter != filenames.end();
			++iter)
		{
			/* Path comprehension */
			str objectFilename = split(*iter, FILE_PATH_SEP).back();
			str objectName = split(objectFilename, FILE_EXT_SEP).front();
			str objectLastName = split(objectName, FILE_NAME_SEP).back();

			Body b;
			str bTexPath;
			GetPathOf(objectName, Format::PNG, bTexPath);
			b.setSingleTexture(bTexPath);
			ForceGetBody(objectName, b);
			posCounter.y = objectLastName.compare("floor") && objectLastName.compare("wild") ? 0.0f : -0.8f;
			std_body res{b, posCounter};
			outList.push_back(res);

			posCounter.x -= tileSize;

		}
		posCounter.x = 0.0f;
		posCounter.z += tileSize;
	}
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


