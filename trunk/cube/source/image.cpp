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
 
#include "image.h"
#include "graphicsport.h"
#include "woopsi.h"
#include "bitmapbase.h"

using namespace WoopsiUI;

Image::Image(s16 x, s16 y, u16 width, u16 height, u16 bitmapX, u16 bitmapY, const BitmapBase* bitmap, u32 flags) : Gadget(x, y, width, height, flags, NULL) {
	_bitmapX = bitmapX;
	_bitmapY = bitmapY;
	_bitmap = bitmap;
}

void Image::drawContents(GraphicsPort* port) {

	Rect rect;
	getClientRect(rect);

	if (isEnabled()) {
		port->drawBitmap(0, 0, rect.width, rect.height, _bitmap, _bitmapX, _bitmapY);
	} else {
		port->drawBitmapGreyScale(0, 0, rect.width, rect.height, _bitmap, _bitmapX, _bitmapY);
	}
}
