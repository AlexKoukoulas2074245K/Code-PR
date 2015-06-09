#include "bitmap.h"

Bitmap::Bitmap(){}
Bitmap::~Bitmap(){}

void Bitmap::FillData(const rgb_dat& source){ mRGBData.assign(source.begin(), source.end()); }
void Bitmap::FillDimensions(const rgb_dims& source){ mDims = source; }

void Bitmap::getRGBAt(const int x, const int y, rgb_trip& outRgb)
{
	if (!mLoaded) return;
	int fixedrow = mDims.height - 1 - y;
	if (fixedrow % 2 == 0) outRgb = mRGBData[fixedrow * mDims.height + mDims.width - 1 - x];
	outRgb = mRGBData[fixedrow * mDims.height + x];
}