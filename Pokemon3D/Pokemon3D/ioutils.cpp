#include <fstream>
#include <sstream>
#include "ioutils.h"

bool LoadOBJFromFile(std::string filename, Body& result)
{

	std::string strname(filename);
	std::ifstream file;

	file.open(filename);

	if (!file.is_open())
	{
		LOGLN(("File does not exist: " + std::string(filename)).c_str());
		return false;
	}

	// Initialize placeholder vectors and the indices
	std::vector<disorganizedData::disorganized_Vertex> disVertices;
	std::vector<disorganizedData::disorganized_Texcoord> disTexcoords;
	std::vector<disorganizedData::disorganized_Normal> disNormals;
	std::vector<organizedData::OBJIndex> orgIndices;
	std::string objectFilename = split(filename, '/').back();
	std::string objectName = split(objectFilename, '.').front();
	std::string objectFirstName = split(objectName, '_').front();
	bool is2d = !objectFirstName.compare("2d");
	
	std::string line;
	for (;std::getline(file, line);)
	{
		if (line[0] == '#') continue;

		std::vector<std::string> splitContents = split(line, ' ');
		std::string prefix = splitContents[0];

		if (prefix[0] == 'v')
		{
			if (prefix[1] == 't')
			{
				disorganizedData::disorganized_Texcoord rawTexcoord = {};
				rawTexcoord.u = is2d ? 1 - std::stof(splitContents[1]) : std::stof(splitContents[1]);
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
	result.setInitDims(resultDimensions);

	return true;
}

bool LoadBMPFromFile(std::string filename, Bitmap& outBmp)
{
	FILE* f;
	fopen_s(&f, filename.c_str(), "rb");

	if (f == NULL)
	{
		LOGLN(("File does not exist: " + std::string(filename)).c_str());
		return false;
	}
	
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	Bitmap::rgb_dat resultData;
	Bitmap::rgb_dims resultDims;

	// extract image height and width from header
	resultDims.width = *(int*) &info[18];
	resultDims.height = *(int*) &info[22];
	int row_padded = (resultDims.width * 3 + 3) & (~3);
	
	unsigned char* data = new unsigned char[row_padded];
	unsigned char tmp;

	
	for (int i = 0; i < resultDims.height; i++)
	{
		fread(data, sizeof(unsigned char), row_padded, f);
		for (int j = 0; j < resultDims.width * 3; j += 3)
		{
			// Convert (B, G, R) to (R, G, B)
			tmp = data[j];
			data[j] = data[j + 2];
			data[j + 2] = tmp;

			Bitmap::RGBTrip rgb;
			rgb.r = (int) data[j];
			rgb.g = (int) data[j + 1];
			rgb.b = (int) data[j + 2];
			resultData.push_back(rgb);
		}
	}

	outBmp.FillData(resultData);
	outBmp.FillDimensions(resultDims);

	delete[] data;
	fclose(f);

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