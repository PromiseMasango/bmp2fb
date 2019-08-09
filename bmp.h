#include <stdint.h>

struct BMPFileHeader {
	uint16_t type;  // 4D42H
	uint32_t size;
	uint16_t res1;
	uint16_t res2;
	uint32_t offset;
};

struct BMPBitmapHeader{
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bpp;
	uint32_t comp;
	uint32_t bitmap;
	uint32_t hres;
	uint32_t vres;
	uint32_t colorUsed;
	uint32_t icolor;
};

struct Pixel{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
//	unsigned char trans;
};
