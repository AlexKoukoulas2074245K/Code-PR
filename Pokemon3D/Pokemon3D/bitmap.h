#pragma once
#include <vector>

class Bitmap
{

public:
	typedef struct RGBTrip
	{
		int r, g, b;
	} rgb_trip;

	typedef struct RGBDims
	{
		int width, height;
	} rgb_dims;

	typedef std::vector<RGBTrip> rgb_dat;
	typedef rgb_dat::const_iterator rgb_const_iter;
	typedef rgb_dat::iterator rgb_iter;
	typedef rgb_dat::size_type rgb_sizet;

	Bitmap();
	~Bitmap();
	void FillData(const rgb_dat& source);
	void FillDimensions(const rgb_dims& source);
	void setLoaded(const bool loaded){ mLoaded = loaded; }
	void getRGBAt(const int x, const int y, rgb_trip& outRgb);
	const rgb_dims& getDims() const { return mDims; }
	bool isLoaded() const { return mLoaded; }

private:
	rgb_dat mRGBData;
	rgb_dims mDims;
	bool mLoaded;
};