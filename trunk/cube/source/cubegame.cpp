// Includes
#include "cubegame.h"
#include "screen.h"
#include "amigawindow.h"
#include "button.h"
#include "gadgeteventhandler.h"
#include "woopsifuncs.h"
#include "settingsscreen.h"
#include "settingsbutton.h"
#include "backbutton.h"
#include "sys/stat.h"
#include "dirent.h"
#include "fat.h"
#include "math.h"

int SLRotation;

void CubeGame::startup()
{
	//consoleDemoInit();
	//mainCube.Resize(2.5);

	//put 3D on touchscreen
	lcdMainOnTop();
	
	// Create screen
	Screen* screen = new Screen("Windowless Screen", Gadget::GADGET_DRAGGABLE);
	woopsiApplication->addGadget(screen);
	
	// Add my settings screen to woopsi
	Rect rect;
	screen->getClientRect(rect);
	
	_settingsscreen = new SettingsScreen(rect.x, rect.y, rect.width, rect.height, 0);
	_settingsscreen->_btnOk->addGadgetEventHandler(this);
	_settingsscreen->_btnCancel->addGadgetEventHandler(this);
	_settingsscreen->btnSave->addGadgetEventHandler(this);
	_settingsscreen->btnLoad->addGadgetEventHandler(this);
	_settingsscreen->btnReset->addGadgetEventHandler(this);
	_settingsscreen->btnScramble->addGadgetEventHandler(this);
	_settingsscreen->btnQScramble->addGadgetEventHandler(this);
	_settingsscreen->btnPlay->addGadgetEventHandler(this);
	_settingsscreen->btnQLoad->addGadgetEventHandler(this);
	_settingsscreen->btnCredits->addGadgetEventHandler(this);

	screen->addGadget(_settingsscreen);

	// Ensure Woopsi can draw itself
	enableDrawing();
	
	// Draw GUI
	redraw();

	// Disable while on top screen
	//woopsiApplication->disable();

	// Initialise states
	moving=false;
	twisting=false;
	settings=true;
	loading=false;
	saving=false;
	switching=false;
	SLRotation=0;
	
	//setup the sub screen for basic printing
	//consoleDemoInit();
	//printf("%s\n", arg);

	fatInitDefault();

	// Setup the Main screen for 3D
	//videoSetMode(MODE_0_3D);


	videoSetMode(MODE_5_3D | DISPLAY_BG0_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	//vramSetMainBanks(VRAM_A_MAIN_SPRITE,VRAM_B_LCD,VRAM_C_LCD,VRAM_D_LCD);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	//vramSetBankB(VRAM_B_LCD);
	bgSetPriority(0, 1);
	//setup sprites
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false); //initialize the oam
	gfxSettings = oamAllocateGfx(&oamMain, SpriteSize_64x32,SpriteColorFormat_256Color);//make room for the sprite
	dmaCopy(settingsbuttonTiles, gfxSettings, settingsbuttonTilesLen);//copy the sprite
	dmaCopy(settingsbuttonPal, SPRITE_PALETTE, settingsbuttonPalLen); //copy the sprites palette
	//gfxBack = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_256Color);
	//dmaCopy(backbuttonTiles, gfxBack, backbuttonTilesLen);
	//dmaCopy(backbuttonPal, SPRITE_PALETTE, backbuttonPalLen);
	oamEnable(&oamMain);


	// initialize gl
	glInit();

	// enable antialiasing
	glEnable(GL_ANTIALIAS);

	// setup the rear plane
	glClearColor(0,31,31,31); // BG must be opaque for AA to work
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);

	// Initialise variables
	dx=0; dy=0;
	oldXY.px = 45;
	oldXY.py = 45;
	touchVector.X=0;
	touchVector.Y=0;
	touchVector.Z=0;

	// Set our view port to be the same size as the screen
	glViewport(0,0,255,191);

	//change ortho vs perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 10);

	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT );
	// Set the current matrix to be the model matrix
	glMatrixMode(GL_MODELVIEW);

	for(int i=0; i<3; i++)
		cube[i].Reset();
	_loadSettings();

}

