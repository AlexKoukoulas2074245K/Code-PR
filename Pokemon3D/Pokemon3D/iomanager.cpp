#include "iomanager.h"
#include "common.h"
#include "ioutils.h"
#include "window.h"

IOManager::IOManager()
{
	mSuppFormats[Format::BMP] = "bmp";
	mSuppFormats[Format::OBJ] = "obj"; 
}
IOManager::~IOManager(){}

/* Attempt to assign to the out parameter the entry value for the given id 
from the preloaded bodies */
void IOManager::GetBody(const str& id, Body& outBody)
{
	if (mPrelBodies.count(id))
	{
		outBody = mPrelBodies[id];
		return;
	}
	LOGLN(("Body does not exist: " + id).c_str());
}
 
/* Forces a GetBody by loading first */
void IOManager::ForceGetBody(const str& id, Body& outBody)
{
	LoadBody(id);
	GetBody(id, outBody);
}

/* Loads a body in the preloaded mmap */
void IOManager::LoadBody(const str& path)
{
	if (mPrelBodies.count(path) ||
		!ValidPath(path, Format::OBJ)) return;

	Body resultBody;
	if (LoadOBJFromFile(path.c_str(), resultBody))
	{
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
		++iter) LoadBody(*iter);
}

void IOManager::GetBmp(const str& id, Bitmap& outBmp)
{
	if (mPrelBitmaps.count(id))
	{
		outBmp = mPrelBitmaps[id];
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

void IOManager::LoadBmp(const str& path)
{
	if (mPrelBitmaps.count(path) ||
		!ValidPath(path, Format::BMP)) return;

	Bitmap resultBmp;
	if (LoadBMPFromFile(path.c_str(), resultBmp))
	{
		mPrelBitmaps[path] = resultBmp;
	}
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

bool IOManager::ValidPath(const str& path, const Format frmt)
{
	str format = mSuppFormats[frmt];
	str_sizet pathSize = path.size();
	if (pathSize <= 3 ||
		path[pathSize - 3] != format[0] ||
		path[pathSize - 2] != format[1] ||
		path[pathSize - 1] != format[2])
	{
		LOGLN("Sanity check failed for: " + path + " against: " + format);
		return false;
	}
	return true;
}
