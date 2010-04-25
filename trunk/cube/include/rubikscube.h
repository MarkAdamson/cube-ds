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

#ifndef CUBE_H
#define CUBE_H

#include <nds.h>
#include <nds/arm9/postest.h>

#include <stdio.h>
#include <stdlib.h>

#include "cubex.h"

using namespace std;

typedef struct tagVECTOR
{
	float X, Y, Z;
}VECTOR;

typedef struct
{
	int32 X, Y, Z;
}VECTORf32;

typedef struct
{
	uint8 r, g, b;
}Colour;

typedef struct
{
	int color; //tile colour, reference to RC_Color
}RubikTile;

// RubikLine:
// a row of three rubik tiles, can be any three, but usually 3-in-a-row.
// used in segment rotation
typedef struct
{
	RubikTile tile [3];
}RubikLine;

typedef struct
{
	RubikTile tile [3][3];
}RubikSide; //a 3x3 tile side of a rubik's cube

// SideSegment
// instance of one segment for rotation.
// consists of one side and four lines,
// arranged clockwise around the side.
typedef struct
{
	RubikSide side;
	RubikLine line [4];
}SideSegment;

// MidSegment
// a middle segment for rotation.
// consists of four lines, proceeding linearly,
// starting from 'MovingSide'
typedef struct
{
	RubikLine line [4];
}MidSegment;


// RubikTwist
// Holds a description of the cube while one side is being
// rotated. Gives the axis around which the rotation occurs,
// which segment on that axis is rotating, and the angle.
// also holds the initial vector along which touchscreen
// input is measured, and descriptions of the three segments.
typedef struct
{
	int axis; //x=0,y=1,z=2
	int position; //left=0,middle=1,right=2
	int rotation;
	bool direction;
	int32 vector[2];
	SideSegment left, right;
	MidSegment middle;
}RubikTwist;

class RubiksCube
{
	private:
	enum RC_Color {RC_RED, RC_YELLOW, RC_GREEN, RC_ORANGE, RC_WHITE, RC_BLUE};
	enum RC_Side {RC_LEFT, RC_BOTTOM, RC_BACK, RC_RIGHT, RC_TOP, RC_FRONT};
	RubikTwist Twist; // Used in Twisting (duh)
	int clicked[3]; // Picking: tracks which tile is currently picked
	bool Picking, Grabbing, Twisting, AutoTwisting, Scrambling, Solving, Painting; //the four (plus one) stages of twisting
	int scramblecount, solvecount;
	int solution[][3];
	Colour palette[6];
	float rx, ry; //current rotation of the cube as a whole
	VECTORf32 Position;
	int32 Size;
	Cubex solver;
	float twistSensitivity, rotateSensitivity;
	int controlStyle, paintColour;
	
	// these are used in picking a tile:
	void startCheck();
	void endCheck(int, int, int);

	void vectorFromSide(VECTOR&, VECTOR&, int); //gives up and right vectors for a given side
	void setDefaultColours();
	void SetRCColor(int);
	
	void DrawTile(int color, int32 x, int32 y, int32 z, int32 size, bool picking);
	void DrawSide(RubikSide, int sideNum, int32 x, int32 y, int32 z, int32 size, bool picking);
	void DrawLine(RubikLine, int32 x, int32 y, int32 z, int32 size);
	RubikSide RotateSide(RubikSide, int);
	void Grab(VECTOR);
	void Draw(touchPosition);
	void InitTwist(bool, int32 *vector);
	void TwistFromCube();
	void FinishTwist();
	void TwistAgain();
	RubikSide Side [6];
	
	public:
	RubiksCube();
	RubiksCube(float size);
	void getLayout(RubikSide*);
	void setLayout(RubikSide*);
	void getPalette(Colour*);
	void setPalette(Colour*);
	void setRotateSensitivity(int newValue);
	void setTwistSensitivity(int newValue);
	void setControlStyle(int);
	void setColour(int, int r, int g, int b);
	bool isBusy();
	void Scramble();
	void Solve();
	void Reset();
	void Tester();
	void Move(int, int);
	void Resize(float size);
	void AutoTwist(int, int, bool);
	void Update(bool, touchPosition, VECTOR, bool, int);
};

#endif
