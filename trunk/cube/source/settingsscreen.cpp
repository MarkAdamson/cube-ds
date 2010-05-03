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
 
#include <stdio.h>
#include <stdlib.h>

#include "settingsscreen.h"
#include "tabscreen.h"
#include "image.h"
#include "mebmp.h"
#include "colourpicker.h"

#include "button.h"
#include "label.h"
#include "graphicsport.h"
#include "calendardaybutton.h"
#include "woopsistring.h"
#include "sliderhorizontal.h"
#include "radiobuttongroup.h"
#include "fonts/gulimche12.h"
#include "woopsifuncs.h"

using namespace WoopsiUI;

SettingsScreen::SettingsScreen(s16 x, s16 y, u16 width, u16 height, u32 flags, GadgetStyle* style) : Gadget(x, y, width, height, flags, style) {

	//_settings.size=2;
	settings.rotateSensitivity=20;
	settings.twistSensitivity=20;
	settings.controlStyle=0;
	setDefaultColours();
	
	
	setBorderless(true);
	//pages[numpages];
	
	buildGUI();
	revertSettings();
}

SettingsScreen::~SettingsScreen() {
}

void SettingsScreen::setDefaultColours()
{
	//Red: RGB
	settings.colour[0][0]=23;
	settings.colour[0][1]=0;
	settings.colour[0][2]=0;
	//Yellow:
	settings.colour[1][0]=31;
	settings.colour[1][1]=31;
	settings.colour[1][2]=0;
	//Green:
	settings.colour[2][0]=0;
	settings.colour[2][1]=15;
	settings.colour[2][2]=0;
	//Orange:
	settings.colour[3][0]=31;
	settings.colour[3][1]=15;
	settings.colour[3][2]=0;
	//White:
	settings.colour[4][0]=31;
	settings.colour[4][1]=31;
	settings.colour[4][2]=31;
	//Blue:
	settings.colour[5][0]=0;
	settings.colour[5][1]=0;
	settings.colour[5][2]=31;
	//Background:
	settings.bgColour[0]=0;
	settings.bgColour[1]=31;
	settings.bgColour[2]=31;
}

void SettingsScreen::revertColours()
{
	for(int i=0; i<6; i++)
	{
		btnColour[i]->setBackColour(woopsiRGB(settings.colour[i][0], settings.colour[i][1], settings.colour[i][2]));
	}
	btnBackgroundColour->setBackColour(woopsiRGB(settings.bgColour[0], settings.bgColour[1], settings.bgColour[2]));
}

void SettingsScreen::updateColours()
{
	for(int i=0; i<6; i++)
	{
		u16 tmpColour = btnColour[i]->getBackColour();
		settings.colour[i][0]=(tmpColour & (u8)31);
		settings.colour[i][1]=((tmpColour >> 5) & (u8)31);
		settings.colour[i][2]=((tmpColour >> 10) & (u8)31);
	}
	u16 tmpColour = btnBackgroundColour->getBackColour();
	settings.bgColour[0]=(tmpColour & (u8)31);
	settings.bgColour[1]=((tmpColour >> 5) & (u8)31);
	settings.bgColour[2]=((tmpColour >> 10) & (u8)31);
}

void SettingsScreen::revertSettings()
{
	sldRotateSensitivity->setValue(settings.rotateSensitivity-10);
	sldTwistSensitivity->setValue(settings.twistSensitivity-10);
	rbgControlStyle->setSelectedIndex(settings.controlStyle);
	revertColours();
}

void SettingsScreen::updateSettings()
{
	settings.rotateSensitivity=sldRotateSensitivity->getValue()+10;
	settings.twistSensitivity=sldTwistSensitivity->getValue()+10;
	settings.controlStyle=rbgControlStyle->getSelectedIndex();
	updateColours();
}

void SettingsScreen::drawContents(GraphicsPort* port) {
	port->drawFilledRect(0, 0, _width, _height, getBackColour());
	
	/*Rect rect;
	getClientRect(rect);
	rect.y+=20;
	rect.height-=42;
	if(!isBorderless())
	{
		//rect.x+=_borderSize.left;
		//rect.y+=_borderSize.top;
		rect.width-=_borderSize.right  + 1;
		rect.height-=_borderSize.bottom;
	}*/
}

void SettingsScreen::drawBorder(GraphicsPort* port) {

	// Stop drawing if the gadget indicates it should not have an outline
	if (isBorderless()) return;

	port->drawBevelledRect(0, 0, _width, _height, getShineColour(), getShadowColour());
}

