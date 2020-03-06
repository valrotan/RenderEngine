#pragma once

#include <gdk/gdk.h>
#include <gtk/gtk.h>

typedef struct {
	int height;
	int width;

	unsigned char *pixels;

	GdkPixbuf *pb;
	GtkWidget *image;
	GtkWidget *window;

	void (*render)(unsigned char *buf, void *param);
	void *renderParams;
} Visualizer;

void visInit(Visualizer *v, int w, int h);
void visShowStill(Visualizer *v);

void visInitVideo(Visualizer *v, int w, int h,
									void render(unsigned char *buf, void *param), void *params);
void visShowVideo(Visualizer *v);
