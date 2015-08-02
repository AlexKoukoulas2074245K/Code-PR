#pragma once
#include <map>
#include <string>
#include <list>

#include "pokedef.h"
#include "staticmodel.h"

class Renderer;
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

	IOManager();
	~IOManager();

	/* Renderer setting */
	void SetRenderer(const sptr<Renderer>& renderer);

	/* Retrieves the content of the specified file and returns it split by newline */
	bool GetFileContent(const str& path, str_list* outList);

	/* Renderable body methods */
	void ForceGetBody(const str& name, Body& outBody);
	void GetBody(const str& name, Body& outBody);
	void LoadBody(const str& name, const str& mat);
	void LoadMultipleBodies(const str& directory);
	void GetAllBodiesFromLevel(
		const str& lvlFilename,
		const float tileSize,
		uint3& outDims,
		std::list<StaticModel>& outList,
		std::list<StaticModel>& outLakeList,
		unsigned int**& ppoutMap);

	void GetPathOf(const str& id, const Format frmt, str& strOut);
	void GetAllFilenames(const str& directory, str_list &outFilenames);

private:
	bool IOManager::ValidPath(const str& path, const Format frmt);

private:
	static const char LEVEL_COMP_SEP = '|';
	static const char FILE_PATH_SEP = '\\';
	static const char FILE_EXT_SEP = '.';
	static const char FILE_NAME_SEP = '_';
	static const char VALUE_SEP = ',';
	static const char NAME_POS_SEP = ':';

	sptr<Renderer> mRenderer;
	std::map<str, Body> mPrelBodies;
	std::map<Format, str> mSuppFormats;
	std::map<Format, str> mFormatPaths;
};