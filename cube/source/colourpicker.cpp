#include <nds.h>
#include "colourpicker.h"
#include "button.h"
#include "sliderhorizontal.h"
#include "woopsifuncs.h"

using namespace WoopsiUI;

ColourPicker::ColourPicker(s16 x, s16 y, u16 width, u16 height, const WoopsiString& title, u16 colour, u32 flags, GadgetStyle* style) : AmigaWindow(x, y, width, height, title, flags, AMIGA_WINDOW_SHOW_DEPTH, style) {

	// Increase the size of the border to leave space between gadgets and the 
	// border decorations
	_borderSize.top += 2;
	_borderSize.right += 2;
	_borderSize.bottom += 2;
	_borderSize.left += 2;

	Rect rect;
	getClientRect(rect);

	// Create OK button
	_okButton = new Button(0, 0, 0, 0, "OK");

	Rect buttonRect;
	_okButton->getPreferredDimensions(buttonRect);

	// Calculate OK button dimensions
	buttonRect.width = (rect.width >> 2) - 1;
	buttonRect.x = rect.x;
	buttonRect.y = (rect.y + rect.height) - buttonRect.height;

	_okButton->changeDimensions(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);

	_okButton->addGadgetEventHandler(this);
	addGadget(_okButton);

	// Calculate cancel button dimensions
	buttonRect.x = rect.x + rect.width - buttonRect.width;
	buttonRect.y = (rect.y + rect.height) - buttonRect.height;

	// Create cancel button
	_cancelButton = new Button(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height, "Cancel");
	_cancelButton->addGadgetEventHandler(this);
	addGadget(_cancelButton);


	// Create Red Slider
	_redSlider = new SliderHorizontal(0,0,10,10);

	// Calculate Red Slider dimensions
	_redSlider->getPreferredDimensions(buttonRect);

	buttonRect.width = rect.width - (rect.width >> 2) - 1;
	buttonRect.x = rect.x;
	buttonRect.y = rect.y;

	_redSlider->changeDimensions(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);

	_redSlider->setMinimumValue(0);
	_redSlider->setMaximumValue(31);
	_redSlider->setValue(23);

	_redSlider->addGadgetEventHandler(this);
	addGadget(_redSlider);
	_redSlider->redraw();

	// Calculate Green Slider Dimensions
	buttonRect.y = buttonRect.y + buttonRect.height + 1;

	// Create Green Slider
	_greenSlider = new SliderHorizontal(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);

	_greenSlider->setMinimumValue(0);
	_greenSlider->setMaximumValue(31);
	_greenSlider->setValue(0);

	_greenSlider->addGadgetEventHandler(this);
	addGadget(_greenSlider);
	_greenSlider->redraw();

	// Calculate Blue Slider Dimensions
	buttonRect.y = buttonRect.y + buttonRect.height + 1;

	// Create Blue Slider
	_blueSlider = new SliderHorizontal(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);

	_blueSlider->setMinimumValue(0);
	_blueSlider->setMaximumValue(31);
	_blueSlider->setValue(0);

	_blueSlider->addGadgetEventHandler(this);
	addGadget(_blueSlider);
	_blueSlider->redraw();

	// Calculate Colour button dimensions
	buttonRect.width = (rect.width >> 2) - 1;
	buttonRect.height = _redSlider->getHeight() * 3 + 2;
	buttonRect.x = rect.x + rect.width - buttonRect.width;
	buttonRect.y = rect.y;

	// Create Colour button
	_colourButton = new Button(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height,"");
	_colourButton->disable();

	addGadget(_colourButton);

	setColour(colour);
}

void ColourPicker::onResize(u16 width, u16 height) {

	// Call base class' method to ensure the basic window resizes correctly
	AmigaWindow::onResize(width, height);

	Rect rect;
	getClientRect(rect);

	// Calculate OK button dimensions
	Rect buttonRect;
	_okButton->getPreferredDimensions(buttonRect);

	buttonRect.width = (rect.width >> 1) - 1;
	buttonRect.x = rect.x;
	buttonRect.y = (rect.y + rect.height) - buttonRect.height;

	_okButton->changeDimensions(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);

	// Calculate cancel button dimensions
	buttonRect.x = rect.x + rect.width - buttonRect.width;
	buttonRect.y = (rect.y + rect.height) - buttonRect.height;

	_cancelButton->changeDimensions(buttonRect.x, buttonRect.y, buttonRect.width, buttonRect.height);
}

uint32 ColourPicker::getColour()
{
	return(_colourButton->getBackColour());
}

void ColourPicker::setColour(u16 colour)
{
	_colourButton->setBackColour(colour);

	_redSlider->setValue(colour & 31);
	_greenSlider->setValue((colour >> 5) & 31);
	_blueSlider->setValue((colour >> 10) & 31);
	_redSlider->redraw();
	_greenSlider->redraw();
	_blueSlider->redraw();
}

void ColourPicker::handleReleaseEvent(const GadgetEventArgs& e) {
	if (e.getSource() != NULL) {
		if (e.getSource() == _cancelButton) {

			// Close the window
			close();
			return;
		} else if (e.getSource() == _okButton) {

			// Raise value changed event to event handler
			_gadgetEventHandlers->raiseValueChangeEvent();

			// Close the window
			close();
			return;
		}
	}
	
	AmigaWindow::handleReleaseEvent(e);
}

void ColourPicker::handleValueChangeEvent(const GadgetEventArgs& e) {
	if (e.getSource() != NULL) {
		_colourButton->setBackColour(woopsiRGB(_redSlider->getValue(), _greenSlider->getValue(), _blueSlider->getValue()));
	}
	_colourButton->setBackColour(woopsiRGB(_redSlider->getValue(), _greenSlider->getValue(), _blueSlider->getValue()));
	_colourButton->redraw();

	AmigaWindow::handleValueChangeEvent(e);
}
