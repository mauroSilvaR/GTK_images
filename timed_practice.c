#include <gtk/gtk.h>
#include <stdio.h>

//definition of mouse events and their specific physhical buttons
#define LEFT_MOUSE_BUTTON    1
#define MIDDLE_MOUSE_BUTTON  2
#define RIGHT_MOUSE_BUTTON   3
#define SCROLL_WHEEL_UP      4
#define SCROLL_WHEEL_DOWN    5


// Variables to track the starting position and the initial scroll position
static gboolean is_dragging = FALSE;
static int start_x = 0, start_y = 0;
static int initial_hadjustment = 0, initial_vadjustment = 0;


//handler for label text change, TODO: break this down to be a "print" like function instead of all this logic
static gboolean update_label(gpointer data) 
{
    static int counter = 0;
    int limit = 100;
    char str[250];


    counter++;
    if(counter>= limit)
    {
        counter = 0;
    }

    sprintf(str, "New Time: %d",counter);
    GtkWidget *label = GTK_WIDGET(data);

    // Set the label text
    gtk_label_set_text(GTK_LABEL(label), str);

    // Return TRUE to continue the timeout
    return TRUE;
}

//hanler for right mouse button press events,  the exact button is also defined by the function
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) 
{
    if (event->button == RIGHT_MOUSE_BUTTON) 
    {  
        is_dragging = TRUE;
        start_x = event->x;
        start_y = event->y;

        GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(user_data));
        GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(user_data));

        initial_hadjustment = gtk_adjustment_get_value(hadj);
        initial_vadjustment = gtk_adjustment_get_value(vadj);

        return TRUE;  // Event handled
    }
    return FALSE;  // Event not handled
}

//handler for movement on screen
static gboolean on_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) 
{
    if (is_dragging) 
    {
        GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(user_data));
        GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(user_data));

        int dx = start_x - event->x;
        int dy = start_y - event->y;

        gtk_adjustment_set_value(hadj, initial_hadjustment + dx);
        gtk_adjustment_set_value(vadj, initial_vadjustment + dy);

        return TRUE;  // Event handled
    }
    return FALSE;  // Event not handled
}

//handler for release of the button
static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->button == RIGHT_MOUSE_BUTTON) 
    {  
        is_dragging = FALSE;
        return TRUE;  // Event handled
    }
    return FALSE;  // Event not handled
}



int main(int argc, char *argv[]) 
{
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Timed_practice");
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 760);

    // Create a grid  container
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create a label for the top text
    GtkWidget *label = gtk_label_new("Image Display");

    // Create a button widgets to allow changing image
    GtkWidget *next = gtk_button_new_with_label(">>>");
    GtkWidget *previous = gtk_button_new_with_label("<<<");

    // Create an image widget
    GtkWidget *image = gtk_image_new_from_file("file_1.png"); // Replace with the path to your image file

    //Create a scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // Ensure the scrolled window expands
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    // add the image to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), image);

    //after creating the widgets add then to the grid at desired positions
    gtk_grid_attach(GTK_GRID(grid), label,              1, 0, 1, 1); // (column, row, width, height)
    gtk_grid_attach(GTK_GRID(grid), next,               2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), previous,           0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window,    1, 1, 1, 1);


//now we connect the necessary signals, destroy signal is separated for now
//---------------------------------------------------------------------------------------------------
    // Connect mouse events for scrolling
    g_signal_connect(scrolled_window, "button-press-event", G_CALLBACK(on_button_press), scrolled_window);
    g_signal_connect(scrolled_window, "motion-notify-event", G_CALLBACK(on_motion_notify), scrolled_window);
    g_signal_connect(scrolled_window, "button-release-event", G_CALLBACK(on_button_release), scrolled_window);

    // Enable mouse events
    gtk_widget_add_events(scrolled_window, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
//---------------------------------------------------------


    // Set up a timeout function to update the label text every second (1000 milliseconds)
    g_timeout_add(1000, update_label, label);

    // Connect the "destroy" signal to quit the GTK main loop
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets within the window
    gtk_widget_show_all(window);

    // Enter the GTK main loop
    gtk_main();

    return 0;
}








