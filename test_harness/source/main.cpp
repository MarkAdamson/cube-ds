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
 *
*--------------------------------------------------------------------------------*/


#include <nds.h>
#include <nds/arm9/postest.h>

#include <stdio.h>
#include <stdlib.h>
#include "rubikscube.h"

void initGfx()
{
	videoSetMode(MODE_5_3D | DISPLAY_BG0_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	bgSetPriority(0, 1);
	lcdMainOnBottom();
}

void initSprites()
{}

void initGL()
{
	// initialize gl
	glInit();

	glEnable(GL_OUTLINE | GL_ANTIALIAS);

	//set the first outline color to white
	glSetOutlineColor(0,RGB15(0,0,0));

	// setup the rear plane
	glClearColor(0,31,31,31); // set BG
	glClearPolyID(63); // the BG and polygons will have the same ID unless a polygon is highlighted
	glClearDepth(0x7FFF);

	// Set our view port to be the same size as the screen
	glViewport(0,0,255,191);

	//change ortho vs perspective
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 20);

	// Set the current matrix to be the model matrix
	glMatrixMode(GL_MODELVIEW);
	glPolyFmt(POLY_ALPHA(31) | POLY_CULL_FRONT | POLY_ID(0));

	glLoadIdentity();
	gluLookAt(	0.0, 0.0, 3.5,		//camera position
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up
}

int main(int argc, char* argv[])
{
	RubiksCube cube;
	bool twisting, moving;
	int dx, dy;
	touchPosition touchXY, oldXY;
	VECTOR touchVector;
	
	dx=0; dy=0;
	oldXY.px = 45;
	oldXY.py = 45;
	touchVector.X=0;
	touchVector.Y=0;
	touchVector.Z=0;
	
	initGfx();
	//initSprites();
	initGL();
	
	consoleDemoInit();
	printf("Console Output:\n");
	
	while(true)
	{
		glPushMatrix();
		// Reset movement states
		twisting=FALSE;
		moving=FALSE;
		
		//get input
		scanKeys();
		int down = keysDown();
		int held = keysHeld();

		// Reset x and y when user touches screen
		if(down & KEY_TOUCH)
		{
			touchRead(&oldXY);
		}

		// Some simple tests using face buttons
		if(down & KEY_X)
		{
			printf("about to test solve...\n");
			cube.Solve();
		}
		if(down & KEY_Y)
		{
			cube.Scramble();
		}
		if(down & KEY_A)
		{
		}
		if(down & KEY_B)
		{
		}

		//if user drags then grab the delta
		if(held & KEY_TOUCH)
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
	
		// create a vector from stylus input to pass to the cube
		touchVector.X=dx;
		touchVector.Y=-dy;

		// Move the cube if necessary
		if(moving) cube.Move(dx, dy);
	
		cube.Update(twisting, touchXY, touchVector, false, 0);
		
		glPopMatrix(1);
		
		glFlush(0);

		swiWaitForVBlank();
	}
}