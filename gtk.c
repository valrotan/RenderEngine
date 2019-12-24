#include <gtk/gtk.h>
#include <time.h>

#define ROWS 256*2
#define COLS 256*2
#define BYTES_PER_PIXEL 3

void bw_to_rgb(guchar *rgb, guchar *bw, size_t sz) {
	for (size_t i = 0; i < sz; i++)
		for (size_t j = 0; j < BYTES_PER_PIXEL; j++)
			rgb[i * BYTES_PER_PIXEL + j] = bw[i];
}

char offset;

void createRGB(guchar *rgb) {
	int k = 0;
	guchar *p = rgb;
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			*(p++) = (offset + i) % 256;
			*(p++) = (offset + j) % 256;
			*(p++) = 256 - (j % 256);
		}
	}
}

struct timespec start, end;

int main(int argc, char **argv) {

	// convert to rgb (by tripling the values)
	guchar rgb[COLS*ROWS*3];
	//	bw_to_rgb(rgb, bw, ROWS * COLS);
	createRGB(rgb);

	gtk_init(NULL, NULL);

	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Image");
	gtk_window_set_default_size(GTK_WINDOW(window), COLS + 20, ROWS + 20);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	GdkPixbuf *pb;
	GtkWidget *image;

	pb = gdk_pixbuf_new_from_data(rgb,
																GDK_COLORSPACE_RGB, // colorspace (must be RGB)
																0,          // has_alpha (0 for no alpha)
																8,          // bits-per-sample (must be 8)
																COLS, ROWS, // cols, rows
																COLS * BYTES_PER_PIXEL, // rowstride
																NULL, NULL // destroy_fn, destroy_fn_data
	);
	image = gtk_image_new_from_pixbuf(pb);

	gtk_container_add(GTK_CONTAINER(window), image);
	gtk_widget_show_all(window);
	for (;; offset++) {
		clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		createRGB(rgb);
		gtk_image_set_from_pixbuf(image, pb);

		gtk_main_iteration_do(FALSE);

		clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		uint64_t delta = (end.tv_sec - start.tv_sec) * 1000000 +
										 (end.tv_nsec - start.tv_nsec) / 1000;
		printf("draw took : %d micros\n", delta);
	}

	return 0;
}
