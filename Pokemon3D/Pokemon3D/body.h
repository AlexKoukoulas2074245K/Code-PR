#pragma once
#include <list>
#include <vector>

#include "texture.h"
#include "pokedef.h"

class Body
{
public:
	typedef struct Vertex
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	} vertex_t;

	/* typedefs for vertex and index lists */
	typedef uint index_t;
	typedef std::list<Vertex> vertex_list;
	typedef std::list<index_t> index_list;
	typedef std::vector<Texture> texture_list;
	typedef std::list<std::string> textoload_list;
	typedef vertex_list::size_type vertex_sizet;
	typedef index_list::size_type index_sizet;
	typedef texture_list::size_type texture_sizet;
	typedef comptr<ID3D11Buffer> body_buffer;
	typedef comptr<ID3D11InputLayout> body_layout;
	
	static const vertex_list BITMAP_VERTICES;
	static const index_list BITMAP_INDICES;
	static const float BITMAP_INIT_WIDTH;
	static const float BITMAP_INIT_HEIGHT;

	Body();
	~Body();

	bool isReady() const;
	const vertex_t& getVertex(vertex_sizet i);
	const vertex_list& getVertices() const { return mVertices; }
	const index_list& getIndices() const { return mIndices; }
	const float3& getDimensions() const { return mDimensions; }
	const float3& getInitDims() const { return mInitDims; }
	const vertex_sizet getVertexCount() const { return mVertices.size(); }
	const index_sizet getIndexCount() const { return mIndices.size(); }
	const Texture& getActiveTexture() const { return mTextures[mActiveTexture]; }
	
	void setVertex(vertex_sizet i, const vertex_t& v);
	void setVertices(const vertex_list& vertices) { mVertices = vertices; }
	void setIndices(const index_list& indices) {  mIndices = indices; }
	void setDimensions(const float3& dimensions) { mDimensions = dimensions; }
	void setInitDims(const float3 &initDims) { mInitDims = initDims; }
	void setTexturesToLoadList(const textoload_list& source) { mTexturesToLoad = source; }
	void setSingleTexture(const std::string& texFilename) { mTexturesToLoad.push_back(texFilename); }
	textoload_list& modTexturesToLoad() { return mTexturesToLoad; }
	texture_list& modTextures() { return mTextures; }
	body_buffer& modVertexBuffer() { return mVertexBuffer; }
	body_buffer& modIndexBuffer() { return mIndexBuffer; }
	const body_buffer& immVertexBuffer() { return mVertexBuffer; }
	const body_buffer& immIndexBuffer() { return mIndexBuffer; }

private:
	textoload_list mTexturesToLoad;
	texture_sizet mActiveTexture;
	texture_list mTextures;
	vertex_list mVertices;
	index_list mIndices;
	float3 mDimensions;
	float3 mInitDims;
	body_buffer mVertexBuffer;
	body_buffer mIndexBuffer;
};