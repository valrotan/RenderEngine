#include "visualizer.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

int height;
int width;
int const BYTES_PER_PIXEL = 3;

guchar* pixels;

GdkPixbuf *pb;
GtkWidget *image;

void visInit(int w, int h) {
	width = w;
	height = h;

	pixels = (guchar*) malloc(width * height * BYTES_PER_PIXEL);

	gtk_init(NULL, NULL);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Image");
	gtk_window_set_default_size(GTK_WINDOW(window), width + 20, height + 20);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_container_add(GTK_CONTAINER(window), image);
	gtk_widget_show_all(window);
}

// TODO
// void visShowFrame(void);

void visShowStill() {
	pb = gdk_pixbuf_new_from_data(pixels,
																GDK_COLORSPACE_RGB, // colorspace (must be RGB)
																0,          // has_alpha (0 for no alpha)
																8,          // bits-per-sample (must be 8)
																width, height, // width, height
																width * BYTES_PER_PIXEL, // heighttride
																NULL, NULL // destroy_fn, destroy_fn_data
	);
	gtk_image_set_from_pixbuf(image, pb);
	gtk_main();
}

unsigned char *visGetPixbuf() {
	return (unsigned char*) pixels;
}

int visGetWidth() {
	return width;
}

int visGetHeight() {
	return height;
}
