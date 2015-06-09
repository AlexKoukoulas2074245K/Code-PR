#pragma once
#include "body.h"
#include "bitmap.h"
#include <map>
#include <string>
#include <vector>

class IOManager
{
public:
	
	enum Format
	{
		BMP, OBJ
	};

	IOManager();
	~IOManager();

	typedef std::string str;
	typedef std::list<std::string> str_list;
	typedef str_list::const_iterator str_list_iter;
	typedef str::size_type str_sizet;

	/* Renderable body methods */
	void ForceGetBody(const str& id, Body& outBody);
	void GetBody(const str& id, Body& outBody);
	void LoadBody(const str& path);
	void LoadMultipleBodies(const str& directory);

	/* Bitmap image methods */
	void ForceGetBmp(const str& id, Bitmap& outBmp);
	void GetBmp(const str& id, Bitmap& outBmp);
	void LoadBmp(const str& path);
	void LoadMultipleBmps(const str& directory);

	/* General File utilities */
	void GetAllFilenames(const str& directory, str_list &outFilenames);

private:
	bool ValidPath(const str& path, const Format frmt);

private:
	std::map<str, Body> mPrelBodies;
	std::map<str, Bitmap> mPrelBitmaps;
	std::map<Format, str> mSuppFormats;
};