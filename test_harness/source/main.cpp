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
#include <fat.h>
#include <png.h>

#include "rubikscube.h"

#define PNG_BYTES_TO_CHECK 8

rgb* backgroundTexData;

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

void initGfx()
{
	videoSetMode(MODE_5_3D | DISPLAY_BG0_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT);
	vramSetBankA(VRAM_A_MAIN_SPRITE);
	vramSetBankB(VRAM_B_TEXTURE);
	bgSetPriority(0, 1);
	lcdMainOnBottom();
}

void initSprites()
{}

void initGL()
{
	// initialize gl
	glInit();

	glEnable(GL_ANTIALIAS | GL_TEXTURE_2D);

	//set the first outline color to white
	//glSetOutlineColor(0,RGB15(0,0,0));

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

bool loadPNG(char* filename)
{
	FILE* fp;
	png_bytep* row_ptrs = NULL;

	/* Open the prospective PNG file. */
	if ((fp = fopen(filename, "rb")) == NULL)
		return false;

	if(check_if_png(fp))
	{
		//create png pointer
		png_structp png_ptr = png_create_read_struct
		(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
			return false;
		//create png info pointer
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			return false;
		}
		//set error exit point
		if (setjmp(png_jmpbuf(png_ptr)))
		{
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

		backgroundTexData = new rgb[width * height];
		for(uint i = 0; i < (width * height); i++)
		{
			rgb tmp = RGB8(pngData[i*channels], pngData[i*channels+1], pngData[i*channels+2]);
			backgroundTexData[i]=tmp;
		}
		
		free(pngData);
		free(row_ptrs);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return true;
	}
	else
	{
		fclose(fp);
		return false;
	}
}

int main(int argc, char* argv[])
{
	//RubiksCube cube;
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
	initSprites();
	initGL();
	
	fatInitDefault();
	consoleDemoInit();
	printf("Console Output:\n");
	
	int textureID;
	loadPNG((char*)"/128.png");
	
	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128 , TEXTURE_SIZE_128, 0, TEXGEN_TEXCOORD, (u8*)backgroundTexData);
	
	
	while(true)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glTranslate3f32(0, 0, floattof32(-1));
		
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK);
		
		glBindTexture(0, textureID);
		
		//draw the obj
		glBegin(GL_QUAD);

			GFX_TEX_COORD = (TEXTURE_PACK(0, inttot16(128)));
			glVertex3v16(floattov16(-0.5),	floattov16(-0.5), 0 );
	
			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128),inttot16(128)));
			glVertex3v16(floattov16(0.5),	floattov16(-0.5), 0 );
	
			GFX_TEX_COORD = (TEXTURE_PACK(inttot16(128), 0));
			glVertex3v16(floattov16(0.5),	floattov16(0.5), 0 );

			GFX_TEX_COORD = (TEXTURE_PACK(0,0));
			glVertex3v16(floattov16(-0.5),	floattov16(0.5), 0 );
		
		glEnd();
		
		glPopMatrix(1);
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
			//cube.Solve();
		}
		if(down & KEY_Y)
		{
			//cube.Scramble();
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
		//if(moving) cube.Move(dx, dy);
	
		//cube.Update(twisting, touchXY, touchVector, false, 0);
		
		glPopMatrix(1);
		
		glFlush(0);

		swiWaitForVBlank();
	}
}