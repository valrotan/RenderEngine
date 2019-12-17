#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CAMERA_WIDTH 512
#define CAMERA_HEIGHT 512

unsigned char *rgbImage;
unsigned char offset;
struct timespec start, end;
GtkWidget *image;

void createRGB() {
	int k = 0;
	unsigned char *p = rgbImage;
	for (int i = 0; i < CAMERA_WIDTH; i++) {
		for (int j = 0; j < CAMERA_HEIGHT; j++) {
			*(p++) = (offset + i) % 256;
			*(p++) = (offset + j) % 256;
			*(p++) = 256 - (j % 256);
		}
	}
}

int loadImage(unsigned char *data) {
	printf("Got image!\n");
	GdkPixbuf *pixbuf =
			gdk_pixbuf_new_from_data(data, GDK_COLORSPACE_RGB, FALSE, 8, CAMERA_WIDTH,
															 CAMERA_HEIGHT, CAMERA_WIDTH * 3, NULL, NULL);
	gtk_image_set_from_pixbuf((GtkImage *)image, pixbuf);
	gtk_widget_queue_draw(image);
	printf("Loaded\n");

	return 0;
}

int main(int argc, char *argv[]) {
	GtkWidget *window;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Image2");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(window), 2);

	image = gtk_image_new();

	gtk_container_add(GTK_CONTAINER(window), image);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
									 NULL);

	gtk_widget_show_all(window);

	rgbImage = (unsigned char *)malloc(CAMERA_WIDTH * CAMERA_HEIGHT * 3);
	createRGB();
	loadImage(rgbImage);

	for (;;offset++) {
		createRGB();
		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
		printf("draw took : %d micros\n", delta);
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		gtk_main_iteration();

	}

	return 0;
}
