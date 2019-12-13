#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

int i;
int completionType;

void processEvent(Display *display, Window window, XShmSegmentInfo *shminfo,
									XImage *image, int width, int height) {
	XEvent ev;
	XNextEvent(display, &ev);
	if (ev.type == completionType || ev.type == Expose) {
		printf("draw\n");
		modifyImage(image, i++);
		XShmPutImage(display, window, DefaultGC(display, 0), image, 0, 0, 0, 0,
								 width, height, True);
//		nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);

	} else if (ev.type = ButtonPress) {
		printf("destroying shared memory\n");
		XShmDetach(display, shminfo);
		XDestroyImage(image);
		shmdt(shminfo->shmaddr);
		shmctl(shminfo->shmid, IPC_RMID, 0);
		printf("done.\n");
		exit(0);
	}
}

int main() {

	printf("starting...\n");

	XImage *image;
	int width = 3*256, height = 3*256;
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

	completionType = XShmGetEventBase(display) + ShmCompletion;

	int screen = DefaultScreen(display);
	XShmSegmentInfo shminfo;

	// XImage *XShmCreateImage (display, visual, depth, format, data,
	//													shminfo,
	// width, height)
	image = XShmCreateImage(
			display, visual,
			24, // Determine correct depth from the visual. Omitted for brevity
			ZPixmap, NULL, &shminfo, width, height);
	printf("created image:\n");
	printf("%d x %d = %d\n", image->bytes_per_line, image->height,
				 image->bytes_per_line * image->height);

	shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height,
												 IPC_CREAT | 0777);
	if (shminfo.shmid < 0) {
		perror("Failed to Create Shared Memory Key\n");
		exit(0);
	}
	printf("got shared memory\n");

	shminfo.shmaddr = image->data = shmat(shminfo.shmid, 0, 0);
	shminfo.readOnly = False; // the server does not need to write to the image
														// so could be false???
	printf("created memory segment\n");

	XShmAttach(display, &shminfo);
	printf("attached to sharead memory\n");
	printf("drawing...\n");

	while (1) {
		processEvent(display, window, &shminfo, image, width, height);
	}

	return 0;
}