void CubeGame::_loadSettings()
{
	if (!opendir("/DATA/"))
		mkdir("/DATA", S_IRUSR || S_IWUSR || S_IROTH || S_IWOTH);

	FILE* settingsfile = fopen("/DATA/cube.dat", "rb");
	RubikSide tmpLayout[6];
	//Colour tmpPalette[6];

	if(settingsfile==NULL)
	{
		fclose(settingsfile);
		_saveSettings();
		settingsfile = fopen("/DATA/cube.dat", "rb");
	}

	fread((void*)&_settingsscreen->settings, sizeof(_settingsscreen->settings), 1, settingsfile);
	for(int i=0; i<3; i++)
	{
		fread((void*)&tmpLayout, sizeof(tmpLayout), 1, settingsfile);
		cube[i].setLayout(tmpLayout);

		//fread((void*)&tmpPalette, sizeof(tmpPalette), 1, settingsfile);
		//cube[i].setPalette(tmpPalette);
	}

	fclose(settingsfile);

	_applySettings();
	_settingsscreen->revertSettings();

}

void CubeGame::_saveSettings()
{
	RubikSide tmpLayout[6];
	//Colour tmpPalette[6];

	FILE* settingsfile=fopen("/DATA/cube.dat", "wb");

	fwrite((void*)&_settingsscreen->settings, sizeof(_settingsscreen->settings), 1, settingsfile);
	for(int i=0; i<3; i++)
	{
		cube[i].getLayout(tmpLayout);
		fwrite((void*)&tmpLayout, sizeof(tmpLayout), 1, settingsfile);

		//cube[i].getPalette(tmpPalette);
		//fwrite((void*)&tmpPalette, sizeof(tmpPalette), 1, settingsfile);
	}
	fclose(settingsfile);
}

void CubeGame::_loadCube(int cuben)
{
	RubikSide tmpLayout[6];
	Colour tmpPalette[6];
	cube[cuben].getLayout(tmpLayout);
	cube[cuben].getPalette(tmpPalette);
	mainCube.setLayout(tmpLayout);
	mainCube.setPalette(tmpPalette);
	loading=false;
}

void CubeGame::_saveCube(int cuben)
{
	cube[cuben]=mainCube;
	//cube[cuben-1].Move(45, 45);

	Settings tmpSettings;
	RubikSide tmpLayout[6];
	//Colour tmpPalette[6];
	FILE* savefile;

	savefile = fopen("/DATA/cube.dat", "rb");
	fread((void*)&tmpSettings, sizeof(tmpSettings), 1, savefile);
	fclose(savefile);

	savefile=fopen("/DATA/cube.dat", "wb");
	fwrite((void*)&tmpSettings, sizeof(tmpSettings), 1, savefile);
	for(int i=0; i<3; i++)
	{
		cube[i].getLayout(tmpLayout);
		fwrite((void*)&tmpLayout, sizeof(tmpLayout), 1, savefile);

		//cube[i].getPalette(tmpPalette);
		//fwrite((void*)&tmpPalette, sizeof(tmpPalette), 1, savefile);
	}
	fclose(savefile);

	saving=false;
}


