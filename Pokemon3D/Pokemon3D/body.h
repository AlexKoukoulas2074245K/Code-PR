#pragma once

#include <list>
#include <vector>
#include "d3dcommon.h"
#include "texture.h"

class Body
{
public:
	typedef struct Vertex
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	} vertex_t;

	struct Dimensions
	{
		float maxWidth, maxHeight, maxDepth;
	};

	/* typedefs for vertex and index lists */
	typedef unsigned int index_t;
	typedef std::list<Vertex> vertex_list;
	typedef std::list<index_t> index_list;
	typedef std::vector<Texture> texture_list;
	typedef std::list<const char* const> textoload_list;
	typedef vertex_list::size_type vertex_sizet;
	typedef index_list::size_type index_sizet;
	typedef texture_list::size_type texture_sizet;
	typedef comptr<ID3D11Buffer> body_buffer;
	typedef comptr<ID3D11InputLayout> body_layout;

	Body();
	Body(const vertex_list& vertices,
		 const index_list& indices,
		 const Dimensions& dimensions);

	bool isReady() const;
	const vertex_list& getVertices() const { return mVertices; }
	const index_list& getIndices() const { return mIndices; }
	const Dimensions& getDimensions() const { return mDimensions; }
	const vertex_sizet getVertexCount() const { return mVertices.size(); }
	const index_sizet getIndexCount() const { return mIndices.size(); }
	const Texture& getActiveTexture() const { return mTextures[mActiveTexture]; }

	void setVertices(const vertex_list& vertices) { mVertices = vertices; }
	void setIndices(const index_list& indices) {  mIndices = indices; }
	void setDimensions(const Dimensions& dimensions) { mDimensions = dimensions; }
	void setTexturesToLoadList(const textoload_list& source) { mTexturesToLoad = source; }
	void setSingleTexture(const char* const texFilename) { mTexturesToLoad.push_back(texFilename); }

	textoload_list& modTexturesToLoad() { return mTexturesToLoad; }
	texture_list& modTextures() { return mTextures; }
	body_buffer& modVertexBuffer(){ return mVertexBuffer; }
	body_buffer& modIndexBuffer(){ return mIndexBuffer; }
	const body_buffer& immVertexBuffer(){ return mVertexBuffer; }
	const body_buffer& immIndexBuffer(){ return mIndexBuffer; }

private:
	textoload_list mTexturesToLoad;
	texture_sizet mActiveTexture;
	texture_list mTextures;
	vertex_list mVertices;
	index_list mIndices;
	Dimensions mDimensions;
	body_buffer mVertexBuffer;
	body_buffer mIndexBuffer;
};