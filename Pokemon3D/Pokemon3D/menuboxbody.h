#pragma once

#include <string>
#include <list>

#include "pokedef.h"
#include "hudcomponent.h"

class MenuBoxBody
{
public:
	MenuBoxBody();
	~MenuBoxBody();

	void fill(
		const std::string& content,
		const float2& offset,
		const bool hasHorArrow = false);
	
	void render(const bool renderContent = true);

private:
	static const float2 BORDER_TL_COORDS[4];
	static const float2 BORDER_T_COORDS[4];
	static const float2 BORDER_TR_COORDS[4];
	static const float2 BORDER_R_COORDS[4];
	static const float2 BORDER_BR_COORDS[4];
	static const float2 BORDER_B_COORDS[4];
	static const float2 BORDER_BL_COORDS[4];
	static const float2 BORDER_L_COORDS[4];

private:
	void addHorSpacing();

private:
	std::string m_content;
	std::list<HUDComponent> m_borderPieces;
	HUDComponent m_base;
	float2 m_dimensions, m_offset, m_textOffset;
};