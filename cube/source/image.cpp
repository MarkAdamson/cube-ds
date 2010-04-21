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
