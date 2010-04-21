/*---------------------------------------------------------------------------------
	
	Rubik's Cube Demo by Mark Adamson
	=================================
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
