#include "menuboxbody.h"

#include "fontengine.h"
#include "common.h"
#include "iomanager.h"
#include "renderer.h"

const float2 MenuBoxBody::BORDER_TL_COORDS[] = {{0.00f, 0.00f}, {0.25f, 0.00f}, {0.25f, 0.25f}, {0.00f, 0.25f}};
const float2 MenuBoxBody::BORDER_T_COORDS[]  = {{0.25f, 0.00f}, {0.50f, 0.00f}, {0.50f, 0.25f}, {0.25f, 0.25f}};
const float2 MenuBoxBody::BORDER_TR_COORDS[] = {{0.50f, 0.00f}, {0.75f, 0.00f}, {0.75f, 0.25f}, {0.50f, 0.25f}};
const float2 MenuBoxBody::BORDER_R_COORDS[]  = {{0.50f, 0.25f}, {0.75f, 0.25f}, {0.75f, 0.50f}, {0.50f, 0.50f}};
const float2 MenuBoxBody::BORDER_BR_COORDS[] = {{0.50f, 0.50f}, {0.75f, 0.50f}, {0.75f, 0.75f}, {0.50f, 0.75f}};
const float2 MenuBoxBody::BORDER_B_COORDS[]  = {{0.25f, 0.50f}, {0.50f, 0.50f}, {0.50f, 0.75f}, {0.25f, 0.75f}};
const float2 MenuBoxBody::BORDER_BL_COORDS[] = {{0.00f, 0.50f}, {0.25f, 0.50f}, {0.25f, 0.75f}, {0.00f, 0.75f}};
const float2 MenuBoxBody::BORDER_L_COORDS[]  = {{0.00f, 0.25f}, {0.25f, 0.25f}, {0.25f, 0.50f}, {0.00f, 0.50f}};

MenuBoxBody::MenuBoxBody(){}
MenuBoxBody::~MenuBoxBody(){}

