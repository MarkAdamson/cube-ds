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
 
	Implemented so far:
	+ Created a simple data structure to represent the cube
	+ Draw the cube on the lower screen
	+ Rotate the cube using the touchscreen
	+ Pick a tile on the cube using L/R + touchscreen
	+ Rotate segments of the cube
	+ Randomise algorithm

	To Do:
	+ Solve algorithm
	+ Extra effects
	+ Backgrounds
	+ etc
*--------------------------------------------------------------------------------*/


#include <nds.h>
#include <nds/arm9/postest.h>

#include <stdio.h>
#include <stdlib.h>
#include "cubegame.h"

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------

	// Create the Windowless Woopsi application
	CubeGame app;
	// Start the app.
	app.main(argc, argv);
}
