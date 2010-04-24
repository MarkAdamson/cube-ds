
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
