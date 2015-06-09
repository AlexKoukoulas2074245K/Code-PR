#include "ioutils.h"

// Splitting helpers defined at the bottom
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

bool LoadOBJFromFile(const char * const filename, Body& result)
{

	std::string strname(filename);
	std::ifstream file;

	file.open(filename);

	if (!file.is_open())
	{
		LOGLN("Could not open: ");
		LOG(filename);
		LOGLN("");
		return false;
	}

	// Initialize placeholder vectors and the indices
	std::vector<disorganizedData::disorganized_Vertex> disVertices;
	std::vector<disorganizedData::disorganized_Texcoord> disTexcoords;
	std::vector<disorganizedData::disorganized_Normal> disNormals;
	std::vector<organizedData::OBJIndex> orgIndices;

	std::string line;
	for (; std::getline(file, line);)
	{
		if (line[0] == '#') continue;

		std::vector<std::string> splitContents = split(line, ' ');
		std::string prefix = splitContents[0];

		if (prefix[0] == 'v')
		{
			if (prefix[1] == 't')
			{
				disorganizedData::disorganized_Texcoord rawTexcoord = {};
				rawTexcoord.u = std::stof(splitContents[1]);
				rawTexcoord.v = 1 - std::stof(splitContents[2]);
				disTexcoords.push_back(rawTexcoord);
			}
			else if (prefix[1] == 'n')
			{
				disorganizedData::disorganized_Normal rawNormal = {};
				rawNormal.nx = std::stof(splitContents[1]);
				rawNormal.ny = std::stof(splitContents[2]);
				rawNormal.nz = std::stof(splitContents[3]);
				disNormals.push_back(rawNormal);
			}
			else
			{
				disorganizedData::disorganized_Vertex rawVertex = {};
				rawVertex.x = std::stof(splitContents[1]);
				rawVertex.y = std::stof(splitContents[2]);
				rawVertex.z = std::stof(splitContents[3]);
				disVertices.push_back(rawVertex);
			}
		}
		else if (prefix[0] == 'f')
		{
			for (int i = 1; i < 4; i++)
			{
				organizedData::OBJIndex objIndex = {};

				std::vector<std::string> splitIndex = split(splitContents[i], '/');
				if (splitIndex[0].size() != 0)
				{
					objIndex.vertexIndex = std::stoi(splitIndex[0]) - 1;
				}
				if (splitIndex[1].size() != 0)
				{
					objIndex.texcoordIndex = std::stoi(splitIndex[1]) - 1;
				}
				if (splitIndex[2].size() != 0)
				{
					objIndex.normalIndex = std::stoi(splitIndex[2]) - 1;
				}

				orgIndices.push_back(objIndex);
			}
		}
	}

	file.close();

	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = 0.0f; maxX = 0.0f;
	minY = 0.0f; maxY = 0.0f;
	minZ = 0.0f; maxZ = 0.0f;

	std::list<Body::Vertex> resultVertices;
	std::list<uint> resultIndices;

	for (size_t i = 0; i < orgIndices.size(); i++)
	{
		Body::Vertex finalVertex = {};
		if (disVertices.size() != 0)
		{
			finalVertex.x = disVertices[orgIndices[i].vertexIndex].x;
			finalVertex.y = disVertices[orgIndices[i].vertexIndex].y;
			finalVertex.z = disVertices[orgIndices[i].vertexIndex].z;

			if (finalVertex.x > maxX) maxX = finalVertex.x;
			if (finalVertex.x < minX) minX = finalVertex.x;
			if (finalVertex.y > maxY) maxY = finalVertex.y;
			if (finalVertex.y < minY) minY = finalVertex.y;
			if (finalVertex.z > maxZ) maxZ = finalVertex.z;
			if (finalVertex.z < minZ) minZ = finalVertex.z;
		}
		if (disTexcoords.size() != 0)
		{
			finalVertex.u = disTexcoords[orgIndices[i].texcoordIndex].u;
			finalVertex.v = disTexcoords[orgIndices[i].texcoordIndex].v;
		}
		if (disNormals.size() != 0)
		{
			finalVertex.nx = disNormals[orgIndices[i].normalIndex].nx;
			finalVertex.ny = disNormals[orgIndices[i].normalIndex].ny;
			finalVertex.nz = disNormals[orgIndices[i].normalIndex].nz;
		}

		resultVertices.push_back(finalVertex);
		resultIndices.push_back(i);
	}
	
	Body::Dimensions resultDimensions;
	resultDimensions.maxDepth = std::abs(maxZ - minZ);
	resultDimensions.maxHeight = std::abs(maxY - minY);
	resultDimensions.maxWidth = std::abs(maxX - minX);
	
	result.setVertices(resultVertices);
	result.setIndices(resultIndices);
	result.setDimensions(resultDimensions);

	return true;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}