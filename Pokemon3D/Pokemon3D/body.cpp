#include "body.h"

Body::Body(){}
Body::Body(const std::list<Vertex>& vertices,
		   const std::list<index_t>& indices,
		   const Dimensions& dimensions)
{
	mVertices = vertices;
	mIndices = indices;
	mDimensions = dimensions;
	mActiveTexture = 0;
}

bool Body::isReady() const 
{
	return mVertices.size() > 0 &&
		   mIndices.size() > 0 &&
		   mTextures.size() > 0 &&
		   mTexturesToLoad.size() == 0; 
}