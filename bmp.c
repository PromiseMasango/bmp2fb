#include <stdio.h>
#include "bmp.h"

/* function declaration */
void readHeader(FILE *file, struct BMPFileHeader *h);
void printBitmap( struct BMPBitmapHeader *h);
void readBitmap(FILE *file ,struct BMPBitmapHeader *h);

/* Print image information. Use for debugging purposes*/
void printBitmap( struct BMPBitmapHeader *h){
	printf("height :%d width :%d\n", h->height, h->width );
	printf("bpp :%d\n", h->bpp );
}

/* Read image header information from a .bmp file*/
void readHeader(FILE *file, struct BMPFileHeader *h){
	fread( &h->type, 2 , 1 , file );
	fread( &h->size, 4 , 1 , file );
	fread( &h->res1, 2 , 1 , file );
	fread( &h->res2 ,2 , 1 , file );
	fread( &h->offset , 4 , 1 , file );
}

/* Read image bitmap information from a .bmp file*/
void readBitmap(FILE *file ,struct BMPBitmapHeader *h){
	fread( &h->size , 4, 1, file );
	fread( &h->width , 4 , 1 , file );
	fread( &h->height , 4 , 1 , file );
	fread( &h->planes , 2 , 1 , file );
	fread( &h->bpp , 2 , 1 , file );
	fread( &h->comp , 4 , 1 , file );
	fread( &h->bitmap , 4 , 1 , file );
	fread( &h->hres , 4 , 1 , file );
	fread( &h->vres , 4 , 1 , file );
	fread( &h->colorUsed ,4 , 1 , file );
	fread( &h->icolor , 4 , 1 , file );
}
