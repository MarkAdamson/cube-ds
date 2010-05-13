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

#include <malloc.h>

#include "cubegame.h"

#include "screen.h"
#include "amigawindow.h"
#include "button.h"
#include "scrollingpanel.h"
#include "textbox.h"
#include "fonts/gulimche12.h"
#include "gadgeteventhandler.h"
#include "woopsifuncs.h"

#include "settingsscreen.h"
#include "settingsbutton.h"
#include "backbutton.h"
#include "undo.h"
#include "redo.h"
#include "cancel.h"
#include "ok.h"
#include "image.h"
#include "mebmp.h"

#include "sys/stat.h"
#include "dirent.h"
#include "fat.h"
#include "math.h"

RubiksCube mainCube;

int SLRotation;
int paintColour;
RubikSide oldLayout[6];

RubiksCube SLCube;
RubikSide SLLayout[3][6];

int textureID;

#define PNG_BYTES_TO_CHECK 8

bool check_if_png(FILE* fp)
{
	png_byte buf[PNG_BYTES_TO_CHECK];

	/* Read in the signature bytes */
	//if (fread(&buf, PNG_BYTES_TO_CHECK, 1, fp) != PNG_BYTES_TO_CHECK)
	//	return 0;
	fread(&buf, PNG_BYTES_TO_CHECK, 1, fp);

	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature. */
	int isPng = png_sig_cmp(buf, 0, PNG_BYTES_TO_CHECK);
	return (isPng==0);
}

void CubeGame::_initWoopsi()
{
	
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
	_settingsscreen->btnPaint->addGadgetEventHandler(this);
	_settingsscreen->btnApplyColours->addGadgetEventHandler(this);
	_settingsscreen->tbxBackgroundImage->addGadgetEventHandler(this);

	_buildTitleScreen();
	//titleScreen->goModal();

	screen->addGadget(_settingsscreen);
	screen->addGadget(titleScreen);

	// Ensure Woopsi can draw itself
	enableDrawing();
	
	// Draw GUI
	redraw();
}
void CubeGame::_initSprites()
{
	
	//setup sprites
	oamInit(&oamMain, SpriteMapping_Bmp_1D_128, false); //initialize the oam

	gfxSettings = oamAllocateGfx(&oamMain, SpriteSize_64x32,SpriteColorFormat_256Color);//make room for the sprite
	dmaCopy(settingsbuttonTiles, gfxSettings, settingsbuttonTilesLen);//copy the sprite
	dmaCopy(settingsbuttonPal, SPRITE_PALETTE, settingsbuttonPalLen); //copy the sprites palette

	gfxBack = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_256Color);
	dmaCopy(backbuttonTiles, gfxBack, backbuttonTilesLen);

	gfxUndo = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	dmaCopy(undoTiles, gfxUndo, undoTilesLen);

	gfxRedo = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	dmaCopy(redoTiles, gfxRedo, redoTilesLen);

	gfxCancel = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_256Color);
	dmaCopy(cancelTiles, gfxCancel, cancelTilesLen);

	gfxOk = oamAllocateGfx(&oamMain, SpriteSize_64x32, SpriteColorFormat_256Color);
	dmaCopy(okTiles, gfxOk, okTilesLen);

	oamEnable(&oamMain);
}
void CubeGame::_initGL()
{
	// initialize gl
	glInit();

	glEnable(GL_OUTLINE | GL_ANTIALIAS | GL_TEXTURE_2D);

	//set the first outline color to white
	glSetOutlineColor(0,RGB15(0,0,0));
	glSetOutlineColor(1,RGB15(31,31,31));

	// setup the rear plane
	glClearColor(0,31,31,31); // set BG
	glClearPolyID(63); // the BG and polygons will have the same ID unless a polygon is highlighted
	glClearDepth(0x7FFF);

	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);

	// Set our view port to be the same size as the screen
	glViewport(0,0,255,191);

	//change ortho vs perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 20);

	// Set the current matrix to be the model matrix
	glMatrixMode(GL_MODELVIEW);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(0));
}

