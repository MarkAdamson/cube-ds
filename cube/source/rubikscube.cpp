/*
 * rubikscube.cpp
 * A Rubik's Cube class
 * by Mark Adamson
 */

#include <nds.h>
#include <nds/arm9/postest.h>

#include <stdio.h>
#include <stdlib.h>

#include "rubikscube.h"

int closeW; // closest distance to camera
int polyCount; // keeps track of the number of polygons drawn
int32 tvf32[3];

void RotateVector (m4x4 &M, VECTOR &V, VECTOR &D)		// Rotate A Vector Using The Supplied Matrix
{
	// Rotate Around The X Axis
	D.X = (f32tofloat(M.m[0]) * V.X) + (f32tofloat(M.m[4]) * V.Y) + (f32tofloat(M.m[8])  * V.Z);
	// Rotate Around The Y Axis
	D.Y = (f32tofloat(M.m[1]) * V.X) + (f32tofloat(M.m[5]) * V.Y) + (f32tofloat(M.m[9])  * V.Z);
	// Rotate Around The Z Axis
	D.Z = (f32tofloat(M.m[2]) * V.X) + (f32tofloat(M.m[6]) * V.Y) + (f32tofloat(M.m[10]) * V.Z);
}

void unitVector (int32* vector)
{
	int32 magnitude = sqrtf32(mulf32(vector[0], vector[0]) + mulf32(vector[1], vector[1]));
	vector[0]=divf32(vector[0], magnitude);
	vector[1]=divf32(vector[1], magnitude);
}

void RubiksCube::startCheck()
{
	while(PosTestBusy()); // wait for the position test to finish
	while(GFX_BUSY); // wait for all the polygons from the last object to be drawn
	PosTest_Asynch(0,0,0); // start a position test at the current translated position
	polyCount = GFX_POLYGON_RAM_USAGE; // save the polygon count
}

void RubiksCube::endCheck(int side, int tileX, int tileY) {
	while(GFX_BUSY); // wait for all the polygons to get drawn
	while(PosTestBusy()); // wait for the position test to finish
	if(GFX_POLYGON_RAM_USAGE>polyCount) // if a polygon was drawn
	{
		if(PosTestWresult()<=closeW) {
			// this is currently the closest object under the cursor!
			closeW=PosTestWresult();
			clicked[0]=side;
			clicked[1]=tileX;
			clicked[2]=tileY;
		}
	}
}

//---------------------------------------------------------------------------------
// Returns the *starting* Up and Right vectors for a given side. Hardcoded.
//---------------------------------------------------------------------------------
void RubiksCube::vectorFromSide(VECTOR &UV, VECTOR &RV, int side)
{
	switch(side){
		case RC_TOP:
			UV.X=0;
			UV.Y=0;
			UV.Z=1;
			RV.X=-1;
			RV.Y=0;
			RV.Z=0;
			break;
		case RC_BOTTOM:
			UV.X=0;
			UV.Y=0;
			UV.Z=1;
			RV.X=1;
			RV.Y=0;
			RV.Z=0;
			break;
		case RC_FRONT:
			UV.X=1;
			UV.Y=0;
			UV.Z=0;
			RV.X=0;
			RV.Y=-1;
			RV.Z=0;
			break;
		case RC_RIGHT:
			UV.X=0;
			UV.Y=1;
			UV.Z=0;
			RV.X=0;
			RV.Y=0;
			RV.Z=-1;
			break;
		case RC_BACK:
			UV.X=1;
			UV.Y=0;
			UV.Z=0;
			RV.X=0;
			RV.Y=1;
			RV.Z=0;
			break;
		case RC_LEFT:
			UV.X=0;
			UV.Y=1;
			UV.Z=0;
			RV.X=0;
			RV.Y=0;
			RV.Z=1;
			break;
	}
}

void RubiksCube::setDefaultColours()
{
	palette[RC_WHITE].r=255;
	palette[RC_WHITE].g=255;
	palette[RC_WHITE].b=255;
	palette[RC_YELLOW].r=255;
	palette[RC_YELLOW].g=255;
	palette[RC_YELLOW].b=0;
	palette[RC_BLUE].r=0;
	palette[RC_BLUE].g=0;
	palette[RC_BLUE].b=255;
	palette[RC_ORANGE].r=255;
	palette[RC_ORANGE].g=127;
	palette[RC_ORANGE].b=0;
	palette[RC_GREEN].r=0;
	palette[RC_GREEN].g=127;
	palette[RC_GREEN].b=0;
	palette[RC_RED].r=191;
	palette[RC_RED].g=0;
	palette[RC_RED].b=0;
}

