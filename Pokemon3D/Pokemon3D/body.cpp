#include "body.h"

const float Body::HUD_BODY_WIDTH = 0.1f;
const float Body::HUD_BODY_HEIGHT = 0.1f;
const Body::index_list Body::HUD_BODY_INDICES = {0, 1, 2, 2, 3, 0};
const Body::vertex_list Body::HUD_BODY_VERTICES = 
{
	Vertex{-Body::HUD_BODY_WIDTH / 2.0f,  Body::HUD_BODY_HEIGHT / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{ Body::HUD_BODY_WIDTH / 2.0f,  Body::HUD_BODY_HEIGHT / 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{ Body::HUD_BODY_WIDTH / 2.0f, -Body::HUD_BODY_HEIGHT / 2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	Vertex{-Body::HUD_BODY_WIDTH / 2.0f, -Body::HUD_BODY_HEIGHT / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
};

const float Body::BILLBOARD_BODY_WIDTH = 1.6f * ((float) GetSystemMetrics(SM_CXSCREEN) /
												 (float) GetSystemMetrics(SM_CYSCREEN));
const float Body::BILLBOARD_BODY_HEIGHT = 1.6f;
const Body::index_list Body::BILLBOARD_BODY_INDICES = {0, 1, 2, 3, 4, 5};
const Body::vertex_list Body::BILLBOARD_BODY_VERTICES = 
{
	Vertex{ Body::BILLBOARD_BODY_WIDTH / 2.0f, -Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	Vertex{ Body::BILLBOARD_BODY_WIDTH / 2.0f,  Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{-Body::BILLBOARD_BODY_WIDTH / 2.0f,  Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{-Body::BILLBOARD_BODY_WIDTH / 2.0f, -Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	Vertex{ Body::BILLBOARD_BODY_WIDTH / 2.0f, -Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	Vertex{-Body::BILLBOARD_BODY_WIDTH / 2.0f,  Body::BILLBOARD_BODY_HEIGHT / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
};
Body::Body():m_activeTexture(0){}
Body::~Body(){}

bool Body::isReady() const 
{
	return m_vertices.size() > 0 &&
		   m_indices.size() > 0 &&
		   m_textures.size() > 0 &&
		   m_texturesToLoad.size() == 0; 
}

const Body::vertex_t& Body::getVertex(vertex_sizet i) const
{
	vertex_list::const_iterator citer = m_vertices.begin();
	std::advance(citer, i);
	return *citer;
}

const Body::vertex_list& Body::getVertices() const { return m_vertices; }
const Body::index_list& Body::getIndices() const { return m_indices; }
const float3& Body::getDimensions() const { return m_dimensions; }
const float3& Body::getInitDims() const { return m_initDims; }
const Body::vertex_sizet Body::getVertexCount() const { return m_vertices.size(); }
const Body::index_sizet Body::getIndexCount() const { return m_indices.size(); }
const Texture& Body::getActiveTexture() const { return m_textures[m_activeTexture]; }
const Body::body_buffer& Body::getVertexBuffer() const { return m_vertexBuffer; }
const Body::body_buffer& Body::getIndexBuffer() const { return m_indexBuffer; }

Body::textoload_list& Body::modTexturesToLoad() { return m_texturesToLoad; }
Body::texture_list& Body::modTextures() { return m_textures; }
Body::body_buffer& Body::modVertexBuffer() { return m_vertexBuffer; }
Body::body_buffer& Body::modIndexBuffer() { return m_indexBuffer; }

void Body::setVertex(vertex_sizet i, const vertex_t& v)
{
	vertex_list::iterator iter = m_vertices.begin();
	std::advance(iter, i);
	*iter = v;
}

void Body::setVertices(const vertex_list& vertices) { m_vertices = vertices; }
void Body::setIndices(const index_list& indices) { m_indices = indices; }
void Body::setDimensions(const float3& dimensions) { m_dimensions = dimensions; }
void Body::setInitDims(const float3 &initDims) { m_initDims = initDims; }
void Body::setTexturesToLoadList(const textoload_list& source) { m_texturesToLoad = source; }
void Body::setSingleTexture(const std::string& texFilename) { m_texturesToLoad.push_back(texFilename); }