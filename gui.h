#include <gtk/gtk.h>
#include "gui_utils.h"

GtkWidget *window;
GtkWidget *fixed;
GtkWidget *menu, *mnew, *mopen, *mdelete, *mcopy, *mcopy_from_linux, *mcopy_from_vd;
GdkRGBA color;
char selected_file[20];
char *cpy_name;
GtkTextBuffer *buffEdit;

void initWindow();
void dirFiles();
GtkWidget* getInputDialog(char title[], char msg[], GtkWidget *entry);
void loadAtStart();
void newEditor(char buffer[], int size);
void onCopyFromLinux(GtkWidget *menuitem, gpointer userdata);
void onCopyFromLinuxResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void onCopyFromVD(GtkWidget *menuitem, gpointer userdata);
void onCopyVDtoVD(GtkWidget *menuitem, gpointer userdata);
void onCopyVDtoVDResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void onDelete(GtkWidget *menuitem, gpointer userdata);
void onDeleteResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void onNew(GtkWidget *menuitem, gpointer userdata);
void onNewResponse(GtkDialog *dialog, gint response_id, gpointer user_data);
void onOpen(GtkWidget *menuitem, gpointer userdata);
void onSave(GtkWidget *menuitem, gpointer userdata);
gboolean on_button_press (GtkWidget* widget, GdkEventButton *event, GdkWindowEdge edge);
void showMessage(char msg[]);
void update();