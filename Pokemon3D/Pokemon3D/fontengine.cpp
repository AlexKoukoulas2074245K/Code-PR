#include "fontengine.h"

#include <vector>

#include "ioutils.h"
#include "renderer.h"
#include "iomanager.h"

const float FontEngine::FONT_IMAGE_SIZE = 256.0f;

void FontEngine::PrepareFontEngine(sptr<IOManager> iom, sptr<Renderer> rend)
{
	IOManager::str configPath;
	iom->GetPathOf(mName, IOManager::Format::FCF, configPath);
	std::list<std::string> fontConfigContents;
	iom->GetFileContent(configPath, &fontConfigContents);

	IOManager::str imagePath;
	iom->GetPathOf(mName, IOManager::HUD, imagePath);
	rend->LoadFontImage(imagePath, &mFontTexture);
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
			mGlyphMap[(*cit)[0]] = float2{static_cast<float>(cellX), static_cast<float>(cellY)};

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
			glyphComp.ChangeTexCoords(newCoords);
			glyphComp.setImage(mName, iom);
			rend->PrepareHUD(&glyphComp);
			mGlyphComps[(*cit)[0]] = glyphComp;

			cellX += FONT_CELL_SIZE;
		}

		cellX = 0U;
		cellY += FONT_CELL_SIZE;
	}
}