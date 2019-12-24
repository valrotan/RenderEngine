#include "visualizerTests.h"

void runVisualizerTests() {
	visInit(256, 256);

	unsigned char *pixels = visGetPixbuf();

	for (int i = 0; i < visGetHeight(); i++) {
		for (int j = 0; j < visGetWidth(); j++) {
			*pixels++ = 256 - (j % 256);
			*pixels++ = j % 256;
			*pixels++ = i % 256;
		}
	}

	visShowStill();
	assertTrue(1, "visualizer test");
}
