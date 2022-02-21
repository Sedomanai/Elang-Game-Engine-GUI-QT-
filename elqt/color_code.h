#pragma once
#include <common/math.h>


namespace el
{
	struct ElangEditorColor
	{
		color8 createRect, selectRect;
		color8 cell, cellHovered, cellSelected, cellSizing, cellShadow, cellOpenHanded, cellClosedHanded;
		unsigned char cellFillAlpha;

		ElangEditorColor() : 
			createRect(255, 0, 0, 255),
			selectRect(255, 255, 255, 255),
			cell(0, 255, 55, 255), 
			cellHovered(30, 255, 220, 255),
			cellSelected(100, 100, 240, 255), 
			cellSizing(255, 120, 220, 255),
			cellShadow(160, 160, 240, 255),
			cellOpenHanded(180, 255, 30, 255), 
			cellClosedHanded(160, 160, 240, 255),
			cellFillAlpha(80)
		{}
	};

	inline ElangEditorColor gEditorColor;
}