//---------------------------------------------------------------------------------
// Sets GL color from an int
//---------------------------------------------------------------------------------
void RubiksCube::SetRCColor(int colourIndex)
{
	glColor3b(palette[colourIndex].r, palette[colourIndex].g, palette[colourIndex].b);
}

//---------------------------------------------------------------------------------
// Draw a single tile of the rubik's cube. assumes all necessary rotation has
// been performed already.
//---------------------------------------------------------------------------------
void RubiksCube::DrawTile(int color, int32 x, int32 y, int32 z, int32 size, bool picking)
{
	int16 blackbit = f32tov16(divf32(size, inttof32(20)));

	int16 vx, vy, vz, vsize;
	vx = f32tov16(x);
	vy = f32tov16(y);
	vz = f32tov16(z);
	vsize = f32tov16(size);

	if(!picking) SetRCColor(color); //if we're not picking, then we need the right colour
	// Draw the coloured part of the tile (sans black bits):
	glVertex3v16(vx + blackbit, vy + blackbit, vz);
	glVertex3v16(vx + vsize - blackbit, vy + blackbit, vz);
	glVertex3v16(vx + vsize - blackbit, vy + vsize - blackbit, vz);
	glVertex3v16(vx + blackbit, vy + vsize - blackbit, vz);

	if(!picking) //if we're not picking, better draw in the black bits too
	{
		glColor3f(0,0,0);
		//top
		glVertex3v16(vx, vy, vz + blackbit);
		glVertex3v16(vx + vsize, vy, vz + blackbit);
		glVertex3v16(vx + vsize - blackbit, vy + blackbit, vz);
		glVertex3v16(vx + blackbit, vy + blackbit, vz);
		//right
		glVertex3v16(vx + vsize - blackbit, vy + blackbit, vz);
		glVertex3v16(vx + vsize, vy, vz + blackbit);
		glVertex3v16(vx + vsize, vy + vsize, vz + blackbit);
		glVertex3v16(vx + vsize - blackbit, vy + vsize - blackbit, vz);
		//bottom
		glVertex3v16(vx + blackbit, vy + vsize - blackbit, vz);
		glVertex3v16(vx + vsize - blackbit, vy + vsize - blackbit, vz);
		glVertex3v16(vx + vsize, vy + vsize, vz + blackbit);
		glVertex3v16(vx, vy + vsize, vz + blackbit);
		//left
		glVertex3v16(vx, vy, vz + blackbit);
		glVertex3v16(vx + blackbit, vy + blackbit, vz);
		glVertex3v16(vx + blackbit, vy + vsize - blackbit, vz);
		glVertex3v16(vx, vy + vsize, vz + blackbit);
	}
}

//---------------------------------------------------------------------------------
// Draws a 3x3 side of the rubiks cube. as with DrawTile, assumes that all rotation
// has already been performed.
//---------------------------------------------------------------------------------
void RubiksCube::DrawSide(RubikSide side, int sideNum, int32 x, int32 y, int32 z, int32 size, bool picking) {
//---------------------------------------------------------------------------------
	int color; // generate an int to signify tile colour (see enum RC_Color)
	glBegin(GL_QUADS); //start drawin squares
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<3; j++)
		{
			color = side.tile[i][j].color; //get this tile's colour
			if(picking) startCheck(); // if picking...
			DrawTile(	color,
						x + mulf32(inttof32(i),divf32(size,inttof32(3))),
						y + mulf32(inttof32(j),divf32(size,inttof32(3))),
						z,
						divf32(size,inttof32(3)),
						picking); // draw the tile at (i,j)
			if(picking) endCheck(sideNum, i, j); // if picking...
		}
	}
	glEnd(); // stop drawin squares

}

