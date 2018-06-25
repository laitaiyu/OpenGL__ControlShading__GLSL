
/* 

 *
 */

#ifdef WIN32
#include <windows.h>
//#include <wingdi.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include "bitmap.h"
#include <iostream>

GLubyte *LoadBitmapFile(const char *fileName,BITMAPFILEHEADER1 *bitmapFileHeader,GLubyte alpha,GLubyte r,GLubyte g,GLubyte b)
{

	FILE				*fp;
	//BITMAPFILEHEADER	bitmapFileHeader;	// Bitmap file header
	GLubyte				*bitmapImage;		// Bitmap image data
	GLubyte				*bitmapImageA;
	//unsigned int		lInfoSize;			// Size of information
	//unsigned int		lBitSize;			// Size of bitmap
	int w;
	int h;
	int offset2;

	fp = fopen(fileName, "rb");

	if (fp == NULL)
		return NULL;

	// bitmap header
	fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER1),1 , fp);

	// Check for BM
	if (bitmapFileHeader->bfType != 'MB')
	{
		fclose(fp);
		return NULL;
	}	

	w = bitmapFileHeader->biWidth;
	h = bitmapFileHeader->biHeight;
	offset2 = (4 - (w % 4)) % 4;


	bitmapImage = new BYTE[w*h*3];
	bitmapImageA = new BYTE[w*h*4];
	// Verify memory allocation
	if (!bitmapImage)
	{
		fclose(fp);
		return NULL;
	}




	int temp = 0;
	for(int i = 0;i < h;i++){
		fread( &bitmapImage[i*w*3], sizeof(unsigned char), w*3, fp );
		fseek( fp, offset2, SEEK_CUR );
	}

	for(int y = 0;y < h;y++)
		for(int x = 0;x < w;x++){
			memcpy( &bitmapImageA[(y*w+x)*4], &bitmapImage[(y*w+x)*3], 3*sizeof(unsigned char) );
			if(bitmapImageA[(y*w+x)*4] == r && bitmapImageA[(y*w+x)*4 + 1] == g && bitmapImageA[(y*w+x)*4 + 2] == b)
				bitmapImageA[(y*w+x)*4 + 3] = 0;
			else
				bitmapImageA[(y*w+x)*4 + 3] = alpha;
		}
	fclose(fp);
	delete [] bitmapImage;
	return bitmapImageA;
}
