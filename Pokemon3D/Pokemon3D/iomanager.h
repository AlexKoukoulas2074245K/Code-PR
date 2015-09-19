#pragma once
#include <map>
#include <string>
#include <list>

#include "pokedef.h"
#include "staticmodel.h"

class IOManager
{
public:
	enum Format
	{
		OBJ, PNG, HUD, FCF,
		DAT, LVL, PIX, VER
	};

	typedef std::string str;
	typedef std::list<std::string> str_list;
	typedef str_list::const_iterator str_list_iter;
	typedef str::size_type str_sizet;

public:
	static IOManager& get();
public:
	/* Retrieves the content of the specified file and returns it split by newline */
	bool getFileContent(const str& path, str_list* outList);

	/* attempts to load and assign a body to the out parameter with either
	   a texture that matches the body's name or the optional matName param if not empty*/
	void forceGetBody(const str& name, Body& outBody, str matName = "");

	/* Attempts to retrieve a body from the body map with the specified name */
	void getBody(const str& name, Body& outBody);
	
	/* Loads a body with a specified material in the preloaded body map */
	void loadBody(const str& name, const str& mat);

	/* Attempts to load multiple bodies from the specified directory */
	void loadMultipleBodies(const str& directory);

	void getAllBodiesFromLevel(
		const str& lvlFilename,
		const float tileSize,
		uint3& outDims,
		std::list<StaticModel>& outList,
		std::list<StaticModel>& outLakeList,
		unsigned int**& ppoutMap);

	void getPathOf(const str& id, const Format frmt, str& strOut);
	void getAllFilenames(const str& directory, str_list &outFilenames);

	const str& getFormatPath(const Format& format) const;
	uint getNBodiesRejected() const;
	uint getNBodiesLoaded() const;

private:
	IOManager();
	IOManager(const IOManager&) = delete;
	void operator=(const IOManager&) = delete;
	bool IOManager::validPath(const str& path, const Format frmt);

private:
	static const char LEVEL_COMP_SEP = '|';
	static const char FILE_PATH_SEP = '\\';
	static const char FILE_EXT_SEP = '.';
	static const char FILE_NAME_SEP = '_';
	static const char VALUE_SEP = ',';
	static const char NAME_POS_SEP = ':';

private:
	std::map<str, Body> m_prelBodies;
	std::map<Format, str> m_suppFormats;
	std::map<Format, str> m_formatPaths;
	uint m_nBodiesRejected;
	uint m_nBodiesLoaded;
};