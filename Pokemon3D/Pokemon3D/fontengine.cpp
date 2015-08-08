#include "fontengine.h"

#include <vector>

#include "ioutils.h"
#include "renderer.h"
#include "iomanager.h"

const uint FontEngine::FONT_CELL_SIZE = 8U;
const float FontEngine::FONT_IMAGE_SIZE = 128.0f;
const float FontEngine::FONT_SIZE = Body::BITMAP_INIT_WIDTH;

FontEngine& FontEngine::get()
{
	static FontEngine fe;
	return fe;
}

FontEngine::FontEngine(){}

void FontEngine::prepareFontEngine(const std::string& name)
{
	m_name = name;
	IOManager::str configPath;
	IOManager::get().getPathOf(m_name, IOManager::Format::FCF, configPath);
	std::list<std::string> fontConfigContents;
	IOManager::get().getFileContent(configPath, &fontConfigContents);

	IOManager::str imagePath;
	IOManager::get().getPathOf(m_name, IOManager::HUD, imagePath);
	Renderer::get().loadFontImage(imagePath, &m_fontTexture);
	uint cellX = 0U;
	uint cellY = 0U;
	for (std::list<std::string>::const_iterator citer = fontConfigContents.begin();
		 citer != fontConfigContents.end();
		 ++citer)
	{
		std::vector<std::string> splitLine = split(*citer, ' ');

		for (std::vector<std::string>::const_iterator cit = splitLine.begin();
			 cit != splitLine.end();
			 ++cit)
		{
			m_glyphMap[(*cit)[0]] = float2{static_cast<float>(cellX), static_cast<float>(cellY)};

			HUDComponent glyphComp;
			float2 newCoords[4];
			newCoords[0] = float2{cellX / FONT_IMAGE_SIZE,
								  cellY / FONT_IMAGE_SIZE};
			newCoords[1] = float2{cellX / FONT_IMAGE_SIZE + FONT_CELL_SIZE / FONT_IMAGE_SIZE,
								  cellY / FONT_IMAGE_SIZE};
			newCoords[2] = float2{cellX / FONT_IMAGE_SIZE + FONT_CELL_SIZE / FONT_IMAGE_SIZE,
								  cellY / FONT_IMAGE_SIZE + FONT_CELL_SIZE / FONT_IMAGE_SIZE};
			newCoords[3] = float2{cellX / FONT_IMAGE_SIZE,
								  cellY / FONT_IMAGE_SIZE + FONT_CELL_SIZE / FONT_IMAGE_SIZE};
			glyphComp.changeTexCoords(newCoords);
			glyphComp.setImage(m_name);
			Renderer::get().prepareHUD(&glyphComp);
			m_glyphComps[(*cit)[0]] = glyphComp;

			cellX += FONT_CELL_SIZE;
		}

		cellX = 0U;
		cellY += FONT_CELL_SIZE;
	}
}

const float2& FontEngine::getGlyphPosition(const char glyph) { return m_glyphMap[glyph]; }
const std::string& FontEngine::getName() const { return m_name; }

HUDComponent* FontEngine::modGlyphCompPointer(const char& glyph) { return &m_glyphComps[glyph]; }
Texture* FontEngine::modTexture() { return &m_fontTexture; }

void FontEngine::setTexture(const Texture& texture) { m_fontTexture = texture; }