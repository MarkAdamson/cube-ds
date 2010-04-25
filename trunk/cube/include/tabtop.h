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

#ifndef TABTOP_H_
#define TABTOP_H_

#include <nds.h>
#include "calendardaybutton.h"
#include "gadgetstyle.h"

namespace WoopsiUI {

	/**
	 * Button used by the Calendar gadget to represent the days.
	 */
	class TabTop : public CalendarDayButton {
	public:

		/**
		 * Constructor for buttons that display a string.
		 * @param x The x co-ordinate of the button, relative to its parent.
		 * @param y The y co-ordinate of the button, relative to its parent.
		 * @param width The width of the button.
		 * @param height The height of the button.
		 * @param text The text for the button to display.
		 * @param style The style that the button should use.  If this is not
		 * specified, the button will use the values stored in the global
		 * defaultGadgetStyle object.  The button will copy the properties of
		 * the style into its own internal style object.
		 */
		TabTop(s16 x, s16 y, u16 width, u16 height, const WoopsiString& text, GadgetStyle* style = NULL);

		/**
		 * Sets the key's stuck down state.  If this is true, the key has a inwards-bevelled
		 * border when drawn.  If it is false, the key has an outwards-bevelled border.
		 * @param isStuckDown The new stuck down state.
		 */
		inline void setStuckDown(bool isStuckDown) { _isStuckDown = isStuckDown; };

	protected:

		/**
		 * Draws the outline of the button.
		 * @param port Graphics port to draw to.
		 */
		virtual void drawOutline(GraphicsPort* port);

		/**
		 * Destructor.
		 */
		virtual inline ~TabTop() { };

		/**
		 * Copy constructor is protected to prevent usage.
		 */
		inline TabTop(const TabTop& button) : CalendarDayButton(button) { };
	};
}


#endif /* TABTOP_H_ */