//---------------------------------------------------------------------------------
// Draws a 3x3 side of the rubiks cube. as with DrawTile, assumes that all rotation
// has already been performed.
//---------------------------------------------------------------------------------
void RubiksCube::DrawLine(RubikLine line, int32 x, int32 y, int32 z, int32 size) {
//---------------------------------------------------------------------------------
	int color; // generate an int to signify tile colour (see enum RC_Color)
	glBegin(GL_QUADS); //start drawin squares
	for(int i=0; i<3; i++)
	{
		color = line.tile[i].color; //get this tile's colour
		DrawTile(color, x, y + mulf32(inttof32(i),size), z, size, false); // draw the tile at (i,j)
	}
	glEnd(); // stop drawin squares

}


//---------------------------------------------------------------------------------
// Rotate a given RubikSide clockwise, a given number of times
// Return the rotated RubikSide
//---------------------------------------------------------------------------------
RubikSide RubiksCube::RotateSide(RubikSide in, int rotations)
{
	RubikSide out=in;
	RubikSide old=in;
	while(rotations)
	{
		for(int i=0;i<3;i++)
		{
			for(int j=0;j<3;j++)
			{
				out.tile[2-j][i].color = old.tile[i][j].color;
			}
		}
		old=out;
		rotations--;
	}
	return out;
}

//---------------------------------------------------------------------------------
// Takes data from varying sources, and creates a description of the required
// Twist. Pretty special - when it's working...
//---------------------------------------------------------------------------------
void RubiksCube::InitTwist(bool vert, int32 *vector)
{
	if(vert) // if it was an Up/Down twist...
	{
		Twist.axis=(clicked[0]+2)%3;
		if(clicked[0]<3) Twist.position=clicked[1];
		else Twist.position=2-clicked[1];
	}else{
		Twist.axis=(clicked[0]+1)%3;
		Twist.position=2-clicked[2];
	}
	
	Twist.rotation=0;
	
	Twist.vector[0]=vector[0];
	Twist.vector[1]=vector[1];
	if(vert && (clicked[0]<3))
	{
		Twist.vector[0]=-Twist.vector[0];
		Twist.vector[1]=-Twist.vector[1];
	}

	TwistFromCube();
}


//---------------------------------------------------------------------------------
// Loads data from the cube into the twist
//---------------------------------------------------------------------------------
void RubiksCube::TwistFromCube()
{
	Twist.left.side=Side[Twist.axis];
	Twist.right.side=Side[Twist.axis + 3];
	
	int one=((Twist.axis+1)%3)+3;
	int two=((Twist.axis+2)%3)+3;
	int three=(Twist.axis+1)%3;
	int four=(Twist.axis+2)%3;
	for(int i=0;i<3;i++)
	{
		Twist.left.line[0].tile[i].color=Side[one].tile[2][2-i].color;
		Twist.middle.line[0].tile[i].color=Side[one].tile[1][2-i].color;
		Twist.right.line[0].tile[i].color=Side[one].tile[0][2-i].color;
		
		Twist.left.line[1].tile[i].color=Side[two].tile[i][2].color;
		Twist.middle.line[1].tile[i].color=Side[two].tile[i][1].color;
		Twist.right.line[1].tile[i].color=Side[two].tile[i][0].color;
		
		Twist.left.line[2].tile[i].color=Side[three].tile[0][i].color;
		Twist.middle.line[2].tile[i].color=Side[three].tile[1][i].color;
		Twist.right.line[2].tile[i].color=Side[three].tile[2][i].color;
		
		Twist.left.line[3].tile[i].color=Side[four].tile[i][2].color;
		Twist.middle.line[3].tile[i].color=Side[four].tile[i][1].color;
		Twist.right.line[3].tile[i].color=Side[four].tile[i][0].color;
	}
}

