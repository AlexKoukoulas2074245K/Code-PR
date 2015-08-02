#include "body.h"

const Body::vertex_list Body::BITMAP_VERTICES = 
{
	Vertex{-0.05f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{0.05f, 0.05f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f},
	Vertex{0.05f, -0.05f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	Vertex{-0.05f, -0.05f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
};
const Body::index_list Body::BITMAP_INDICES = {0, 1, 2, 2, 3, 0};
const float Body::BITMAP_INIT_WIDTH = 1.0f;
const float Body::BITMAP_INIT_HEIGHT = 1.0f;

Body::Body():mActiveTexture(0){}
Body::~Body(){}

const Body::vertex_t& Body::getVertex(vertex_sizet i)
{
	vertex_list::const_iterator citer = mVertices.begin();
	std::advance(citer, i);
	return *citer;
}

void Body::setVertex(vertex_sizet i, const vertex_t& v)
{
	vertex_list::iterator iter = mVertices.begin();
	std::advance(iter, i);
	*iter = v;
}

bool Body::isReady() const 
{
	return mVertices.size() > 0 &&
		   mIndices.size() > 0 &&
		   mTextures.size() > 0 &&
		   mTexturesToLoad.size() == 0; 
}