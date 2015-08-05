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
	FontEngine(const std::string& fontName);
	~FontEngine();

	void prepareFontEngine(sptr<IOManager> iom, sptr<Renderer> rend);

	const float2& getGlyphPosition(const char glyph);
	const std::string& getName() const;
	
	HUDComponent* modGlyphCompPointer(const char& glyph);
	Texture* modTexture();

	void setTexture(const Texture& texture);

private:
	static const uint FONT_CELL_SIZE = 16;
	static const float FONT_IMAGE_SIZE;

private:
	std::string m_name;
	Texture m_fontTexture;
	std::map<char, float2> m_glyphMap;
	std::map<char, HUDComponent> m_glyphComps;
};