void MenuBoxBody::fill(
	const std::string& content,
	const float2& offset,
	const bool hasHorArrow /* false */)
{
	m_content = content;
	if (hasHorArrow) addHorSpacing();
	m_textOffset = offset;
	m_offset = {};
	m_offset.x = offset.x - FontEngine::FONT_SIZE * 0.5f / AppConfig::ASPECT;
	m_offset.y = offset.y + FontEngine::FONT_SIZE * 0.5f;

	float baseWidthCounter = 0.0f, maxWidth = 0.0f;
	float baseHeightCounter = FontEngine::FONT_SIZE;
	uint nCharCounter = 0U, maxNChars = 0U;
	uint nRows = 1U;
	for (std::string::const_iterator citer = m_content.begin();
		 citer != m_content.end();
		 ++citer)
	{
		if (*citer != '\n')
		{
			baseWidthCounter += FontEngine::FONT_SIZE / AppConfig::ASPECT;
			++nCharCounter;
			continue;
		}
		
		++nRows;
		baseHeightCounter += FontEngine::FONT_SIZE;
		if (baseWidthCounter > maxWidth)
		{
			maxWidth = baseWidthCounter;
			maxNChars = nCharCounter;
		}
		baseWidthCounter = 0.0f;
		nCharCounter = 0U;
	}
	if (baseWidthCounter > maxWidth)
	{
		maxWidth = baseWidthCounter;
		maxNChars = nCharCounter;
	}

	float2 baseDimensions = {};
	baseDimensions.x = maxWidth;
	baseDimensions.y = baseHeightCounter;

	m_base.setImage("menuboxbg");
	m_base.setDimensions(baseDimensions.x * AppConfig::ASPECT, baseDimensions.y);
	m_base.setPosition(m_offset.x + baseDimensions.x * 0.5f, m_offset.y - baseDimensions.y * 0.5f);
	Renderer::get().prepareHUD(&m_base);

	/* Create the top and bottom border pieces */
	float xCounter = offset.x;
	float topCompY = offset.y + FontEngine::FONT_SIZE;
	float botCompY = offset.y - baseDimensions.y;
	LOGLN(m_base.getPosition().y);
	LOGLN(botCompY);

	for (uint i = 0U;
		 i < maxNChars;
		 ++i)
	{
		HUDComponent topComp;
		topComp.setImage("hudbodycomps");
		topComp.changeTexCoords(BORDER_T_COORDS);
		topComp.setPosition(xCounter, topCompY);
		Renderer::get().prepareHUD(&topComp);
		m_borderPieces.push_back(topComp);

		HUDComponent botComp;
		botComp.setImage("hudbodycomps");
		botComp.changeTexCoords(BORDER_B_COORDS);
		botComp.setPosition(xCounter, botCompY);
		Renderer::get().prepareHUD(&botComp);
		m_borderPieces.push_back(botComp);
		xCounter += FontEngine::FONT_SIZE / AppConfig::ASPECT;
	}

	/* Create the left and right border pieces */
	float yCounter = offset.y;
	float leftCompX = offset.x - FontEngine::FONT_SIZE * 0.5f;
	float rightCompX = xCounter;

	for (uint i = 0U;
		i < nRows;
		++i)
	{
		HUDComponent leftComp;
		leftComp.setImage("hudbodycomps");
		leftComp.changeTexCoords(BORDER_L_COORDS);
		leftComp.setPosition(leftCompX, yCounter);
		Renderer::get().prepareHUD(&leftComp);
		m_borderPieces.push_back(leftComp);

		HUDComponent rightComp;
		rightComp.setImage("hudbodycomps");
		rightComp.changeTexCoords(BORDER_R_COORDS);
		rightComp.setPosition(rightCompX, yCounter);
		Renderer::get().prepareHUD(&rightComp);
		m_borderPieces.push_back(rightComp);
		yCounter -= FontEngine::FONT_SIZE;
	}

	/* Create the corner pieces */
	HUDComponent topleftComp;
	topleftComp.setImage("hudbodycomps");
	topleftComp.changeTexCoords(BORDER_TL_COORDS);
	topleftComp.setPosition(leftCompX, topCompY);
	Renderer::get().prepareHUD(&topleftComp);
	m_borderPieces.push_back(topleftComp);

	HUDComponent toprightComp;
	toprightComp.setImage("hudbodycomps");
	toprightComp.changeTexCoords(BORDER_TR_COORDS);
	toprightComp.setPosition(xCounter, topCompY);
	Renderer::get().prepareHUD(&toprightComp);
	m_borderPieces.push_back(toprightComp);

	HUDComponent botrightComp;
	botrightComp.setImage("hudbodycomps");
	botrightComp.changeTexCoords(BORDER_BR_COORDS);
	botrightComp.setPosition(xCounter, botCompY);
	Renderer::get().prepareHUD(&botrightComp);
	m_borderPieces.push_back(botrightComp);

	HUDComponent botleftComp;
	botleftComp.setImage("hudbodycomps");
	botleftComp.changeTexCoords(BORDER_BL_COORDS);
	botleftComp.setPosition(leftCompX, botCompY);
	Renderer::get().prepareHUD(&botleftComp);
	m_borderPieces.push_back(botleftComp);
}

void MenuBoxBody::render(const bool renderContent /* true */)
{
	Renderer::get().renderHUD(&m_base);

	for (std::list<HUDComponent>::iterator iter = m_borderPieces.begin();
		 iter != m_borderPieces.end();
		 ++iter)
	{
		Renderer::get().renderHUD(&(*iter));
	}

	if(renderContent) Renderer::get().renderText(m_content, m_textOffset);
}

void MenuBoxBody::addHorSpacing()
{
	for (std::string::size_type i = 0;
		 i < m_content.size();
		 ++i)
	{
		if (m_content[i] == '\n' ) m_content.insert(m_content.begin() + i + 1, ' ');
	}
}