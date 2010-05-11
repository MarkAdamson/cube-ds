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

#include <png.h>
#include "woopsi.h"
#include "rubikscube.h"
#include "button.h"
#include "scrollingpanel.h"
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
	u16* gfxUndo;
	u16* gfxRedo;
	u16* gfxCancel;
	u16* gfxOk;

	//background image
	rgb* backgroundTexData;
	bool showBackgroundImage;
	GL_TEXTURE_TYPE_ENUM glColourType;
	
	// Variables for the game controls
	int dx, dy;
	touchPosition touchXY, oldXY;
	VECTOR touchVector;
	SettingsScreen* _settingsscreen;
	
	// The woopsi screen to draw to
	//SettingsScreen* _settingsscreen;
	// Woopsi buttons
	Button* btn_Return;
	ScrollingPanel* titleScreen;
	
	// Modified to include glflush:
	void processOneVBL(Gadget*);
	
	// Main functions:
	void _initWoopsi();
	void _initSprites();
	void _initGL();
	void _test();
	void _switchScreens();
	void _drawPalette();
	void _showPainterGUI();
	void _hidePainterGUI();
	void _drawShit();
	void _run();
	void _loadSettings();
	void _saveSettings();
	void _applySettings();
	void _loadCube(int cuben);
	void _saveCube(int cuben);
	void _buildTitleScreen();
	bool _loadPNG(char* filename);
	//void* _pngEndOfRowCallback(png_struct*, png_uint_32, int);
	
	// Gadget handler for Woopsi button presses
	void handleActionEvent(const WoopsiUI::GadgetEventArgs&);
	void handleValueChangeEvent(const WoopsiUI::GadgetEventArgs&);
	
public:
	void startup();
	void shutdown();
};
