#include "visualizer.h"

void visInit(Visualizer *v, int w, int h) {
	v->width = w;
	v->height = h;

	v->pixels = (guchar *)malloc(v->width * v->height * 3);

	gtk_init(NULL, NULL);

	v->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(v->window), "Image");
	gtk_window_set_default_size(GTK_WINDOW(v->window), v->width + 20,
															v->height + 20);
	gtk_window_set_position(GTK_WINDOW(v->window), GTK_WIN_POS_CENTER);
	g_signal_connect(v->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	v->pb =
			gdk_pixbuf_new_from_data(v->pixels,
															 GDK_COLORSPACE_RGB, // colorspace (must be RGB)
															 0,                  // has_alpha (0 for no alpha)
															 8, // bits-per-sample (must be 8)
															 v->width, v->height, // width, height
															 v->width * 3,        // heightstride
															 NULL, NULL // destroy_fn, destroy_fn_data
			);
	v->image = gtk_image_new();
	gtk_container_add(GTK_CONTAINER(v->window), v->image);
	gtk_widget_show_all(v->window);
}

void visShowStill(Visualizer *v) {
	gtk_image_set_from_pixbuf(GTK_IMAGE(v->image), v->pb);
	gtk_main();
}

void _quit_cb(GtkWidget *button, gpointer data) {
	(void)button;
	(void)data; /*Avoid compiler warnings*/
	gtk_main_quit();
	return;
}

gboolean _renderCaller(gpointer data) {

	Visualizer *v = (Visualizer *)data;
	v->render(v->pixels, v->renderParams);
	gtk_image_set_from_pixbuf(GTK_IMAGE(v->image), v->pb);
	return TRUE;
}

void visInitVideo(Visualizer *v, int w, int h,
									void (*render)(unsigned char *buf, void *param), void *params) {
	gtk_init(NULL, NULL);

	v->width = w;
	v->height = h;

	v->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(v->window), "destroy", G_CALLBACK(gtk_main_quit),
									 NULL);

	v->pixels = (unsigned char *)malloc(v->width * v->height * 3);
	v->pb =
			gdk_pixbuf_new_from_data(v->pixels,
															 GDK_COLORSPACE_RGB, // colorspace (must be RGB)
															 0,                  // has_alpha (0 for no alpha)
															 8, // bits-per-sample (must be 8)
															 v->width, v->height, // cols, rows
															 v->width * 3,        // rowstride
															 NULL, NULL // destroy_fn, destroy_fn_data
			);
	v->image = gtk_image_new_from_pixbuf(v->pb);
	//	gtk_container_add(GTK_CONTAINER(window), image);

	GtkWidget *quit_button = gtk_button_new_with_label("Quit");
	g_signal_connect(G_OBJECT(quit_button), "clicked", G_CALLBACK(_quit_cb),
									 NULL);

	GtkWidget *box = gtk_box_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(v->window), box);
	gtk_box_pack_start(GTK_BOX(box), v->image, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(box), quit_button, 0, 0, 0);

	v->render = render;
	v->renderParams = params;
}

void visShowVideo(Visualizer *v) {
	gtk_widget_show_all(v->window);
	g_timeout_add(200, _renderCaller, v);
	gtk_main();
}
