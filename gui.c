#include "gui.h"

#define WIDTH 800

int main() {
    gtk_init(NULL, NULL);
    //loadAtStart();
    initWindow();
    gtk_main();
    return 0;
}

void initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "FileSystem");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect_swapped(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    fixed = gtk_fixed_new();
    color.red = 0xffff;
    color.green = 0xffff;
    color.blue = 0xffff;
    color.alpha = 1.0;
    gtk_widget_override_background_color(window, GTK_STATE_NORMAL, &color);
    gtk_container_add(GTK_CONTAINER(window), fixed);
    menu = gtk_menu_new();
    mnew = gtk_menu_item_new_with_label("Nuevo");
    g_signal_connect(mnew, "activate", (GCallback) onNew, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mnew);
    mopen = gtk_menu_item_new_with_label("Abrir");
    g_signal_connect(mopen, "activate", (GCallback) onOpen, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mopen);
    mcopy = gtk_menu_item_new_with_label("Copiar de vd a vd");
    g_signal_connect(mcopy, "activate", (GCallback) onCopyVDtoVD, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mcopy);
    mcopy_from_linux = gtk_menu_item_new_with_label("Copiar desde linux");
    g_signal_connect(mcopy_from_linux, "activate", (GCallback) onCopyFromLinux, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mcopy_from_linux);
    mcopy_from_vd = gtk_menu_item_new_with_label("Copiar desde vd");
    g_signal_connect(mcopy_from_vd, "activate", (GCallback) onCopyFromVD, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mcopy_from_vd);
    mdelete = gtk_menu_item_new_with_label("Eliminar");
    g_signal_connect(mdelete, "activate", (GCallback) onDelete, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mdelete);
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
        image = gtk_image_new_from_icon_name("text-plain", GTK_ICON_SIZE_DIALOG);
        gtk_widget_override_background_color(eventbox, GTK_STATE_NORMAL, &color);
        gtk_container_add(GTK_CONTAINER (eventbox), image);
        gtk_fixed_put(GTK_FIXED(fix), eventbox, 10, 10);
        label = gtk_label_new(files[i].name);
        gtk_widget_set_name(eventbox, files[i].name);
        gtk_fixed_put(GTK_FIXED(fix), label, 15, 55);
        g_signal_connect(G_OBJECT(eventbox), "button_press_event",
            G_CALLBACK(on_button_press), image);
        column += 100;
    }
    gtk_widget_show_all(fixed);
}

void loadAtStart() {
    char a1[80] = "/home/cristian/Dropbox/ITESO/Otoño 2014/SO/Práctica 8/vdisk.h";
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

void onOpen(GtkWidget *menuitem, gpointer userdata) {
    char *buffer;
    int size = 0;
    catv(selected_file, &buffer, &size);
    newEditor(buffer, size);
}

gboolean on_button_press (GtkWidget* widget, GdkEventButton *event,
    GdkWindowEdge edge) {
        if (event->type == GDK_BUTTON_PRESS) {
            if (event->button == 3) {
                strcpy(selected_file, gtk_widget_get_name(widget));
                gtk_menu_popup (GTK_MENU(menu), NULL, NULL, NULL, NULL, 0,
                    gtk_get_current_event_time());
                gtk_widget_show_all(menu);
            }
        }
        return FALSE;
}

GtkWidget* getInputDialog(char title[], char msg[], GtkWidget *entry) {
    GtkWidget *dialog, *content_area, *label;
    dialog = gtk_dialog_new_with_buttons ("Nuevo archivo",  GTK_WINDOW(window),
        GTK_DIALOG_MODAL, "_Aceptar", GTK_RESPONSE_ACCEPT, "_Cancelar",
        GTK_RESPONSE_REJECT, NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
    label = gtk_label_new("Nombre del archivo");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    return dialog;
}

void onNew(GtkWidget *menuitem, gpointer userdata) {
    GtkWidget *entry = entry = gtk_entry_new();
    GtkWidget *dialog = getInputDialog("Nuevo archivo", "Nombre del archivo", entry);
    g_signal_connect(dialog, "response", G_CALLBACK (onNewResponse),
        entry);
    gtk_widget_show_all (dialog);
}

void onNewResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == -3) {
        GtkWidget *entry = user_data;
        int size = gtk_entry_get_text_length(GTK_ENTRY(entry));
        if (size == 0 || size > 19) {
            gtk_widget_destroy(GTK_WIDGET(dialog));
            showMessage("Nombre no cumple con las reglas");
        } else {
            vdcreat((char*)gtk_entry_get_text(GTK_ENTRY(entry)), 0);
            update();
        }        
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void onDelete(GtkWidget *menuitem, gpointer userdata) {
    GtkWidget *dialog, *content_area, *label;
    dialog = gtk_dialog_new_with_buttons ("Eliminar archivo",  GTK_WINDOW(window),
        GTK_DIALOG_MODAL, "_Aceptar", GTK_RESPONSE_ACCEPT, "_Cancelar",
        GTK_RESPONSE_REJECT, NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
    label = gtk_label_new("¿Seguro que desea eliminarlo?");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    g_signal_connect(dialog, "response", G_CALLBACK (onDeleteResponse), NULL);
    gtk_widget_show_all (dialog);
}

void onDeleteResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == -3) {
        vdunlink(selected_file);
        update();
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void showMessage(char msg[]) {
    GtkWidget *dialog, *content_area, *label, *entry;
    dialog = gtk_dialog_new_with_buttons ("INFO",  GTK_WINDOW(window),
        GTK_DIALOG_MODAL, "_OK", GTK_RESPONSE_ACCEPT, NULL);
    content_area = gtk_dialog_get_content_area (GTK_DIALOG(dialog));
    label = gtk_label_new(msg);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    g_signal_connect_swapped (dialog, "response", G_CALLBACK (gtk_widget_destroy),
        dialog);
    gtk_widget_show_all (dialog);
}

void newEditor(char buffer[], int size) {
    GtkWidget *edit = gtk_window_new(GTK_WINDOW_TOPLEVEL),
        *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0),
        *view = gtk_text_view_new(), *menubar = gtk_menu_bar_new(),
        *filemenu = gtk_menu_new(), *file = gtk_menu_item_new_with_label("Archivo"),
        *save = gtk_menu_item_new_with_label("Guardar");
    GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    GtkTextBuffer *tbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GError *error = NULL;
    gtk_text_buffer_insert_at_cursor(tbuffer, buffer, size);
    gtk_window_set_title(GTK_WINDOW(edit), "Editor");
    gtk_window_set_default_size(GTK_WINDOW(edit), WIDTH, 600);
    gtk_window_set_position(GTK_WINDOW(edit), GTK_WIN_POS_CENTER);
    g_signal_connect_swapped(G_OBJECT(edit), "destroy",
        G_CALLBACK(gtk_widget_destroy), edit);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);
    gtk_container_add(GTK_CONTAINER(edit), vbox);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 3);
    gtk_container_add(GTK_CONTAINER(scrolledwindow), view);
    g_signal_connect(save, "activate", (GCallback) onSave, tbuffer);
    gtk_widget_show_all(edit);
}

