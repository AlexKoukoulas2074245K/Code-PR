#include "iomanager.h"
#include "common.h"
#include "ioutils.h"

IOManager::IOManager(){}
IOManager::~IOManager(){}

/* Attempt to assign to the out parameter the entry value for the given id 
from the preloaded bodies */
void IOManager::GetBody(const str& id, Body& outBody)
{
	if (mPreLoaded.count(id))
	{
		outBody = mPreLoaded[id];
		return;
	}
	outBody.setLoaded(false);
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
	if (mPreLoaded.count(path)) return;

	Body resultBody;
	if (LoadOBJFromFile(path.c_str(), resultBody))
	{
		mPreLoaded[path] = resultBody;
	}
}
 
/* Loads a variable number of obj files from the paths supplied */
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

/* Loads all obj files from the directory given */
void IOManager::LoadMultipleBodies(const str& directory)
{
	str_list paths;
	GetAllFilenames(directory, paths);
	for (str_list_iter iter = paths.begin();
		iter != paths.end();
		++iter) LoadBody(*iter);
}


