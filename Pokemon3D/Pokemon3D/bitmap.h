#pragma once
#include "pokedef.h"
#include <vector>

class Bitmap
{

public:
	typedef std::vector<int3> rgb_dat;
	typedef rgb_dat::const_iterator rgb_const_iter;
	typedef rgb_dat::iterator rgb_iter;
	typedef rgb_dat::size_type rgb_sizet;

	Bitmap();
	~Bitmap();
	void FillData(const rgb_dat& source);
	void FillDimensions(const uint2& source);
	void setLoaded(const bool loaded){ mLoaded = loaded; }
	void getRGBAt(const int x, const int y, int3& outRgb);
	const uint2& getDims() const { return mDims; }
	bool isLoaded() const { return mLoaded; }

private:
	rgb_dat mRGBData;
	uint2 mDims;
	bool mLoaded;
};