void CubeGame::startup()
{
	videoSetMode(MODE_5_3D | DISPLAY_BG0_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	vramSetBankB(VRAM_B_TEXTURE);
	//vramSetBankD(VRAM_D_TEXTURE);
	bgSetPriority(0, 1);

	//consoleDemoInit();
	lcdMainOnTop();
	fatInitDefault();


	_initWoopsi();

	_initSprites();

	_initGL();

	// Initialise states
	moving=false;
	twisting=false;
	settings=true;
	loading=false;
	saving=false;
	switching=false;
	painting=false;
	showBackgroundImage=false;
	SLRotation=0;

	// Initialise variables
	dx=0; dy=0;
	oldXY.px = 45;
	oldXY.py = 45;
	touchVector.X=0;
	touchVector.Y=0;
	touchVector.Z=0;

	//Reset all cubes
	mainCube.Reset();
	SLCube.Reset();
	for(int i=0; i<3; i++)
	{
		mainCube.getLayout(SLLayout[i]);
	}

	_loadSettings();
}

void CubeGame::_buildTitleScreen()
{
	titleScreen = new ScrollingPanel(0, 0, 256, 192, GADGET_BORDERLESS);

	Label* label = new Label(0, 0, 254, 20, "KICK-ASS RUBIK'S CUBE FOR DS");
	label->setBorderless(true);
	label->setFont(new Gulimche12);
	titleScreen->addGadget(label);

	label = new Label(0, 20, 256, 14, "v0.7 by Mark Adamson");
	label->setBorderless(true);
	titleScreen->addGadget(label);

	mebmp* bmwMe = new mebmp();
	Image* image = new Image(78, 66, 100, 80, 0, 0, bmwMe, GADGET_BORDERLESS);
	titleScreen->addGadget(image);

	label = new Label(0, 96, 78, 14, "Created by");
	label->setBorderless(true);
	titleScreen->addGadget(label);

	label = new Label(0, 110, 78, 14, "This Guy-->");
	label->setBorderless(true);
	titleScreen->addGadget(label);

	label = new Label(178, 96, 78, 14, "Inspired by");
	label->setBorderless(true);
	titleScreen->addGadget(label);

	label = new Label(178, 110, 78, 14, "<--This Guy");
	label->setBorderless(true);
	titleScreen->addGadget(label);


	Button* button = new Button(3, 40, 70, 25, "SCRAMBLE");
	button->setRefcon(94);
	button->addGadgetEventHandler(this);
	titleScreen->addGadget(button);

	button = new Button(183, 40, 70, 25, "PLAY");
	button->setRefcon(95);
	button->addGadgetEventHandler(this);
	titleScreen->addGadget(button);

	button = new Button(8, 164, 60, 20, "Load");
	button->setRefcon(96);
	button->addGadgetEventHandler(this);
	titleScreen->addGadget(button);

	button = new Button(188, 164, 60, 20, "Settings");
	button->setRefcon(97);
	button->addGadgetEventHandler(this);
	titleScreen->addGadget(button);
}

void CubeGame::_loadSettings()
{
	if (!opendir("/DATA/"))
		mkdir("/DATA", S_IRUSR || S_IWUSR || S_IROTH || S_IWOTH);

	FILE* settingsfile = fopen("/DATA/cube.dat", "rb");
	RubikSide tmpLayout[6];

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
		//cube[i].setLayout(tmpLayout);
		for(int j=0; j<6; j++)
			SLLayout[i][j]=tmpLayout[j];
	}

	fclose(settingsfile);

	_applySettings();
	_settingsscreen->revertSettings();

}