//----------------------------------------------------------------
// The main loop of the game - called (hopefully) once per vblank
//----------------------------------------------------------------
void CubeGame::_run()
{
	// Reset movement states
	twisting=FALSE;
	moving=FALSE;

	// Scankeys already called by woopsi
	int down = keysDown();
	int held = keysHeld();

	if(!settings)
	{
		// Reset x and y when user touches screen
		if(down & KEY_TOUCH)
		{
			touchRead(&oldXY);
			
			// If player touches the top-right poly, take them to settings
			if((oldXY.px >=192) && (oldXY.py <=32))
			{
				settings=!settings;
				_switchScreens();
				_drawShit();
				return;
			}
		}

		// Some simple tests using face buttons
		if(down & KEY_X)
		{
			printf("about to test solve...\n");
			mainCube.Solve();
		}
		if(down & KEY_Y)
		{
			mainCube.Scramble();
		}
		if(down & KEY_A)
		{
			mainCube.setControlStyle(0);
		}
		if(down & KEY_B)
		{
			mainCube.setControlStyle(1);
		}

		//if user drags then grab the delta
		if( held & KEY_TOUCH)
		{
			// take a new reading
			touchRead(&touchXY);
			// find the difference between old and new values
			dx = touchXY.px - oldXY.px;
			dy = touchXY.py - oldXY.py;
			// make the new value the old value
			oldXY=touchXY;
			
			// see if the user is trying to twist
			if((held & KEY_L)||(held & KEY_R))
			{
				twisting=TRUE;
			}else{
				if(dx || dy)
					moving=TRUE;
			}

		}else{
			dx=0;
			dy=0;
		}
	}else{
		dx=0;
		dy=0;
	}
	
	// create a vector from stylus input to pass to the cube
	touchVector.X=dx;
	touchVector.Y=-dy;

	// Move the cube if necessary
	if(moving) mainCube.Move(dx, dy);

	// Draw GL stuff
	_drawShit();

}

//-------------------------------------------------------------
// just a test
//-------------------------------------------------------------
void CubeGame::_test()
{
	//printf("%i\n", woopsiApplication->getChildCount());
	//woopsiApplication->getChild(0).hide();
	//woopsiApplication->screen->btn_Return->moveTo(176, 155);
}

