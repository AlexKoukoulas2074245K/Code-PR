#pragma once
#include <string>
#include "pokedef.h"
#include "body.h"
#include "bitmap.h"

namespace disorganizedData
{
	struct disorganized_Vertex{ float x, y, z; };
	struct disorganized_Texcoord{ float u, v; };
	struct disorganized_Normal{ float nx, ny, nz; };
}

namespace organizedData
{
	struct OBJIndex{ uint vertexIndex, texcoordIndex, normalIndex; };
}

/* Core obj file loader */
bool LoadOBJFromFile(std::string filename, Body& outModel);
bool LoadBMPFromFile(std::string filename, Bitmap& outBmp);

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);
