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
		OBJ, PNG, HUD, FCF
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

	/* Renderable body methods */
	void forceGetBody(const str& name, Body& outBody);
	void getBody(const str& name, Body& outBody);
	void loadBody(const str& name, const str& mat);
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
};