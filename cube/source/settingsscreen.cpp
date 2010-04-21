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
}

void SettingsScreen::revertSettings()
{
	sldRotateSensitivity->setValue(settings.rotateSensitivity-10);
	sldTwistSensitivity->setValue(settings.twistSensitivity-10);
	rbgControlStyle->setSelectedIndex(settings.controlStyle);
	for(int i=0; i<6; i++)
	{
		btnColour[i]->setBackColour(woopsiRGB(settings.colour[i][0], settings.colour[i][1], settings.colour[i][2]));
	}
}

void SettingsScreen::updateSettings()
{
	settings.rotateSensitivity=sldRotateSensitivity->getValue()+10;
	settings.twistSensitivity=sldTwistSensitivity->getValue()+10;
	settings.controlStyle=rbgControlStyle->getSelectedIndex();
	for(int i=0; i<6; i++)
	{
		u16 tmpColour = btnColour[i]->getBackColour();
		settings.colour[i][0]=(tmpColour & (u8)31);
		settings.colour[i][1]=((tmpColour >> 5) & (u8)31);
		settings.colour[i][2]=((tmpColour >> 10) & (u8)31);
	}
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
	WoopsiString title[6] = {"Main", "Game", "Control", "Solve", "Painter", "Backgrd"};
	for(int i=0; i<6; i++)
	{
		_pages->nameTab(i, title[i]);
	}
	
	buildMainPage();
	buildGamePage();
	buildControlPage();
	buildSolvePage();
	buildPainterPage();
	buildBackgroundPage();
	
	addGadget(_pages);
}

