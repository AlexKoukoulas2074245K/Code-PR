#include "body.h"

Body::Body():mActiveTexture(0){}
Body::~Body(){}

bool Body::isReady() const 
{
	return mVertices.size() > 0 &&
		   mIndices.size() > 0 &&
		   mTextures.size() > 0 &&
		   mTexturesToLoad.size() == 0; 
}