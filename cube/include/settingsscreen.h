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
 
#ifndef _SETTINGSSCREEN_H_
#define _SETTINGSSCREEN_H_

#include <nds.h>
#include "gadget.h"
#include "gadgetstyle.h"
#include "gadgeteventhandler.h"
#include "woopsistring.h"

enum SS_Pages {SS_TITLE, SS_GAME, SS_CONTROL, SS_SOLVE, SS_BACKGROUND, SS_PAINTER};

	typedef struct
	{
		//int size;
		int rotateSensitivity;
		int twistSensitivity;
		int controlStyle;
		int colour[6][3];
	}Settings;

namespace WoopsiUI {

	class Button;
	class SliderHorizontal;
	class RadioButtonGroup;
	class TabScreen;
	class ColourPicker;

	/**
	 * Class providing a calendar/date picker.  Raises an EVENT_ACTION event
	 * when a new date is clicked.
	 */
	class SettingsScreen : public Gadget, public GadgetEventHandler {
	public:

		/**
		 * Constructor.
		 * @param x The x co-ordinate of the calendar.
		 * @param y The y co-ordinate of the calendar.
		 * @param width The width of the calendar.
		 * @param height The height of the calendar.
		 * @param day The default day to display.
		 * @param month The default month to display.
		 * @param year The default year to display.
		 * @param flags Standard flags.
		 * @param style The style that the gadget should use.  If this is not
		 * specified, the gadget will use the values stored in the global
		 * defaultGadgetStyle object.  The gadget will copy the properties of
		 * the style into its own internal style object.
		 */
		SettingsScreen(s16 x, s16 y, u16 width, u16 height, u32 flags, GadgetStyle* style = NULL);

		/**
		 * Handles events raised by its sub-gadgets.
		 * @param e Event data to process.
		 */
		//virtual void handleReleaseEvent(const GadgetEventArgs& e);

		/**
		 * Insert the dimensions that this gadget wants to have into the rect
		 * passed in as a parameter.  All co-ordinates are relative to the gadget's
		 * parent.
		 * @param rect Reference to a rect to populate with data.
		 */
		virtual void getPreferredDimensions(Rect& rect) const;
		
		virtual void revertSettings();
		
		virtual void updateSettings();
		
		Button* _btnOk;						/**< Pointer to the left arrow */
		Button* _btnCancel;
		Button* btnScramble;
		Button* btnReset;
		Button* btnSave;
		Button* btnLoad;
		Button* btnPlay;
		Button* btnQLoad;
		Button* btnQScramble;
		Button* btnCredits;
		Button* btnColour[6];
		Button* btnPaint;
		TabScreen* _pages;
		WoopsiString strPage[6];
		Settings settings;

	protected:

		Rect pageRect;
		SliderHorizontal* sldRotateSensitivity;
		SliderHorizontal* sldTwistSensitivity;
		RadioButtonGroup* rbgControlStyle;
		
		/**
		 * Draw the area of this gadget that falls within the clipping region.
		 * Called by the redraw() function to draw all visible regions.
		 * @param port The GraphicsPort to draw to.
		 * @see redraw()
		 */
		virtual void drawContents(GraphicsPort* port);

		/**
		 * Draw the area of this gadget that falls within the clipping region.
		 * Called by the redraw() function to draw all visible regions.
		 * @param port The GraphicsPort to draw to.
		 * @see redraw()
		 */
		virtual void drawBorder(GraphicsPort* port);

		/**
		 * Resize the calendar to the new dimensions.
		 * @param width The new width.
		 * @param height The new height.
		 */
		void onResize(u16 width, u16 height);

		/**
		 * Destructor.
		 */
		virtual ~SettingsScreen();

		void setDefaultColours();

		/**
		 * Creates all gadgets but leaves them textless.
		 */
		void buildGUI();
		
		void buildMainPage();
		void buildGamePage();
		void buildControlPage();
		void buildSolvePage();
		void buildPainterPage();
		void buildBackgroundPage();
		
		// Gadget handler for Woopsi button presses
		void handleActionEvent(const WoopsiUI::GadgetEventArgs&);

		// Gadget handler for Woopsi button presses
		void handleValueChangeEvent(const WoopsiUI::GadgetEventArgs&);

		/**
		 * Copy constructor is protected to prevent usage.
		 */
		inline SettingsScreen(const SettingsScreen& calendar) : Gadget(calendar) { };
	};
}

#endif
