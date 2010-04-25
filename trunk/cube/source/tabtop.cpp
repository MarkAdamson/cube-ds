/*
 * Copyright Mark Adamson 2010
 *
 * This file is part of cube-ds.
 *
 * cube-ds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cube-ds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cube-ds.  If not, see <http://www.gnu.org/licenses/>.
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
