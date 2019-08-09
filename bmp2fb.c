#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "bmp.c"

void die(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}

void printUsage(){
	printf("./bmp2fb <filename.bmp> <transparency value> <framebuffer device>\n");
	exit(1);
}


int main(int argv, char * argc[]){
	if( argv != 4 )
		printUsage();

	/*open .bmp file*/
	FILE *fin = fopen(argc[1], "rb");
	if( (fin == NULL) )
		die("fopen");

	/*transparency value*/
	unsigned char trans = atoi( argc[2] );

	if ( trans < 0 )
		trans = 0;
	if ( trans > 255 )
		trans = 255;


	/*structure*/
	struct BMPFileHeader *head = NULL;
	struct BMPBitmapHeader *bitmap = NULL;

	/*allocate memory*/
	head = malloc( sizeof(struct BMPFileHeader ));
	bitmap = malloc( sizeof(struct BMPBitmapHeader ));

	/*zero out memory*/
	memset( head, 0, sizeof(struct BMPFileHeader));
	memset( bitmap, 0, sizeof(struct BMPBitmapHeader ));

	/*read file header information*/
	readHeader( fin, head );

	/*read file bitmap information*/
	readBitmap( fin, bitmap );

/*print image bitmap information*/
#ifdef DEBUG
	printBitmap( bitmap );
#endif

	/*image picture elements*/
	struct Pixel pixels[bitmap->height][bitmap->width];

	/*dimensions*/
	unsigned int x = 0;
	unsigned int y = 0;

	/*read image pixels*/
	for( x = 0 ; x < abs(bitmap->height) ; x++ ){
		for( y = 0 ; y < bitmap->width; y++ ){
			fread( &pixels[x][y].red, 1, 1, fin);
			fread( &pixels[x][y].green, 1, 1, fin);
			fread( &pixels[x][y].blue, 1, 1, fin);
		}
	}

	/*open framebuffer device*/
	int device = open(argc[3], O_RDWR);
	if( device == -1 )
		die("error, are you root?");

	/*define structures*/
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	/*zero out structures*/
	memset( &vinfo, 0, sizeof(vinfo));
	memset( &finfo, 0, sizeof(finfo));


	/*screen properties*/
	if ( ioctl(device, FBIOGET_FSCREENINFO, &finfo) == -1)
		die("FBIOGET_FSCREENINFO");

	if ( ioctl(device, FBIOGET_VSCREENINFO, &vinfo) == -1)
		die("FBIOGET_FSCREENINFO");


	/*used for debugging purposes*/
#ifdef DEBUG	
	printf("**************\n");
	printf("id :%s\n", finfo.id);
	printf("yres :%d\n", vinfo.yres );
	printf("xres :%d\n", vinfo.xres );
	printf("bits_per_pixel :%d\n", vinfo.bits_per_pixel);
	printf("**************\n");

#endif

	/*used by mmap*/
	char *buffer = NULL;
	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	unsigned int location = 0;
	int ret = 0;

	/*map framebuffer to memory*/
	buffer = (char *)mmap( NULL, screensize, PROT_WRITE , MAP_SHARED , device , 0);
	if( buffer == MAP_FAILED )
		die("MAP_FAILED");


	/*compute maximum height*/
	unsigned int max_height = vinfo.yres;
	if( abs(bitmap->height) < max_height )
		max_height = abs(bitmap->height);

	/*compute maximum width*/
	unsigned int max_width = vinfo.xres;

	if( abs(bitmap->width) < max_width )
		max_width = abs(bitmap->width);

	/*write image pixels to framebuffer*/
	for( y = 0; y < max_height; y++ ){
		for( x = 0 ; x < max_width ; x++ ){

			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;

			*(buffer + location )    = pixels[y][x].red;
			*(buffer + location + 1) = pixels[y][x].green;
			*(buffer + location + 2) = pixels[y][x].blue;
			*(buffer + location + 3) = 0;

		}
	}

	//flush all changes to display
	vinfo.activate = FB_ACTIVATE_ALL;
	ret = ioctl( device , FBIOPUT_VSCREENINFO , &vinfo );

#ifdef DEBUG
	if( ret < 0 )
		printf("error, cannot force display\n");
#endif

	//unmap mapped memory
	munmap( buffer , screensize );

	/*free allocated memory*/
	munmap( head , sizeof( *head ));
	munmap( bitmap, sizeof(*bitmap));

	/*close file descriptors*/
	fclose(fin);
	close(device);

	return 0;
}

