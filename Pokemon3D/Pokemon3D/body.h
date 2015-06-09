#pragma once

#include <list>
#include <vector>
#include "common.h"

class Body
{
public:
	struct Vertex
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

	struct Dimensions
	{
		float maxWidth, maxHeight, maxDepth;
	};

	/* typedefs for vertex and index lists */
	typedef std::list<Vertex> vertex_list;
	typedef std::list<uint> index_list;

	Body();
	Body(const vertex_list& vertices,
		 const index_list& indices,
		 const Dimensions& dimensions);

	bool isLoaded() const { return mLoaded; }
	vertex_list& getVertices() { return mVertices; }
	index_list& getIndices() { return mIndices; }
	Dimensions& getDimensions(){ return mDimensions; }

	void setLoaded(const bool loaded) { mLoaded = loaded; }
	void setVertices(const vertex_list& vertices) { mVertices = vertices; }
	void setIndices(const index_list& indices) {  mIndices = indices; }
	void setDimensions(const Dimensions& dimensions) { mDimensions = dimensions; }

private:
	bool mLoaded;
	std::list<void*> mMaterials;
	vertex_list mVertices;
	index_list mIndices;
	Dimensions mDimensions;
};