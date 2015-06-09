#pragma once
#include "body.h"
#include <map>
#include <string>

class IOManager
{
public:
	IOManager();
	~IOManager();

	typedef std::string str;
	typedef std::list<std::string> str_list;
	typedef str_list::const_iterator str_list_iter;

	void ForceGetBody(const str& id, Body& outBody);
	void GetBody(const str& id, Body& outBody);
	void LoadBody(const str& path);
	void LoadMultipleBodies(const str& directory);
	void GetAllFilenames(const str& directory, str_list &outFilenames);

private:
	std::map<std::string, Body>mPreLoaded;
};