//---------------------------------------------------------------------------------
// Interprets the final position of a twist and puts the data back onto the cube
// Also sends twist data to thecubex solver, and manages scrambling/solving.
//---------------------------------------------------------------------------------
void RubiksCube::FinishTwist()
{
	// Count up rotations (quarter-twists)
	int rotation=Twist.rotation;
	rotation %= 360;
	if(rotation<0)
		rotation+=360;
	int rotations=rotation/90;//find the number of quarter-turns (moves)
	if(rotation % 90 > 45) rotations++;
	
	if(Twist.position==0 || Twist.position==3)
	{
		Twist.left.side=RotateSide(Twist.left.side, rotations);
		for(int i=0;i<rotations;i++)
		{
			RubikLine tmpline=Twist.left.line[3];
			Twist.left.line[3]=Twist.left.line[2];
			Twist.left.line[2]=Twist.left.line[1];
			Twist.left.line[1]=Twist.left.line[0];
			Twist.left.line[0]=tmpline;
		}
	}
	if(Twist.position==1 || Twist.position==3)
	{
		
		for(int i=0;i<rotations;i++)
		{
			RubikLine tmpline=Twist.middle.line[3];
			Twist.middle.line[3]=Twist.middle.line[2];
			Twist.middle.line[2]=Twist.middle.line[1];
			Twist.middle.line[1]=Twist.middle.line[0];
			Twist.middle.line[0]=tmpline;
		}
	}
	if(Twist.position==2 || Twist.position==3)
	{
		Twist.right.side=RotateSide(Twist.right.side, 4-rotations);
		for(int i=0;i<rotations;i++)
		{
			RubikLine tmpline=Twist.right.line[3];
			Twist.right.line[3]=Twist.right.line[2];
			Twist.right.line[2]=Twist.right.line[1];
			Twist.right.line[1]=Twist.right.line[0];
			Twist.right.line[0]=tmpline;
		}
	}
	
	Side[Twist.axis]=Twist.left.side;
	Side[Twist.axis + 3]=Twist.right.side;
	
	int one=((Twist.axis+1)%3)+3;
	int two=((Twist.axis+2)%3)+3;
	int three=(Twist.axis+1)%3;
	int four=(Twist.axis+2)%3;
	for(int i=0;i<3;i++)
	{
		Side[one].tile[2][2-i].color=Twist.left.line[0].tile[i].color;
		Side[one].tile[1][2-i].color=Twist.middle.line[0].tile[i].color;
		Side[one].tile[0][2-i].color=Twist.right.line[0].tile[i].color;
		
		Side[two].tile[i][2].color=Twist.left.line[1].tile[i].color;
		Side[two].tile[i][1].color=Twist.middle.line[1].tile[i].color;
		Side[two].tile[i][0].color=Twist.right.line[1].tile[i].color;
		
		Side[three].tile[0][i].color=Twist.left.line[2].tile[i].color;
		Side[three].tile[1][i].color=Twist.middle.line[2].tile[i].color;
		Side[three].tile[2][i].color=Twist.right.line[2].tile[i].color;
		
		Side[four].tile[i][2].color=Twist.left.line[3].tile[i].color;
		Side[four].tile[i][1].color=Twist.middle.line[3].tile[i].color;
		Side[four].tile[i][0].color=Twist.right.line[3].tile[i].color;
	}
	
	// Send the twist to cubex.
	switch(Twist.axis)
	{
	case 0:
		switch(Twist.position)
		{
		case 0:
			for(int i=0; i<rotations; i++)
			{
				solver.LD();
				printf("LD\n");
			}
			break;
		case 1:
			for(int i=0; i<rotations; i++)
			{
				solver.MD();
				printf("MD\n");
			}
			break;
		case 2:
			for(int i=0; i<rotations; i++)
			{
				solver.RD();
				printf("RD\n");
			}
			break;
		case 3:
			for(int i=0; i<rotations; i++)
			{
				solver.CD();
				printf("CD\n");
			}
			break;
		}
	break;
	case 1:
		switch(Twist.position)
		{
		case 0:
			for(int i=0; i<rotations; i++)
			{
				solver.DR();
				printf("DR\n");
			}
			break;
		case 1:
			for(int i=0; i<rotations; i++)
			{
				solver.MR();
				printf("MR\n");
			}
			break;
		case 2:
			for(int i=0; i<rotations; i++)
			{
				solver.UR();
				printf("UR\n");
			}
			break;
		case 3:
			for(int i=0; i<rotations; i++)
			{
				solver.CR();
				printf("CR\n");
			}
			break;
		}
	break;
	case 2:
		switch(Twist.position)
		{
		case 0:
			for(int i=0; i<rotations; i++)
			{
				solver.BA();
				printf("BA\n");
			}
			break;
		case 1:
			for(int i=0; i<rotations; i++)
			{
				solver.MA();
				printf("MA\n");
			}
			break;
		case 2:
			for(int i=0; i<rotations; i++)
			{
				solver.FA();
				printf("FA\n");
			}
			break;
		case 3:
			for(int i=0; i<rotations; i++)
			{
				solver.CA();
				printf("CA\n");
			}
			break;
		}
	break;
	}

	Twist.rotation=0;
	AutoTwisting=false;
	
	// handle scrambling
	if(Scrambling)
	{
		scramblecount--;
		if(scramblecount==0)
			Scrambling=false;
	}
	// or solving
	if(Solving)
	{
		solvecount--;
		if(solvecount==0)
			Solving = false;
	}
}