//-----------------------------------------------------------------------------
// Calculates and draws all the 3d stuff, as well as asking the cube to draw
// itself as well.
//-----------------------------------------------------------------------------
void CubeGame::_drawShit()
{
	glLoadIdentity();

	gluLookAt(	0.0, 0.0, 3.5,		//camera position
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up

	// Update and draw the cube
	if(!settings)
	{
		mainCube.Update(twisting, touchXY, touchVector);
		oamSet(&oamMain,0,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxSettings,0,false,false,false,false,false);
		//oamSet(&oamMain,0,256,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,false,false,false,false);


	}else{
		if(!saving && !loading)
		{
			mainCube.Update(twisting, touchXY, touchVector);
			//oamSet(&oamMain,0,256,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,false,false,false,false);
		}else{
			//oamSet(&oamMain,0,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,false,false,false,false);

			SLRotation+=100;
			//SLRotation%=360;

			glTranslate3f32(inttof32(-2), 0, 0);
			glRotateYi(SLRotation);
			cube[0].Resize(0.9);
			//cube[0].Move(-1,0);
			cube[0].Update(twisting, touchXY, touchVector);

			glLoadIdentity();

			gluLookAt(	0.0, 0.0, 3.5,		//camera position
						0.0, 0.0, 0.0,		//look at
						0.0, 1.0, 0.0);		//up

			glRotateYi(SLRotation);
			cube[1].Resize(1);
			//cube[1].Move(-1,0);
			cube[1].Update(twisting, touchXY, touchVector);

			glLoadIdentity();

			gluLookAt(	0.0, 0.0, 3.5,		//camera position
						0.0, 0.0, 0.0,		//look at
						0.0, 1.0, 0.0);		//up

			glTranslate3f32(inttof32(2), 0, 0);
			glRotateYi(SLRotation);
			cube[2].Resize(0.9);
			//cube[2].Move(-1,0);
			cube[2].Update(twisting, touchXY, touchVector);

			if(keysDown() & KEY_TOUCH)
			{
				touchPosition tmpXY;
				touchRead(&tmpXY);

				// If player touches the top-right poly, take them back to settings
				if((tmpXY.px >=192) && (tmpXY.py <=32))
				{
					loading=false;
					saving=false;
					_switchScreens();
					return;
				}

				int cubetmp=0;
				float disttmp;
				disttmp=sqrt((45-tmpXY.px)*(45-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py));
				if(sqrt((45-tmpXY.px)*(45-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=1;
				else if(sqrt((128-tmpXY.px)*(128-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=2;
				else if(sqrt((211-tmpXY.px)*(211-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=3;

				if(cubetmp)
				{
					if(saving)
					{
						_saveCube(cubetmp-1);
					}
					else
						_loadCube(cubetmp-1);
					_switchScreens();
					return;
				}
			}
		}


		oamSet(&oamMain,0,256,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxSettings,0,false,false,false,false,false);
	}

	oamUpdate(&oamMain);
}

void CubeGame::_applySettings()
{
	mainCube.setRotateSensitivity(_settingsscreen->settings.rotateSensitivity);
	mainCube.setTwistSensitivity(_settingsscreen->settings.twistSensitivity);
	mainCube.setControlStyle(_settingsscreen->settings.controlStyle);
	for(int i=0; i<6; i++)
	{
		mainCube.setColour(i, (_settingsscreen->settings.colour[i][0]+1)*8-1, (_settingsscreen->settings.colour[i][1]+1)*8-1, (_settingsscreen->settings.colour[i][2]+1)*8-1);
		for(int j=0; j<3; j++)
			cube[j].setColour(i, (_settingsscreen->settings.colour[i][0]+1)*8-1, (_settingsscreen->settings.colour[i][1]+1)*8-1, (_settingsscreen->settings.colour[i][2]+1)*8-1);
	}
}

//------------------------------------------------------------------------------
// Bring woopsi down to the touchscreen, and send the cube to the top screen (and vice versa)
//------------------------------------------------------------------------------
void CubeGame::_switchScreens()
{
	// Obviously...
	switching=true;
	
	// let the game know we're playing with the settings
	//settings=!settings;
	
	// enable or disable woopsi
	if(settings && !loading && !saving)
	{
		woopsiApplication->enable();
		//btn_Return->moveTo(176, 6);
	}else{
		woopsiApplication->disable();
		//btn_Return->moveTo(176, 155);
	}
}

//------------------------------------------------------------------------------
// Replacement for the normal processOneVBL
// exactly the same except for....
//------------------------------------------------------------------------------
void CubeGame::processOneVBL(Gadget* gadget)
{
	// ....this if-else bit, and....
	if(!settings)
		_run();
	else
		_drawShit();
	
	handleVBL();
	handleStylus(gadget);
	handleKeys();
	handleLid();
	// ...this:
	glFlush(0);
	//oamUpdate(&oamMain);
	woopsiWaitVBL();
	// and this:
	if(switching) lcdSwap();
	switching=false;
}

void CubeGame::handleActionEvent(const GadgetEventArgs& e)
{
	switch(e.getSource()->getRefcon())
	{
	case 10:
		saving=true;
		_switchScreens();
		break;
	case 11:
		loading=true;
		_switchScreens();
		break;
	case 12:
		mainCube.Reset();
		break;
	case 13:
		mainCube.Scramble();
		break;
	case 94: //Quick Scramble
		mainCube.Scramble();
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();
		settings=!settings;
		_switchScreens();
		break;
	case 95: //Play
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();
		settings=!settings;
		_switchScreens();
		break;
	case 96: //Quick Load;
		//Coming Soon!
		break;
	case 97: //Credits
		//Coming Soonish!
		break;
	case 98:
		_settingsscreen->revertSettings();
		settings=!settings;
		_switchScreens();
		break;
	case 99:
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();
		settings=!settings;
		_switchScreens();
		break;
	}
	//_switchScreens();
	printf("Something\n");
}


void CubeGame::shutdown() {

	// Call base shutdown method
	Woopsi::shutdown();
}