void onSave(GtkWidget *menuitem, gpointer userdata) {
    GtkTextBuffer *tbuffer = userdata;
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(tbuffer, &start);
    gtk_text_buffer_get_end_iter(tbuffer, &end);
    gchar *buff;
    buff = (char *) gtk_text_buffer_get_text(tbuffer, &start, &end, FALSE);
    saveEdit(selected_file, buff);
}

void update() {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(fixed));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
    dirFiles();
}

void onCopyFromLinux(GtkWidget *menuitem, gpointer userdata) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Copiar archivo", GTK_WINDOW(window),
        action, "_Cancelar", GTK_RESPONSE_CANCEL, "_Copiar",
        GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        cpy_name = filename;
        GtkWidget *entry = entry = gtk_entry_new();
        GtkWidget *dialog = getInputDialog("Copia de archivo desde Linux", "Nombre del archivo en VD", entry);
        g_signal_connect(dialog, "response", G_CALLBACK (onCopyFromLinuxResponse),
            entry);
        gtk_widget_show_all (dialog);
    }

    gtk_widget_destroy (dialog);
}

void onCopyFromLinuxResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    GtkWidget *entry = user_data;
    int size = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if (size == 0 || size > 19) {
        gtk_widget_destroy(GTK_WIDGET(dialog));
        showMessage("Nombre no cumple con las reglas");
    } else {
        copyuv(cpy_name, (char*) gtk_entry_get_text(GTK_ENTRY(entry)));
        update();
    }
    g_free (cpy_name);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void onCopyFromVD(GtkWidget *menuitem, gpointer userdata) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Guardar archivo", GTK_WINDOW(window),
        action, "_Guardar", GTK_RESPONSE_ACCEPT, "_Cancelar",
        GTK_RESPONSE_CANCEL, NULL);
    chooser = GTK_FILE_CHOOSER (dialog);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename (chooser);
        copyvu(selected_file, filename);
        g_free (filename);
    }

    gtk_widget_destroy (GTK_WIDGET(dialog));
}

void onCopyVDtoVD(GtkWidget *menuitem, gpointer userdata) {
    GtkWidget *entry = entry = gtk_entry_new();
    GtkWidget *dialog = getInputDialog("Copia de archivo VD a VD", "Nombre del archivo en VD", entry);
    g_signal_connect(dialog, "response", G_CALLBACK(onCopyVDtoVDResponse),
        entry);
    gtk_widget_show_all (dialog);
}

void onCopyVDtoVDResponse(GtkDialog *dialog, gint response_id, gpointer user_data) {
    GtkWidget *entry = user_data;
    int size = gtk_entry_get_text_length(GTK_ENTRY(entry));
    if (size == 0 || size > 19) {
        gtk_widget_destroy(GTK_WIDGET(dialog));
        showMessage("Nombre no cumple con las reglas");
    } else {
        copyvv(selected_file, (char*) gtk_entry_get_text(GTK_ENTRY(entry)));
        update();
    }
    g_free (cpy_name);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}