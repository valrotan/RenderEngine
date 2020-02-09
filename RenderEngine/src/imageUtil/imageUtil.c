#include "imageUtil.h"
#include <stdio.h>

int saveToTGA(const char *filename, unsigned char *pbuf, u_int16_t w, u_int16_t h) {
	FILE *tga = fopen(filename, "wb");
	if (tga == 0) {
		return 0;
	}

	char header[18] = {0,0,2, // image type
										 0,0,0,0,0, // color map
										0,0,0,0, // x,y
										(char) (w & 0xff), (char) (w >> 8),
										(char) (h & 0xff), (char) (h >> 8),
										24, 0b00100000};
	fwrite(header, sizeof(char), 18, tga);

	unsigned char *p = pbuf;
	for (int i = 0; i < w * h; i++) {
		fwrite(p + 2, 1, 1, tga);
		fwrite(p + 1, 1, 1, tga);
		fwrite(p + 0, 1, 1, tga);
		p += 3;
	}

	fclose(tga);
	return 1;
}
