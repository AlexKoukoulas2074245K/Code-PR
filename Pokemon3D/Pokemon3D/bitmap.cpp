#include "bitmap.h"

Bitmap::Bitmap(){}
Bitmap::~Bitmap(){}

void Bitmap::FillData(const rgb_dat& source){ mRGBData.assign(source.begin(), source.end()); }
void Bitmap::FillDimensions(const uint2& source){ mDims = source; }

void Bitmap::getRGBAt(const int x, const int y, int3& outRgb)
{
	if (!mLoaded) return;
	int fixedrow = mDims.x - 1 - y;
	if (fixedrow % 2 == 0) outRgb = mRGBData[fixedrow * mDims.y + mDims.x - 1 - x];
	outRgb = mRGBData[fixedrow * mDims.y + x];
}