void SettingsScreen::buildGUI() {

	Rect rect;
	getClientRect(rect);

	// Add Ok and Cancel buttons
	_btnOk = new Button(rect.width - 61, rect.height - 21, 60, 20, "Ok");
	_btnOk->setRefcon(99);
	addGadget(_btnOk);
	_btnCancel = new Button(rect.width - 122, rect.height - 21, 60, 20, "Cancel");
	_btnCancel->setRefcon(98);
	addGadget(_btnCancel);
	
	pageRect=rect;
	pageRect.height-=20;
	
	_pages = new TabScreen(pageRect.x+1, pageRect.y+1, pageRect.width-2, pageRect.height-3, 6, 0);
	WoopsiString title[6] = {"Game", "Control", "Backgrd", "Credits", "Modify Cube", "Solve Cube"};
	for(int i=0; i<6; i++)
	{
		_pages->nameTab(i, title[i]);
	}
	
	buildGamePage();
	buildControlPage();
	buildBackgroundPage();
	buildCreditsPage();
	buildSolvePage();
	buildPainterPage();
	
	addGadget(_pages);
}


void SettingsScreen::buildGamePage()
{
	Label* label = new Label(0, 0, 254, 20, "GAME OPTIONS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(0, label);

	//Save Button
	btnSave = new Button(22, 35, 70, 30, "SAVE");
	btnSave->setRefcon(10);
	_pages->addGadgetToPage(0, btnSave);
	
	btnLoad = new Button(92, 35, 70, 30, "LOAD");
	btnLoad->setRefcon(11);
	_pages->addGadgetToPage(0, btnLoad);
	
	btnReset = new Button(162, 35, 70, 30, "RESET");
	btnReset->setRefcon(12);
	_pages->addGadgetToPage(0, btnReset);
	
	btnScramble = new Button(92, 65, 70, 30, "SCRAMBLE");
	btnScramble->setRefcon(13);
	_pages->addGadgetToPage(0, btnScramble);
}

void SettingsScreen::buildControlPage()
{
	Label* label = new Label(0, 0, 254, 20, "CONTROL SETTINGS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(1, label);

	label = new Label(20, 32, 150, 14, "Movement Sensitivity:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(TEXT_ALIGNMENT_HORIZ_LEFT);
	_pages->addGadgetToPage(1, label);

	sldRotateSensitivity = new SliderHorizontal(20, 46, 150, 10);
	sldRotateSensitivity->setMinimumValue(0);
	sldRotateSensitivity->setMaximumValue(40);
	sldRotateSensitivity->setValue(10);
	sldRotateSensitivity->setRefcon(20);
	sldRotateSensitivity->addGadgetEventHandler(this);
	_pages->addGadgetToPage(1, sldRotateSensitivity);

	label = new Label(20, 68, 150, 14, "Twisting Sensitivity:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(1, label);

	sldTwistSensitivity = new SliderHorizontal(20, 82, 150, 10);
	sldTwistSensitivity->setMinimumValue(0);
	sldTwistSensitivity->setMaximumValue(40);
	sldTwistSensitivity->setValue(10);
	sldTwistSensitivity->setRefcon(21);
	sldTwistSensitivity->addGadgetEventHandler(this);
	_pages->addGadgetToPage(1, sldTwistSensitivity);

	label = new Label(180, 32, 64, 14, "Control");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(1, label);
	label = new Label(180, 46, 64, 14, "Style:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(1, label);
	label = new Label(205, 61, 14, 14, "A");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(1, label);
	label = new Label(205, 77, 14, 14, "B");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(1, label);

	rbgControlStyle = new RadioButtonGroup(190, 60);
	rbgControlStyle->newRadioButton(0, 0, 10, 10);
	rbgControlStyle->newRadioButton(0, 16, 10, 10);
	rbgControlStyle->setSelectedIndex(0);
	rbgControlStyle->setRefcon(22);
	rbgControlStyle->addGadgetEventHandler(this);
	_pages->addGadgetToPage(1, rbgControlStyle);
}


void SettingsScreen::buildBackgroundPage()
{
	Label* label = new Label(0, 0, 254, 20, "BACKGROUND SETTINGS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(2, label);

	RadioButtonGroup* rbgBackgroundType = new RadioButtonGroup(10, 30);
	rbgBackgroundType->newRadioButton(0, 0, 8, 8);
	rbgBackgroundType->newRadioButton(0, 14, 8, 8);
	rbgBackgroundType->setSelectedIndex(0);
	rbgBackgroundType->setRefcon(50);
	rbgBackgroundType->addGadgetEventHandler(this);
	_pages->addGadgetToPage(2, rbgBackgroundType);

	label = new Label(25, 30, 50, 14, "Colour:");
	label->setBorderless(true);
	_pages->addGadgetToPage(2, label);
	label = new Label(25, 44, 50, 14, "File:");
	label->setBorderless(true);
	_pages->addGadgetToPage(2, label);

	btnBackgroundColour = new Button(75, 27, 30, 14, "");
	btnBackgroundColour->setBackColour(woopsiRGB(settings.bgColour[0], settings.bgColour[1], settings.bgColour[2]));
	btnBackgroundColour->setRefcon(51);
	btnBackgroundColour->addGadgetEventHandler(this);
	_pages->addGadgetToPage(2, btnBackgroundColour);
}

void SettingsScreen::buildCreditsPage()
{
	Label* label = new Label(0, 0, 254, 20, "CREDITS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(3, label);
}


void SettingsScreen::buildPainterPage()
{
	Label* label = new Label(0, 0, 254, 20, "CUBE LAYOUT EDITOR");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(4, label);

	int width = _pages->getPageWidth();
	int offset = (width - 130)/2;

	for(u8 i=0; i<6; i++)
	{
		btnColour[i] = new Button((i%3)*20+offset, (i/3)*20+30, 20, 20, "");
		btnColour[i]->setBackColour(woopsiRGB(settings.colour[i][0], settings.colour[i][1], settings.colour[i][2]));
		btnColour[i]->setRefcon((4*10)+i);
		btnColour[i]->addGadgetEventHandler(this);
		_pages->addGadgetToPage(4, btnColour[i]);
	}

	Button* button = new Button(width-60-offset, 30, 60, 20, "Reset");
	button->setRefcon(46);
	button->addGadgetEventHandler(this);
	_pages->addGadgetToPage(4, button);

	btnApplyColours = new Button(width-60-offset, 50, 60, 20, "Apply");
	btnApplyColours->setRefcon(47);
	_pages->addGadgetToPage(4, btnApplyColours);

	btnPaint = new Button(width/2 - 30, 80, 60, 30, "Paint");
	btnPaint->setRefcon(48);
	_pages->addGadgetToPage(4, btnPaint);

	/*btnColour[0] = new Button(0, 0, 20, 20, "");
	btnColour[0]->setBackColour(woopsiRGB(settings.colour[0][0], settings.colour[0][1], settings.colour[0][2]));
	btnColour[0]->setRefcon(40);
	_pages->addGadgetToPage(4, btnColour[0]);*/
}

void SettingsScreen::buildSolvePage()
{
	Label* label = new Label(0, 0, 254, 20, "CUBE SOLVER");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(5, label);

	//label = new Label(0, 0, _pages->getPageWidth(), _pages->getPageHeight(), "Solve Page - Under Construction");
	//label->setBorderless(true);
	//_pages->addGadgetToPage(5, label);
}


void SettingsScreen::onResize(u16 width, u16 height) {

	// Get a rect describing the gadget
	Rect rect;
	getClientRect(rect);
}

// Get the preferred dimensions of the gadget
void SettingsScreen::getPreferredDimensions(Rect& rect) const {
	rect.x = _x;
	rect.y = _y;
	rect.width = 0;
	rect.height = 0;

	if (!_flags.borderless) {
		//rect.width = _borderSize.left + _borderSize.right;
		//rect.height = _borderSize.top + _borderSize.bottom;
	}

	rect.width += 192;
	rect.height += 256;
}


void SettingsScreen::handleActionEvent(const GadgetEventArgs& e)
{
	int refcon=e.getSource()->getRefcon();
	
	if(refcon>=40 && refcon<46)
	{
		ColourPicker* pick = new ColourPicker(27, 55, 200, 80, "Colour Picker", btnColour[refcon-40]->getBackColour(), GADGET_DRAGGABLE);
		pick->setRefcon(refcon-40);
		pick->addGadgetEventHandler(this);
		addGadget(pick);
		pick->goModal();
		//pick->redraw();
	}
	if(refcon==46)
	{
		setDefaultColours();
		for(int i=0; i<6; i++)
			btnColour[i]->setBackColour(woopsiRGB(settings.colour[i][0], settings.colour[i][1], settings.colour[i][2]));
		redraw();
	}
	if(refcon==51)
	{
		ColourPicker* pick = new ColourPicker(27, 55, 200, 80, "Choose Background Colour", e.getSource()->getBackColour(), GADGET_DRAGGABLE);
		pick->setRefcon(refcon);
		pick->addGadgetEventHandler(this);
		addGadget(pick);
		pick->goModal();
	}
}

void SettingsScreen::handleValueChangeEvent(const GadgetEventArgs& e)
{
	if(e.getSource() !=NULL)
	{
		int refcon=e.getSource()->getRefcon();
		if(refcon<6)
			btnColour[refcon]->setBackColour(((ColourPicker*)e.getSource())->getColour());
		if(refcon==51)
			btnBackgroundColour->setBackColour(((ColourPicker*)e.getSource())->getColour());
	}
}