//---------------------------------------------------------------------------------
// Used for Autotwisting - moves the twist on one increment, and if needed
// calls FinishTwist
//---------------------------------------------------------------------------------
void RubiksCube::TwistAgain()
{
	//printf("TwistAgain\n");
	AutoTwisting=true;
	if(Twist.direction)
	{
		int rotation=Twist.rotation;
		rotation%=90;
		rotation=90-rotation;
		//printf("rotation=%i\n",rotation);
		if(rotation<=10)
		{
			FinishTwist();
		}else{
			Twist.rotation+=10;
		}

	}
	if(!Twist.direction)
	{
		int rotation=Twist.rotation;
		rotation%=90;
		//printf("%i...", rotation);
		if(rotation<=10)
		{
			FinishTwist();
		}else{
			Twist.rotation-=10;
		}
	}

}

//---------------------------------------------------------------------------------
// Begins an Autotwist. Called either to finish up a manual twist, or as part
// of scrambling or solving
//---------------------------------------------------------------------------------
void RubiksCube::AutoTwist(int axis, int position, bool direction)
{
	// THIS LOOKS WRONG:
	if(direction) Twist.rotation=1;
	else Twist.rotation=359;

	
	Twist.axis=axis;
	Twist.position=position;
	Twist.direction=direction;
	TwistFromCube();
	TwistAgain();
}

RubiksCube::RubiksCube()
{
	Position.X=inttof32(-1);
	Position.Y=inttof32(-1);
	Position.Z=inttof32(-1);
	Size=inttof32(2);
	rotateSensitivity=2;
	twistSensitivity=2;
	controlStyle=0;
	setDefaultColours();
	Reset();
}

RubiksCube::RubiksCube(float size)
{
	Position.X=floattof32(-size/2);
	Position.Y=floattof32(-size/2);
	Position.Z=floattof32(-size/2);
	Size=floattof32(size);
	rotateSensitivity=2;
	twistSensitivity=2;
	controlStyle=0;
	setDefaultColours();
	Reset();
}

void RubiksCube::Scramble()
{
	//printf("Scramble();\n");
	if(!isBusy())
	{
		//printf("Not Busy\n");
		Scrambling=true;
		scramblecount=20;
	}
}

void RubiksCube::Solve()
{
	solver.SolveCube();
	solvecount=solver.mov[0];
	if(solvecount)Solving=true;
	printf("solvecount = %i", solvecount);
	for (int i=0; i<solvecount; i++)
	{
		for (int j=0; j<3; j++)
		{
			solution[i][j]=solver.isolution[i+1][j];
		}
		printf("axis: %i, position: %i, direction: %i\n", solver.isolution[i+1][0], solver.isolution[i+1][1], solver.isolution[i+1][2]);
	}
}

void RubiksCube::Reset()
{
	solver.ResetCube();
	//solver.cenfix=true;
	solver.shorten=true;
	for(int i=0; i<6; i++){
		for(int j=0; j<3; j++){
			for(int k=0; k<3; k++){
				Side[i].tile[j][k].color=i;
	}}}
	/*Side[RC_LEFT].tile[0][0].color=RC_BLUE;
	Side[RC_RIGHT].tile[0][0].color=RC_BLUE;
	Side[RC_TOP].tile[0][0].color=RC_BLUE;
	Side[RC_BOTTOM].tile[0][0].color=RC_BLUE;
	Side[RC_BACK].tile[0][0].color=RC_BLUE;
	Side[RC_FRONT].tile[0][0].color=RC_WHITE;*/
	rx=45;
	ry=45;
}

