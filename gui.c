#include "gui.h"

#define WIDTH 800

int main() {
    gtk_init(NULL, NULL);
    //loadAtStart();
    initWindow();
    gtk_main();
    return 0;
}

void showSaveDialog() {
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    GtkWidget* dlg = gtk_file_chooser_dialog_new(
        "Save in", GTK_WINDOW(window), action, "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save",GTK_RESPONSE_ACCEPT, NULL);
    gint res = gtk_dialog_run (GTK_DIALOG (dlg));

    if (res == GTK_RESPONSE_ACCEPT) {
        printf("Accept\n");
        char *filename;
        //filename = gtk_file_chooser_get_filename (chooser);
        //save_to_file (filename);
        //g_free (filename);
    }

    gtk_widget_destroy (dlg);
}

gboolean on_button_press (GtkWidget* widget,
  GdkEventButton *event, GdkWindowEdge edge)
{
  if (event->type == GDK_BUTTON_PRESS) {
    GList *list = gtk_container_get_children (widget);
    for (; list != NULL; list=list->next) {
        printf("%d\n", type(list->data));
    }
    showSaveDialog();
  }
  return FALSE;
}

void initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "FileSystem");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect_swapped(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    fixed = gtk_fixed_new();
    GdkColor color;
    color.red = 0xffff;
    color.green = 0xffff;
    color.blue = 0xffff;
    //gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
    gtk_container_add(GTK_CONTAINER(window), fixed);
    dirFiles();
    gtk_widget_show_all(window);
}

void dirFiles() {
    GtkWidget *image, *fix, *label, *eventbox;
    int row = 50, column = 50;
    struct INODE* files = dir_root_gui();
    for (int i = 0; files[i].size != -1; i++) {
        if ((column+70) > WIDTH) {
            column = 50;
            row += 80;
        }
        eventbox = gtk_event_box_new ();
        fix = gtk_fixed_new();
        gtk_fixed_put(GTK_FIXED(fixed), fix, column, row);
        gtk_widget_set_size_request(fix, 60, 80);
        image = gtk_image_new_from_icon_name("file", GTK_ICON_SIZE_DIALOG);
        gtk_container_add(GTK_CONTAINER (eventbox), image);
        gtk_fixed_put(GTK_FIXED(fix), eventbox, 10, 10);
        label = gtk_label_new(files[i].name);
        gtk_fixed_put(GTK_FIXED(fix), label, 15, 55);
        g_signal_connect(G_OBJECT(eventbox), "button_press_event",
            G_CALLBACK(on_button_press), image);
        column += 60;
    }
}

void loadAtStart() {
    char a1[80] = "/home/ad_cdvelazq/Filesystem/vdisk.h";
    char a2[80];
    int n = 0;
    a2[2] = '\0';
    for(int j=65; j<75; j++){
     a2[0] = (char) j;
     for(int i=65; i< 75; i++){
         a2[1] = (char) i;
             copyuv(a1, a2);
             n++;
             if(n == 40) break;
     }
     if(n == 40) break;
    }
}