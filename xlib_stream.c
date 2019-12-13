#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

// todo: figure out how to use completion events

void modifyImage(XImage *image, int offset) {
	int i, j;
	char *p = image->data;
	for (i = 0; i < image->width; i++) {
		for (j = 0; j < image->height; j++) {
			*p++ = i + offset % 256;         // blue
			*p++ = j + offset % 256;         // green
			*p++ = (256 + offset - j) % 256; // red
			p++;
		}
	}
}

int width = 3*256, height = 3*256;
unsigned char *imageData;
int i;
int completionType;
struct timespec start, end;

void processEvent(Display *display, Window window,
									XImage *image, int width, int height) {
	XEvent ev;
	XNextEvent(display, &ev);
	if (ev.type == Expose) {

		modifyImage(image, i++);

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("draw took : %d micros\n", delta);
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		XPutImage(display, window, DefaultGC(display, 0), image, 0, 0, 0, 0,
								 width, height);

	} else if (ev.type = ButtonPress) {

		printf("done.\n");
		exit(0);
	}
}

int main() {

	printf("starting...\n");

	imageData = (unsigned char *)malloc(width * height * 4);
	XImage *image;
	Display *display = XOpenDisplay(NULL);
	Visual *visual = DefaultVisual(display, 0);
	Window window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0,
																			width, height, 1, 0, 0);

	if (visual->class != TrueColor) {
		fprintf(stderr, "Your display isn't set to true color visual ...\n");
		return 0;
	}
	printf("created window\n");

	XSelectInput(display, window, ButtonPressMask | ExposureMask);
	XMapWindow(display, window);
	printf("mapped window\n");

	// XImage *XShmCreateImage (display, visual, depth, format, data,
	//													shminfo,
	// width, height)
	image = XCreateImage(
			display, visual,
			24, // Determine correct depth from the visual. Omitted for brevity
			ZPixmap, NULL, imageData, width, height, 32, width * 32 / 8);

	printf("drawing...\n");

	while (1) {
		processEvent(display, window, image, width, height);
	}

	return 0;
}
