#pragma once

#include <map>
#include <list>

#include "pokedef.h"
#include "hudcomponent.h"


class FontEngine
{
public:
	static const float FONT_SIZE;

public:
	static FontEngine& get();
public:
	void prepareFontEngine(const std::string& name);

	const float2& getGlyphPosition(const char glyph);
	const std::string& getName() const;
	
	HUDComponent* modGlyphCompPointer(const char& glyph);
	Texture* modTexture();

	void setTexture(const Texture& texture);

private:
	FontEngine();
	FontEngine(const FontEngine&) = delete;
	void operator=(const FontEngine&) = delete;

private:
	static const uint FONT_CELL_SIZE;
	static const float FONT_IMAGE_SIZE;

private:
	std::string m_name;
	Texture m_fontTexture;
	std::map<char, float2> m_glyphMap;
	std::map<char, HUDComponent> m_glyphComps;
};