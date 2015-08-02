#pragma once

#include <map>
#include <list>

#include "pokedef.h"
#include "hudcomponent.h"

class Renderer;
class IOManager;
class FontEngine
{
public:
	FontEngine(const std::string& fontName): mName(fontName){}
	~FontEngine(){}

	void PrepareFont(sptr<IOManager> iom, sptr<Renderer> rend);
	const float2& getGlyphPosition(const std::string& glyph){ return mGlyphMap[glyph]; }
	HUDComponent* modGlyphCompPointer(const std::string& glyph) { return &mGlyphComps[glyph]; }
	const std::string& getName() const { return mName; }
	Texture& modTexture() { return mFontTexture; }
	void setTexture(const Texture& texture) { mFontTexture = texture; }

private:
	static const uint FONT_CELL_SIZE = 16;
	static const float FONT_IMAGE_SIZE;
		
	std::string mName;
	Texture mFontTexture;
	std::map<std::string, float2> mGlyphMap;
	std::map<std::string, HUDComponent> mGlyphComps;
};