void CubeGame::_saveSettings()
{
	RubikSide tmpLayout[6];

	FILE* settingsfile=fopen("/DATA/cube.dat", "wb");

	fwrite((void*)&_settingsscreen->settings, sizeof(_settingsscreen->settings), 1, settingsfile);
	for(int i=0; i<3; i++)
	{
		//cube[i].getLayout(tmpLayout);
		for(int j=0; j<6; j++)
			tmpLayout[j]=SLLayout[i][j];
		fwrite((void*)&tmpLayout, sizeof(tmpLayout), 1, settingsfile);
	}
	fclose(settingsfile);
}

void CubeGame::_loadCube(int cuben)
{
	//RubikSide tmpLayout[6];
	//cube[cuben].getLayout(tmpLayout);
	mainCube.setLayout(SLLayout[cuben]);
	loading=false;
}

void CubeGame::_saveCube(int cuben)
{
	Settings tmpSettings;
	RubikSide tmpLayout[6];

	mainCube.getLayout(SLLayout[cuben]);
	//cube[cuben].setLayout(tmpLayout);

	FILE* savefile;

	savefile = fopen("/DATA/cube.dat", "rb");
	fread((void*)&tmpSettings, sizeof(tmpSettings), 1, savefile);
	fclose(savefile);

	savefile=fopen("/DATA/cube.dat", "wb");
	fwrite((void*)&tmpSettings, sizeof(tmpSettings), 1, savefile);
	for(int i=0; i<3; i++)
	{
		//cube[i].getLayout(tmpLayout);
		for(int j=0; j<6; j++)
			tmpLayout[j]=SLLayout[i][j];
		fwrite((void*)&tmpLayout, sizeof(tmpLayout), 1, savefile);
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
	bool palTouch = false;

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
			if(!painting)
			{
				if((oldXY.px >=192) && (oldXY.py <=32))
				{
					settings=true;
					_switchScreens();
					_drawShit();
					return;
				}
			}
			// During Painting, register palette touches
			else
			{
				if(oldXY.px>=6 && oldXY.px < 54 && oldXY.py >= 60 && oldXY.py < 132)
				{
					touchPosition tmpXY = oldXY;
					tmpXY.px -= 6;
					tmpXY.py -= 60;
					
					paintColour=tmpXY.py / 24;
					if(tmpXY.px >=24) paintColour += 3;
					palTouch=true;
				}

				if(oldXY.px>=10 && oldXY.px < 42 && oldXY.py >= 10 && oldXY.py < 42) //undo
				{
					mainCube.undoPaint();
					palTouch=true;
				}
				if(oldXY.px>=47 && oldXY.px < 79 && oldXY.py >= 10 && oldXY.py < 42) //redo
				{
					mainCube.redoPaint();
					palTouch=true;
				}
				if(oldXY.px < 60 && oldXY.py >= 172) //cancel
				{
					settings=true;
					painting=false;
					mainCube.setLayout(oldLayout);
					_switchScreens();
					_drawShit();
					_hidePainterGUI();
					return;
				}
				if(oldXY.px>=196 && oldXY.py >= 172) //ok
				{
					settings=true;
					painting=false;
					_switchScreens();
					_drawShit();
					_hidePainterGUI();
					return;
				}
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
			if((abs(dx)>20)||(abs(dy)>20))
			{
				dx=0;
				dy=0;
			}
			
			// make the new value the old value
			oldXY=touchXY;
			
			if((oldXY.px>=6 && oldXY.px < 54 && oldXY.py >= 60 && oldXY.py < 132) ||
			(oldXY.px>=5 && oldXY.px < 37 && oldXY.py >= 5 && oldXY.py < 37) ||
			(oldXY.px>=42 && oldXY.px < 74 && oldXY.py >= 5 && oldXY.py < 37))
				palTouch=true;
			
			// see if the user is trying to twist
			if((held & KEY_L)||(held & KEY_R))
			{
				twisting=TRUE;
			}else{
				if((dx || dy) && !(palTouch && painting))
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

void CubeGame::_drawPalette()
{
	Colour palette[6];
	float size = 0.6;
	mainCube.getPalette(palette);
	glTranslate3f32(floattof32(-3.1), floattof32(0.9), 0);
	for(int i=0; i<6; i++)
	{
		glColor3b(palette[i].r, palette[i].g, palette[i].b);
		if(i==paintColour)
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(8));
		else
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(0));
		glBegin(GL_QUADS);
		glVertex3f((i/3)*size,-(i%3)*size,0);
		glVertex3f((i/3)*size+size*0.9,-(i%3)*size,0);
		glVertex3f((i/3)*size+size*0.9,-(i%3)*size-size*0.9,0);
		glVertex3f((i/3)*size,-(i%3)*size-size*0.9,0);
		glEnd();
	}
}

void CubeGame::_showPainterGUI()
{
	oamSet(&oamMain,2,5,5,0,0,SpriteSize_32x32,SpriteColorFormat_256Color,gfxUndo,0,false,false,false,false,false);
	oamSet(&oamMain,3,42,5,0,0,SpriteSize_32x32,SpriteColorFormat_256Color,gfxRedo,0,false,false,false,false,false);
	oamSet(&oamMain,4,0,160,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxCancel,0,false,false,false,false,false);
	oamSet(&oamMain,5,192,160,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxOk,0,false,false,false,false,false);
}

void CubeGame::_hidePainterGUI()
{
	oamSet(&oamMain,2,10,10,0,0,SpriteSize_32x32,SpriteColorFormat_256Color,gfxUndo,0,false,true,false,false,false);
	oamSet(&oamMain,3,47,10,0,0,SpriteSize_32x32,SpriteColorFormat_256Color,gfxRedo,0,false,true,false,false,false);
	oamSet(&oamMain,4,0,160,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxCancel,0,false,true,false,false,false);
	oamSet(&oamMain,5,192,160,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxOk,0,false,true,false,false,false);
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

	glPushMatrix();
	if(painting) _drawPalette();
	glPopMatrix(1);
	glPushMatrix();
	if(showBackgroundImage)
	{
		glTranslate3f32(0, 0, floattof32(-10));
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(63));
		glMaterialf(GL_EMISSION, RGB15(31,31,31));
		glBindTexture(0, textureID);

		glBegin(GL_QUADS);
		glNormal(NORMAL_PACK(0,inttov10(-1),0));

		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(256),0));
		glVertex3f(-64,	64, 0 );

		GFX_TEX_COORD = (TEXTURE_PACK(inttot16(256), inttot16(256)));
		glVertex3f(64,	64, 0 );

		GFX_TEX_COORD = (TEXTURE_PACK(0,inttot16(256)));
		glVertex3f(64,	-64, 0 );

		GFX_TEX_COORD = (TEXTURE_PACK(0, 0));
		glVertex3f(-64,	-64, 0 );

		glEnd();

		glBindTexture(0,0);
	}
	glPopMatrix(1);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(0));
	// Update and draw the cube
	if(!settings)
	{
		glPushMatrix();
		mainCube.Update(twisting, touchXY, touchVector, painting, paintColour);
		glPopMatrix(1);
		if(!painting) oamSet(&oamMain,0,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxSettings,0,false,false,false,false,false);
		//settingsButton->show();
		oamSet(&oamMain,1,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,true,false,false,false);


	}else{
		if(!saving && !loading)
		{
			glPushMatrix();
			mainCube.Update(twisting, touchXY, touchVector, false, 0);
			glPopMatrix(1);
			oamSet(&oamMain,1,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,true,false,false,false);
		}else{
			oamSet(&oamMain,1,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxBack,0,false,false,false,false,false);

			SLRotation+=100;
			//SLRotation%=360;

			glPushMatrix();
			glTranslate3f32(inttof32(-2), 0, 0);
			glRotateYi(SLRotation);
			SLCube.Resize(0.9);
			SLCube.setLayout(SLLayout[0]);
			SLCube.Update(twisting, touchXY, touchVector, false, 0);
			glPopMatrix(1);

			glPushMatrix();
			glRotateYi(SLRotation);
			SLCube.Resize(1);
			SLCube.setLayout(SLLayout[1]);
			SLCube.Update(twisting, touchXY, touchVector, false, 0);
			glPopMatrix(1);

			glPushMatrix();
			glTranslate3f32(inttof32(2), 0, 0);
			glRotateYi(SLRotation);
			SLCube.Resize(0.9);
			SLCube.setLayout(SLLayout[2]);
			SLCube.Update(twisting, touchXY, touchVector, false, 0);
			glPopMatrix(1);

			if(keysDown() & KEY_TOUCH)
			{
				touchPosition tmpXY;
				touchRead(&tmpXY);

				// If player touches the top-right poly, take them back to settings
				if((tmpXY.px >=192) && (tmpXY.py <=32))
				{
					loading=false;
					saving=false;
					painting=false;
					_switchScreens();
					return;
				}

				int cubetmp=0;
				if(sqrt((45-tmpXY.px)*(45-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=1;
				else if(sqrt((128-tmpXY.px)*(128-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=2;
				else if(sqrt((211-tmpXY.px)*(211-tmpXY.px)+(96-tmpXY.py)*(96-tmpXY.py))<35)
					cubetmp=3;

				if(cubetmp)
				{
					if(saving)
						_saveCube(cubetmp-1);
					else
						_loadCube(cubetmp-1);
					_switchScreens();
					return;
				}
			}
		}


		oamSet(&oamMain,0,192,0,0,0,SpriteSize_64x32,SpriteColorFormat_256Color,gfxSettings,0,false,true,false,false,false);
		//settingsButton->hide();
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
		SLCube.setColour(i, (_settingsscreen->settings.colour[i][0]+1)*8-1, (_settingsscreen->settings.colour[i][1]+1)*8-1, (_settingsscreen->settings.colour[i][2]+1)*8-1);
		//for(int j=0; j<3; j++)
			//cube[j].setColour(i, (_settingsscreen->settings.colour[i][0]+1)*8-1, (_settingsscreen->settings.colour[i][1]+1)*8-1, (_settingsscreen->settings.colour[i][2]+1)*8-1);
	}
	glClearColor(_settingsscreen->settings.bgColour[0], _settingsscreen->settings.bgColour[1], _settingsscreen->settings.bgColour[2], 31);

	if(_settingsscreen->settings.showBackgroundImage && _settingsscreen->settings.bgFilenameLength)
	{
		int len=_settingsscreen->settings.bgFilenameLength;
		char* tmp;
		tmp = new char[len];
		for (int i=0; i<len; i++)
			tmp[i] = _settingsscreen->settings.bgFilename[i];
		//_settingsscreen->tbxBackgroundImage->getText().copyToCharArray(tmp);
		//_settingsscreen->tbxImageCheck->setText(tmp);
		if(_loadPNG(tmp))
			showBackgroundImage=true;
	}
	else
		showBackgroundImage=false;
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

bool CubeGame::_loadPNG(char* filename)
{
	FILE* fp;
	png_bytep* row_ptrs = NULL;

	/* Open the prospective PNG file. */
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		_settingsscreen->tbxImageCheck->setText(filename);
		return false;
	}

	if(check_if_png(fp))
	{
		//create png pointer
		png_structp png_ptr = png_create_read_struct
		(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
		{
			_settingsscreen->tbxImageCheck->setText("png_ptr failed");
			return false;
		}
		//create png info pointer
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			_settingsscreen->tbxImageCheck->setText("info_ptr failed");
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			return false;
		}
		//set error exit point
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			_settingsscreen->tbxImageCheck->setText("png load error");
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			fclose(fp);
			return false;
		}
		
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
		
		png_read_info(png_ptr, info_ptr);
		
		uint32 width = png_get_image_width(png_ptr, info_ptr);
		uint32 height = png_get_image_height(png_ptr, info_ptr);
		uint32 colourType = png_get_color_type(png_ptr, info_ptr);
		uint32 channels = png_get_channels(png_ptr, info_ptr);
		uint32 bitDepth = png_get_bit_depth(png_ptr, info_ptr);
		
		printf("width: %i\n", width);
		printf("height: %i\n", height);
		printf("bitDepth: %i\n", bitDepth);
		printf("channels: %i\n", channels);

		switch (colourType)
		{
			case PNG_COLOR_TYPE_PALETTE:
				png_set_palette_to_rgb(png_ptr);
				channels = 3;
				break;
			case PNG_COLOR_TYPE_GRAY:
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				png_set_gray_to_rgb(png_ptr);
				channels+=2;
				//if (bitDepth < 8)
				//png_set_expand_gray_1_2_4_to_8(png_ptr);
				//bitDepth = 8;
				break;
		}

		//We don't support 16 bit precision.. so if the image Has 16 bits per channel
        //precision... round it down to 8.
		if (bitDepth == 16)
		{
			png_set_strip_16(png_ptr);
			bitDepth = 8;
		}

		//if(channels==3)
		//	glColourType = GL_RGB;
		//if(channels==4)
		//	glColourType = GL_RGBA;

		png_read_update_info(png_ptr, info_ptr);

		int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		unsigned char* pngData;

		/* Allocate the image_data buffer. */
		if ((pngData = (unsigned char *) malloc(rowbytes * height))==NULL) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			_settingsscreen->tbxImageCheck->setText("pngData OOM");
			return false;
		}

		if ((row_ptrs = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			free(pngData);
			pngData = NULL;
			return false;
		}

		/* set the individual row_pointers to point at the correct offsets */
		for (uint i = 0;  i < height;  ++i)
			row_ptrs[height - 1 - i] = pngData + i*rowbytes;

		//And here it is! The actual reading of the image!
		//Read the imagedata and write it to the addresses pointed to
		//by rowptrs (in other words: our image databuffer)
		png_read_image(png_ptr, row_ptrs);

		backgroundTexData = new rgb[65536];

		for(uint i=0; i< 65536; i++)
		{
			uint dx = i / 256;
			uint dy = i % 256;
			uint sx = dx * width / 256;
			uint sy = dy * height / 256;
			uint source = sy * width + sx;

			backgroundTexData[i]=RGB8(pngData[source*channels], pngData[source*channels+1], pngData[source*channels+2]);
		}

		glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_256 , TEXTURE_SIZE_256, 0, TEXGEN_TEXCOORD, (u8*)backgroundTexData);

		free(backgroundTexData);
		free(pngData);
		free(row_ptrs);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		redraw();
		return true;
	}
	else
	{
		fclose(fp);
		_settingsscreen->tbxImageCheck->setText("not valid png");
		return false;
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
	case 47: //Apply Colours
		_settingsscreen->updateColours();
		_applySettings();
		break;
	case 48: //Paint
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();

		painting=true;
		paintColour=0;
		_showPainterGUI();

		mainCube.getLayout(oldLayout);
		settings=!settings;
		_switchScreens();
		break;
	case 94: //Quick Scramble
		mainCube.Scramble();
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();
		settings=!settings;
		titleScreen->hide();
		_switchScreens();
		break;
	case 95: //Play
		_settingsscreen->updateSettings();
		_applySettings();
		_saveSettings();
		settings=!settings;
		titleScreen->hide();
		_switchScreens();
		break;
	case 96: //Quick Load;
		loading=true;
		titleScreen->hide();
		_switchScreens();
		break;
	case 97: //Settings
		titleScreen->hide();
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
}


void CubeGame::shutdown() {

	// Call base shutdown method
	Woopsi::shutdown();
}
