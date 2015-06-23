#pragma once
#include "body.h"
#include "bitmap.h"
#include "renderer.h"
#include <map>
#include <string>
#include <vector>

class IOManager
{
public:
	
	enum Format
	{
		BMP, OBJ, PNG
	};

	typedef Renderer::std_pos std_pos;
	typedef struct BodyStruct
	{
		Body body;
		std_pos pos;
	} std_body;

	typedef std::string str;
	typedef std::list<std::string> str_list;
	typedef str_list::const_iterator str_list_iter;
	typedef str::size_type str_sizet;

	IOManager();
	~IOManager();

	/* Renderer setting */
	void SetRenderer(const sptr<Renderer>& renderer){ mRenderer = renderer; }

	/* Renderable body methods */
	void ForceGetBody(const str& id, Body& outBody);
	void GetBody(const str& id, Body& outBody);
	void LoadBody(const str& id, const str& mat);
	void LoadMultipleBodies(const str& directory);
	void GetAllBodiesFromLevel(
		const str& lvlFilename,
		const float tileSize,
		std::list<std_body>& outList);

	/* Bitmap image methods */
	void ForceGetBmp(const str& id, Bitmap& outBmp);
	void GetBmp(const str& id, Bitmap& outBmp);
	void LoadBmp(const str& id);
	void LoadMultipleBmps(const str& directory);


	void GetAllFilenames(const str& directory, str_list &outFilenames);

private:
	bool IOManager::ValidPath(const str& path, const Format frmt);
	void GetPathOf(const str& id, const Format frmt, str& strOut);

private:
	static const char LEVEL_COMP_SEP = '|';
	static const char FILE_PATH_SEP = '\\';
	static const char FILE_EXT_SEP = '.';
	static const char FILE_NAME_SEP = '_';

	sptr<Renderer> mRenderer;
	std::map<str, Body> mPrelBodies;
	std::map<str, Bitmap> mPrelBitmaps;
	std::map<Format, str> mSuppFormats;
	std::map<Format, str> mFormatPaths;
};