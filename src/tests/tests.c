#include <stdio.h>

#include "renderMathTests.h"
#include "rendererTests.h"
#include "visualizerTests.h"

int main() {

	printf("Runnning tests...\n");

	printf("Running math tests...\n");
	runMathTests();

	printf("Running renderer tests...\n");
	runRendererTests();

//	printf("Running visualizer tests...\n");
//	runVisualizerTests();

	printf("Finished testing.\n");

	return 0;
}
