/*
 * image.h
 *
 *  Created on: 8 Apr 2010
 *      Author: mark
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <nds.h>
#include "gadget.h"

namespace WoopsiUI {

	class BitmapBase;

	/**
	 * Button used by the Calendar gadget to represent the days.
	 */
	class Image : public Gadget {
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
		Image(s16 x, s16 y, u16 width, u16 height, u16 bitmapX, u16 bitmapY, const BitmapBase* bitmap, u32 flags);

	protected:
		const BitmapBase* _bitmap;	/**< Bitmap to display when button is not clicked */
		u16 _bitmapX;
		u16 _bitmapY;

		/**
		 * Draw the area of this gadget that falls within the clipping region.
		 * Called by the redraw() function to draw all visible regions.
		 * @param port The GraphicsPort to draw to.
		 * @see redraw()
		 */
		virtual void drawContents(GraphicsPort* port);

		/**
		 * Destructor.
		 */
		virtual inline ~Image() { };

		/**
		 * Copy constructor is protected to prevent usage.
		 */
		inline Image(const Image& image) : Gadget(image) { };
	};
}


#endif /* IMAGE_H_ */
