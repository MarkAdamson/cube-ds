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

#include "woopsi.h"
#include "rubikscube.h"
#include "button.h"
#include "settingsscreen.h"


using namespace WoopsiUI;

// Current woopsi buttons
enum Btns {BTN_SCRAMBLE, BTN_RESET, BTN_RETURN};

// Woopsi application class for the game
class CubeGame : public Woopsi, public GadgetEventHandler {

private:
	// States for the game controls
	bool moving, twisting, settings, saving, loading, switching, painting;
	u16* gfxSettings;
	u16* gfxBack;
	
	// Variables for the game controls
	int dx, dy;
	touchPosition touchXY, oldXY;
	VECTOR touchVector;
	SettingsScreen* _settingsscreen;
	
	// The cube itself
	RubiksCube mainCube;
	RubiksCube cube[3];
	
	// The woopsi screen to draw to
	//SettingsScreen* _settingsscreen;
	// Woopsi buttons
	Button* btn_Return;
	
	// Modified to include glflush:
	void processOneVBL(Gadget*);
	
	// Main functions:
	void _test();
	void _switchScreens();
	void _drawPalette();
	void _drawShit();
	void _run();
	void _loadSettings();
	void _saveSettings();
	void _applySettings();
	void _loadCube(int cuben);
	void _saveCube(int cuben);
	
	// Gadget handler for Woopsi button presses
	void handleActionEvent(const WoopsiUI::GadgetEventArgs&);
	
public:
	void startup();
	void shutdown();
};