void RubiksCube::getLayout(RubikSide* output)
{
	for(int i=0; i<6; i++)
	{
		output[i]=Side[i];
	}
}

void RubiksCube::setLayout(RubikSide* input)
{
	for(int i=0; i<6; i++)
	{
		Side[i]=input[i];
	}
}

void RubiksCube::getPalette(Colour* output)
{
	for(int i=0; i<6; i++)
		output[i]=palette[i];
}

void RubiksCube::setPalette(Colour* input)
{
	for(int i=0; i<6; i++)
		palette[i]=input[i];
}

void RubiksCube::setRotateSensitivity(int newValue)
{
	rotateSensitivity=newValue/10;
}

void RubiksCube::setTwistSensitivity(int newValue)
{
	twistSensitivity=newValue/10;
}

void RubiksCube::setControlStyle(int newValue)
{
	controlStyle=newValue;
}

void RubiksCube::setColour(int colourIndex, int r, int g, int b)
{
	palette[colourIndex].r=r;
	palette[colourIndex].g=g;
	palette[colourIndex].b=b;
}

void RubiksCube::Tester()
{
	solver.RenderScreen();
}

void RubiksCube::Move(int x, int y)
{
	rx+=x*rotateSensitivity;
	int newy=ry+y*rotateSensitivity;
	if(newy>100)
		ry=100;
	else if(newy<-100)
		ry=-100;
	else
		ry=newy;
}

void RubiksCube::Resize(float size)
{
	Position.X=floattof32(-size/2);
	Position.Y=floattof32(-size/2);
	Position.Z=floattof32(-size/2);
	Size=floattof32(size);
}

bool RubiksCube::isBusy()
{
	return(Twisting||AutoTwisting);
}

void RubiksCube::Grab(VECTOR touchVector)
{
	tvf32[0]+=floattof32(touchVector.X);
	tvf32[1]+=floattof32(touchVector.Y);
	tvf32[2]=0;
	if(f32toint(sqrtf32(mulf32(tvf32[0],tvf32[0])+mulf32(tvf32[1],tvf32[1])+mulf32(tvf32[2],tvf32[2])))>5)
	{
		VECTOR upv, rightv, rotduv, rotdrv;
		int32 uvf32[3];//up vector as f32
		int32 rvf32[3];//right vector
		int32 magup, magright;
		m4x4 grabMatrix;//container for the Position Matrix

		vectorFromSide(upv, rightv, clicked[0]);
		//printf ("Initial Vector:\n %f, ", tmpv.X);
		//printf("%f, ", tmpv.Y);
		//printf("%f\n", tmpv.Z);
		glGetFixed(GL_GET_MATRIX_POSITION, (int32*)&grabMatrix);
		glMatrixMode(GL_MODELVIEW);
		//rotate the up vector thru the projection matrix
		//and cast it to f32
		RotateVector(grabMatrix, upv, rotduv);
		uvf32[0]=floattof32(rotduv.X);
		uvf32[1]=floattof32(rotduv.Y);
		uvf32[2]=floattof32(rotduv.Z);
		//rinse and repeat with the right vector
		RotateVector(grabMatrix, rightv, rotdrv);
		rvf32[0]=floattof32(rotdrv.X);
		rvf32[1]=floattof32(rotdrv.Y);
		rvf32[2]=floattof32(rotdrv.Z);

		if(controlStyle)
		{
			int32 suvf32[3];
			int32 srvf32[3];
			suvf32[0]=0;
			suvf32[1]=inttof32(1);
			suvf32[2]=0;
			srvf32[0]=inttof32(1);
			srvf32[1]=0;
			srvf32[2]=0;

			magup=dotf32(uvf32, suvf32);
			magright=dotf32(uvf32, srvf32);
			if(abs(magup)>abs(magright))
			{
				for(int i=0; i<3; i++)
				{
					if(magup>0)
					{
						rvf32[i]=srvf32[i];
						uvf32[i]=suvf32[i];
					}
					else
					{
						rvf32[i]=-srvf32[i];
						uvf32[i]=-suvf32[i];
					}
				}
			} else {
				for(int i=0; i<3; i++)
				{
					if(magright>0)
					{
						uvf32[i]=srvf32[i];
						rvf32[i]=-suvf32[i];
					}
					else
					{
						uvf32[i]=-srvf32[i];
						rvf32[i]=suvf32[i];
					}
				}
			}

		}
		
		magup=dotf32(uvf32, tvf32);
		magright=dotf32(rvf32, tvf32);
		
		if(magup || magright)
		{
			int32 tmp[2];
			if(abs(magup)>abs(magright))
			{
				tmp[0]=uvf32[0];
				tmp[1]=uvf32[1];
				unitVector((int32*)tmp);
				InitTwist(true, tmp);
			}else{
				tmp[0]=rvf32[0];
				tmp[1]=rvf32[1];
				unitVector((int32*)tmp);
				InitTwist(false, tmp);
			}
			Twisting=true;
			Grabbing=false;
			tvf32[0]=0;
			tvf32[1]=0;
		}
	}
}

