#include "body.h"

Body::Body(){}
Body::Body(const std::list<Vertex>& vertices,
		   const std::list<uint>& indices,
		   const Dimensions& dimensions)
{
	mVertices = vertices;
	mIndices = indices;
	mDimensions = dimensions;
}