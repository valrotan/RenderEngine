#include "engine.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

	RenderArgs args = engineDefaultArgs();

	for (int i = 1; i < argc; i += 2) {
		if (strcmp(argv[i], "-help") == 0) {
			printf("Simple Render Engine in C \n"
						 "\n"
						 "-h/-help  -  print help menu \n"
						 "\n");

		} else if (strcmp(argv[i], "-input") == 0 || strcmp(argv[i], "-i") == 0) {
			args.objPath = argv[i + 1];

		} else if (strcmp(argv[i], "-output") == 0 || strcmp(argv[i], "-o") == 0) {
			args.outputPath = argv[i + 1];

		} else if (strcmp(argv[i], "-height") == 0 || strcmp(argv[i], "-h") == 0) {
			args.resolution.y = strtol(argv[i + 1], NULL, 10);

		} else if (strcmp(argv[i], "-modelRotateX") == 0 ||
							 strcmp(argv[i], "-mx") == 0) {
			args.modelRotation.x = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-modelRotateY") == 0 ||
							 strcmp(argv[i], "-my") == 0) {
			args.modelRotation.y = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-cameraRotateX") == 0 ||
							 strcmp(argv[i], "-cx") == 0) {
			args.cameraRotation.x = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-cameraRotateY") == 0 ||
							 strcmp(argv[i], "-cy") == 0) {
			args.cameraRotation.y = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-cameraRotateZ") == 0 ||
							 strcmp(argv[i], "-cz") == 0) {
			args.cameraRotation.z = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-width") == 0 || strcmp(argv[i], "-w") == 0) {
			args.resolution.x = strtol(argv[i + 1], NULL, 10);

		} else if (strcmp(argv[i], "-fov") == 0 || strcmp(argv[i], "-f") == 0) {
			args.fov = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-scale") == 0 || strcmp(argv[i], "-s") == 0) {
			args.scale = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-bkgR") == 0 || strcmp(argv[i], "-br") == 0) {
			args.bkgColor.x = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-bkgG") == 0 || strcmp(argv[i], "-bg") == 0) {
			args.bkgColor.y = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-bkgB") == 0 || strcmp(argv[i], "-bb") == 0) {
			args.bkgColor.z = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-ambient") == 0 || strcmp(argv[i], "-b") == 0) {
			args.ambientLight = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-kspecularExponent") == 0 ||
							 strcmp(argv[i], "-c") == 0) {
			args.kSpecularExponent = (int)strtol(argv[i + 1], NULL, 10);

		} else if (strcmp(argv[i], "-kemmision") == 0 ||
							 strcmp(argv[i], "-ke") == 0) {
			args.baseTriangle.k_e = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-kdiffuse") == 0 ||
							 strcmp(argv[i], "-kd") == 0) {
			args.baseTriangle.k_d = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-kspecular") == 0 ||
							 strcmp(argv[i], "-ks") == 0) {
			args.baseTriangle.k_s = strtod(argv[i + 1], NULL);

		} else if (strcmp(argv[i], "-aliasSamples") == 0 ||
							 strcmp(argv[i], "-a") == 0) {
			args.nAntialiasingSamples = (int)strtol(argv[i + 1], NULL, 10);

		} else if (strcmp(argv[i], "-traces") == 0 || strcmp(argv[i], "-t") == 0) {
			args.nTraces = (int)strtol(argv[i + 1], NULL, 10);

		} else if (strcmp(argv[i], "-threads") == 0 || strcmp(argv[i], "-n") == 0) {
			args.nThreads = (int)strtol(argv[i + 1], NULL, 10);

		} else {
			printf("Unknown option: %s\n", argv[i]);
			return 0;
		}
	}

	engineRun(&args);
}