void RubiksCube::Draw(touchPosition touchXY)
{
	int viewport[]={0,0,255,191}; // used later for gluPickMatrix()
	
	int16 vx, vy, vz, vsize;
	vx = f32tov16(Position.X);
	vy = f32tov16(Position.Y);
	vz = f32tov16(Position.Z);
	vsize = f32tov16(Size);

	glPushMatrix();
	{
		glRotateY(90);
		glRotateZ(180);
		
		if(Twisting||AutoTwisting)
		{
			int ax=0;
			// Rotate so we draw the cube the right way round
			while(ax<Twist.axis)
			{
				glRotateX(90);
				glRotateZ(90);
				ax++;
			}

			if(Twist.position==0 || Twist.position==3) glRotateZ(Twist.rotation);
			DrawSide(Twist.left.side,Twist.axis,Position.X,Position.Y,Position.Z,Size,false);
			glBegin(GL_QUADS);
				glColor3f(0,0,0);
				glVertex3v16(vx + vsize, vy, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx, vy, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx, vy + vsize, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx + vsize, vy + vsize, vz + f32tov16(divf32(Size,inttof32(3))));
			glEnd();
			if(Twist.position==0 || Twist.position==3) glRotateZ(-Twist.rotation);
			glRotateY(180);
			if(Twist.position==2 || Twist.position==3) glRotateZ(-Twist.rotation);
			DrawSide(Twist.right.side,Twist.axis+3,Position.X,Position.Y,Position.Z,Size,false);
			glBegin(GL_QUADS);
				glColor3f(0,0,0);
				glVertex3v16(vx + vsize, vy, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx, vy, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx, vy + vsize, vz + f32tov16(divf32(Size,inttof32(3))));
				glVertex3v16(vx + vsize, vy + vsize, vz + f32tov16(divf32(Size,inttof32(3))));
			glEnd();
			if(Twist.position==2 || Twist.position==3) glRotateZ(Twist.rotation);
			glRotateY(90);
			glRotateX(-90);
			if(Twist.position==1 || Twist.position==3) glRotateX(Twist.rotation);
			glBegin(GL_QUADS);
				glColor3f(0,0,0);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3))), vy, vz);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3))), vy + vsize, vz);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3))), vy + vsize, vz + vsize);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3))), vy, vz + vsize);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3)))*2, vy, vz);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3)))*2, vy, vz + vsize);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3)))*2, vy + vsize, vz + vsize);
				glVertex3v16(vx + f32tov16(divf32(Size,inttof32(3)))*2, vy + vsize, vz);
			glEnd();
			if(Twist.position==1 || Twist.position==3) glRotateX(-Twist.rotation);
			for(int i=0;i<4;i++)
			{
				if(Twist.position==0) glRotateX(Twist.rotation);
				DrawLine(Twist.left.line[i],Position.X,Position.Y,Position.Z,Size/3);
				if(Twist.position==0) glRotateX(-Twist.rotation);
				if(Twist.position==1) glRotateX(Twist.rotation);
				DrawLine(Twist.middle.line[i],Position.X+(Size/3),Position.Y,Position.Z,Size/3);
				if(Twist.position==1) glRotateX(-Twist.rotation);
				if(Twist.position==2) glRotateX(Twist.rotation);
				DrawLine(Twist.right.line[i],Position.X+(Size/3*2),Position.Y,Position.Z,Size/3);
				if(Twist.position==2) glRotateX(-Twist.rotation);
				glRotateX(90);
			}
		}else{
			for(int i=0;i<3;i++)
			{
				DrawSide(Side[i],i,Position.X,Position.Y,Position.Z,Size, false);
				glRotateY(180);
				DrawSide(Side[i+3],i+3,Position.X,Position.Y,Position.Z,Size, false);
				glRotateX(90);
				glRotateZ(-90);
			}
		}
	}
	glPopMatrix(1);
	
	
	if(Picking){
		for(int i=0;i<3;i++)
			clicked[i]=0;
		closeW = 0x7FFFFFFF; //reset the distance
		
		//set the viewport to just off-screen, this hides all rendering that will be done during picking
		glViewport(0,192,0,192);
		
		// setup the projection matrix for picking
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPickMatrix((touchXY.px),(191-touchXY.py),4,4,viewport); // render only what is below the cursor
		gluPerspective(70, 256.0 / 192.0, 0.1, 10); // this must be the same as the original perspective matrix
		
		glMatrixMode(GL_MODELVIEW); // switch back to modifying the modelview matrix for drawing
		
		{
			glRotateY(90);
			glRotateZ(180);
			for(int i=0;i<3;i++)
			{
				DrawSide(Side[i],i,Position.X,Position.Y,Position.Z,Size, true);
				glRotateY(180);
				DrawSide(Side[i+3],i+3,Position.X,Position.Y,Position.Z,Size, true);
				glRotateX(90);
				glRotateZ(-90);
			}
		}
		
		glViewport(0,0,255,191);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70, 256.0 / 192.0, 0.1, 10);
		glMatrixMode(GL_MODELVIEW);
		
		if(closeW<0x7FFFFFFF)
		{
			if(Painting)
			{
				Side[clicked[0]].tile[clicked[1]][clicked[2]].color=paintColour;
			}else{
				Grabbing=true;
				Picking=false;
			}
		}
	}else{
		//Twisting=false;
	}
}

