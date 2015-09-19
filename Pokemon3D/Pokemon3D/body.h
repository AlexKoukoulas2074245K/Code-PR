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
	
	static const vertex_list HUD_BODY_VERTICES;
	static const index_list HUD_BODY_INDICES;
	static const float HUD_BODY_WIDTH;
	static const float HUD_BODY_HEIGHT;

	static const vertex_list BILLBOARD_BODY_VERTICES;
	static const index_list BILLBOARD_BODY_INDICES;
	static const float BILLBOARD_BODY_WIDTH;
	static const float BILLBOARD_BODY_HEIGHT;

public:
	Body();
	~Body();

	bool isReady() const;
	const vertex_t& getVertex(vertex_sizet i) const;
	const vertex_list& getVertices() const;
	const index_list& getIndices() const;
	const float3& getDimensions() const;
	const float3& getInitDims() const;
	const vertex_sizet getVertexCount() const;
	const index_sizet getIndexCount() const;
	const Texture& getActiveTexture() const;
	const body_buffer& getVertexBuffer() const;
	const body_buffer& getIndexBuffer() const;
	
	textoload_list& modTexturesToLoad();
	texture_list& modTextures();
	body_buffer& modVertexBuffer();
	body_buffer& modIndexBuffer();

	void setVertex(vertex_sizet i, const vertex_t& v);
	void setVertices(const vertex_list& vertices);
	void setIndices(const index_list& indices);
	void setDimensions(const float3& dimensions);
	void setInitDims(const float3 &initDims);
	void setTexturesToLoadList(const textoload_list& source);
	void setSingleTexture(const std::string& texFilename);
	
	mat4x4 orientation;

private:
	textoload_list m_texturesToLoad;
	texture_sizet m_activeTexture;
	texture_list m_textures;
	vertex_list m_vertices;
	index_list m_indices;
	float3 m_dimensions;
	float3 m_initDims;
	body_buffer m_vertexBuffer;
	body_buffer m_indexBuffer;
};