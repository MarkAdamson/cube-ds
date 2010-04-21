/*
 * tabtop.cpp
 *
 *  Created on: 31 Mar 2010
 *      Author: mark
 */

#include "tabtop.h"
#include "graphicsport.h"

using namespace WoopsiUI;

TabTop::TabTop(s16 x, s16 y, u16 width, u16 height, const WoopsiString& text, GadgetStyle* style) : CalendarDayButton(x, y, width, height, text, style) {
	_borderSize.top = 1;
	_borderSize.right = 1;
	_borderSize.bottom = 1;
	_borderSize.right = 1;

	_isStuckDown = false;
}

void TabTop::drawOutline(GraphicsPort* port) {

	// Stop drawing if the gadget indicates it should not have an outline
	if (isBorderless()) return;

	// Work out which colours to use
	u16 col1;
	u16 col2;

	if (isClicked() || _isStuckDown) {
		// Bevelled out of the screen
		col1 = getShineColour();
		col2 = getShadowColour();
		port->drawBevelledRect(0, 0, _width, _height, col1, col2);
		port->drawPixel(0, _height-1, col1);
		port->drawLine(1, _height-1, _width-2, _height-1, getBackColour());
	} else {
		// Bevelled into the screen
		col1 = getShadowColour();
		col2 = getShineColour();
		port->drawBevelledRect(0, 0, _width, _height, col1, col2);
	}

}