//---------------------------------------------------------------------------------
// Updates the Rubik's Cube, called once per frame. The general workhorse function
// of the class. Handles Grabbing/Twisting, and calls the Draw() function.
//---------------------------------------------------------------------------------
void RubiksCube::Update(bool moving, touchPosition touchXY, VECTOR touchVector, bool painting, int colour)
{
	Painting=painting;
	paintColour=colour;
	// somehow stumbled onto a good way to handle Twisting using states
	// FUCK TOP-DOWN PLANNING; PROTOTYPING FTW!
	if (moving && !Grabbing && !Twisting)
	{
		Picking=true;
	}
	if(!moving)
	{
		if(Twisting)
		{
			if(Twist.rotation%90>=45) Twist.direction=true;
			else Twist.direction=false;
			TwistAgain();
		}
		Picking=false;
		Grabbing=false;
		Twisting=false;
	}

	if(AutoTwisting)
		TwistAgain();

	if((!AutoTwisting) && Scrambling)
	{
		//printf("(!AutoTwisting) && Scrambling\n");
		AutoTwist(rand()%3, rand()%3, !(!(rand()%2)));
	}
	
	if((!AutoTwisting) && Solving)
	{
		int tmp = solver.mov[0]-solvecount;
		AutoTwist(solution[tmp][0], solution[tmp][1], (!(!solution[tmp][2])));
	}

	
	if(Twisting)
	{
		int32 touchVf32[3];
		int32 twistVf32[3];
		
		touchVf32[0]=floattof32(touchVector.X*twistSensitivity);
		touchVf32[1]=floattof32(touchVector.Y*twistSensitivity);
		touchVf32[2]=0;
		
		twistVf32[0]=Twist.vector[0];
		twistVf32[1]=Twist.vector[1];
		twistVf32[2]=0;
		
		int32 twistyness = dotf32(twistVf32, touchVf32);
		Twist.rotation+=f32toint(twistyness);
		if (Twist.rotation<0)
			Twist.rotation+=360;
	}
	
	//move the cube
	glRotateX(ry);
	glRotateY(rx);
	
	
	if(Grabbing) Grab(touchVector);
	
	Draw(touchXY);
}