void SettingsScreen::buildMainPage()
{
	Label* label = new Label(0, 0, 254, 20, "KICK-ASS RUBIK'S CUBE FOR DS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(0, label);

	label = new Label(0, 20, _pages->getPageWidth(), 14, "v0.2 by Mark Adamson");
	label->setBorderless(true);
	_pages->addGadgetToPage(0, label);

	mebmp* bmwMe = new mebmp();
	Image* image = new Image((_pages->getPageWidth()-100)/2, (_pages->getPageHeight()-80)/2+10, 100, 80, 0, 0, bmwMe, GADGET_BORDERLESS);
	_pages->addGadgetToPage(0, image);

	label = new Label(0, _pages->getPageHeight()/2, (_pages->getPageWidth()-100)/2, 14, "Created by");
	label->setBorderless(true);
	_pages->addGadgetToPage(0, label);

	label = new Label(0, _pages->getPageHeight()/2+14, (_pages->getPageWidth()-100)/2, 14, "This Guy-->");
	label->setBorderless(true);
	_pages->addGadgetToPage(0, label);

	label = new Label((_pages->getPageWidth()-100)/2+100, _pages->getPageHeight()/2, (_pages->getPageWidth()-100)/2, 14, "Inspired by");
	label->setBorderless(true);
	_pages->addGadgetToPage(0, label);

	label = new Label((_pages->getPageWidth()-100)/2+100, _pages->getPageHeight()/2+14, (_pages->getPageWidth()-100)/2, 14, "<--This Guy");
	label->setBorderless(true);
	_pages->addGadgetToPage(0, label);


	btnQScramble = new Button(3, (_pages->getPageHeight()-80)/2+10, 70, 25, "SCRAMBLE");
	btnQScramble->setRefcon(94);
	_pages->addGadgetToPage(0, btnQScramble);

	btnPlay = new Button(_pages->getPageWidth()-73, (_pages->getPageHeight()-80)/2+10, 70, 25, "PLAY");
	btnPlay->setRefcon(95);
	_pages->addGadgetToPage(0, btnPlay);

	btnQLoad = new Button(8, _pages->getPageHeight()-28, 60, 20, "Load");
	btnQLoad->setRefcon(96);
	_pages->addGadgetToPage(0, btnQLoad);

	btnCredits = new Button(_pages->getPageWidth()-68, _pages->getPageHeight()-28, 60, 20, "Credits");
	btnCredits->setRefcon(97);
	_pages->addGadgetToPage(0, btnCredits);
}


void SettingsScreen::buildGamePage()
{
	Label* label = new Label(0, 0, 254, 20, "GAME OPTIONS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(1, label);

	//Save Button
	btnSave = new Button(22, 35, 70, 30, "SAVE");
	btnSave->setRefcon(10);
	_pages->addGadgetToPage(1, btnSave);
	
	btnLoad = new Button(92, 35, 70, 30, "LOAD");
	btnLoad->setRefcon(11);
	_pages->addGadgetToPage(1, btnLoad);
	
	btnReset = new Button(162, 35, 70, 30, "RESET");
	btnReset->setRefcon(12);
	_pages->addGadgetToPage(1, btnReset);
	
	btnScramble = new Button(92, 65, 70, 30, "SCRAMBLE");
	btnScramble->setRefcon(13);
	_pages->addGadgetToPage(1, btnScramble);
}

void SettingsScreen::buildControlPage()
{
	Label* label = new Label(0, 0, 254, 20, "CONTROL SETTINGS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	_pages->addGadgetToPage(2, label);

	label = new Label(20, 32, 150, 14, "Movement Sensitivity:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(TEXT_ALIGNMENT_HORIZ_LEFT);
	_pages->addGadgetToPage(2, label);

	sldRotateSensitivity = new SliderHorizontal(20, 46, 150, 10);
	sldRotateSensitivity->setMinimumValue(0);
	sldRotateSensitivity->setMaximumValue(40);
	sldRotateSensitivity->setValue(10);
	sldRotateSensitivity->setRefcon(20);
	sldRotateSensitivity->addGadgetEventHandler(this);
	_pages->addGadgetToPage(2, sldRotateSensitivity);

	label = new Label(20, 68, 150, 14, "Twisting Sensitivity:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(2, label);

	sldTwistSensitivity = new SliderHorizontal(20, 82, 150, 10);
	sldTwistSensitivity->setMinimumValue(0);
	sldTwistSensitivity->setMaximumValue(40);
	sldTwistSensitivity->setValue(10);
	sldTwistSensitivity->setRefcon(21);
	sldTwistSensitivity->addGadgetEventHandler(this);
	_pages->addGadgetToPage(2, sldTwistSensitivity);

	label = new Label(180, 32, 64, 14, "Control");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(2, label);
	label = new Label(180, 46, 64, 14, "Style:");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(2, label);
	label = new Label(205, 61, 14, 14, "A");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(2, label);
	label = new Label(205, 77, 14, 14, "B");
	label->setBorderless(true);
	//label->setTextAlignmentHoriz(1);
	_pages->addGadgetToPage(2, label);

	rbgControlStyle = new RadioButtonGroup(190, 60);
	rbgControlStyle->newRadioButton(0, 0, 10, 10);
	rbgControlStyle->newRadioButton(0, 16, 10, 10);
	rbgControlStyle->setSelectedIndex(0);
	rbgControlStyle->setRefcon(22);
	rbgControlStyle->addGadgetEventHandler(this);
	_pages->addGadgetToPage(2, rbgControlStyle);
}

void SettingsScreen::buildSolvePage()
{
Label* label = new Label(0, 0, _pages->getPageWidth(), _pages->getPageHeight(), "Solve Page - Under Construction");
label->setBorderless(true);
_pages->addGadgetToPage(3, label);
}


void SettingsScreen::buildPainterPage()
{
	//Label* label = new Label(0, 0, _pages->getPageWidth(), _pages->getPageHeight(), "Painter Page - Under Construction");
	//label->setBorderless(true);
	//_pages->addGadgetToPage(4, label);

	for(u8 i=0; i<6; i++)
	{
		btnColour[i] = new Button((i%3)*20+30, (i/3)*20+30, 20, 20, "");
		btnColour[i]->setBackColour(woopsiRGB(settings.colour[i][0], settings.colour[i][1], settings.colour[i][2]));
		btnColour[i]->setRefcon((4*10)+i);
		btnColour[i]->addGadgetEventHandler(this);
		_pages->addGadgetToPage(4, btnColour[i]);
	}

	/*btnColour[0] = new Button(0, 0, 20, 20, "");
	btnColour[0]->setBackColour(woopsiRGB(settings.colour[0][0], settings.colour[0][1], settings.colour[0][2]));
	btnColour[0]->setRefcon(40);
	_pages->addGadgetToPage(4, btnColour[0]);*/
}


void SettingsScreen::buildBackgroundPage()
{
Label* label = new Label(0, 0, _pages->getPageWidth(), _pages->getPageHeight(), "Background Page - Under Construction");
label->setBorderless(true);
_pages->addGadgetToPage(5, label);
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
	
	if(refcon>=40)
	{
		ColourPicker* pick = new ColourPicker(27, 55, 200, 80, "Colour Picker", btnColour[refcon-40]->getBackColour(), GADGET_DRAGGABLE);
		pick->setRefcon(refcon-40);
		pick->addGadgetEventHandler(this);
		addGadget(pick);
		pick->goModal();
		//pick->redraw();
	}
}

void SettingsScreen::handleValueChangeEvent(const GadgetEventArgs& e)
{
	if(e.getSource() !=NULL)
	{
		int refcon=e.getSource()->getRefcon();
		if(refcon<6)
			btnColour[refcon]->setBackColour(((ColourPicker*)e.getSource())->getColour());
	}
}
