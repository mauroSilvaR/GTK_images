#include <gtk/gtk.h>
#include <stdio.h>

//definition of mouse events and their specific physhical buttons
#define LEFT_MOUSE_BUTTON    1
#define MIDDLE_MOUSE_BUTTON  2
#define RIGHT_MOUSE_BUTTON   3
#define SCROLL_WHEEL_UP      4
#define SCROLL_WHEEL_DOWN    5

//defining ZOOM factor to limit range of zoom
#define ZOOM_FACTOR_MIN 0.3  // TODO: this can be taken by an XML
#define ZOOM_FACTOR_MAX 10.0 // TODO: this can be taken by an XML


// Variables to track the starting position and the initial scroll position
static gboolean is_dragging = FALSE;
static gboolean image_is_present = FALSE;
static int start_x = 0, start_y = 0;
static int initial_hadjustment = 0, initial_vadjustment = 0;

//main label to be used in the top text
GtkWidget *main_label;

//structure to hold our application data
typedef struct 
{
    GtkWidget *image;
    GdkPixbuf *original_pixbuf;
    double zoom_factor;
} AppData;

//funtion to print to a label
// parameters*****
//data the pointer to the label you want to print
//char str[250] the string you want to print to that label
static void print_to_main_label(GtkLabel *label, const gchar *new_text)
{
    // Set the label text
    gtk_label_set_text(GTK_LABEL(label), new_text);
}

//function to check if an image was loaded
// the function just checks if the widget is null or not, when an image widget is null that indicates no image is present
// if no image is present after you attempted to load it that means that it wasnt loaded
gboolean check_if_image_was_loaded(GtkImage *image)
{
    if(image == NULL)
    {
        image_is_present = FALSE;
        return FALSE;
    }
    else
    {
        image_is_present = TRUE;
        return TRUE;
    }
}

//handler for label text change, TODO: break this down to be a perpetual loop, somewhat like
// the update functions on frame change in game engines
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


    //only print to the top layer if the image was loaded
    sprintf(str, "New Time: %d",counter);
    print_to_main_label(data,str);




    // Return TRUE to continue the timeout
    return TRUE;
}

//function to treat right mouse button events
gboolean treat_right_mouse_button(gpointer user_data,GdkEventButton *event)
{
        is_dragging = TRUE;
        start_x = event->x;
        start_y = event->y;

        GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(user_data));
        GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(user_data));

        initial_hadjustment = gtk_adjustment_get_value(hadj);
        initial_vadjustment = gtk_adjustment_get_value(vadj);
        return TRUE; // Event handled
}

//handler for right mouse button press events,  the exact button is also defined by the function
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data) 
{
    //variable to hold processing status of the on_button_press
    char status;

    switch(event->button)
    {
        case RIGHT_MOUSE_BUTTON:
        {
            //treat button, user wants to move image
            status = treat_right_mouse_button(user_data,event);
            break;
        }
        case LEFT_MOUSE_BUTTON:
        {
            //no function to this button yet
            status = TRUE;
            break;
        }
        default:
        {
            status = FALSE;  // Event not handled
            break;
        }
    }    

    return status;
}

//handler for release of the button
static gboolean on_button_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data) 
{
    //variable to hold processing status of the on_button_press
    char status;

    switch(event->button)
    {
        case RIGHT_MOUSE_BUTTON:
        {  
            //update flag, user doesn't want to move flag anymore
            is_dragging = FALSE;  
            //  Event handled
            status = TRUE;       
            break;
        }
        case LEFT_MOUSE_BUTTON:
        {
            //no function to this button yet,treat event as if was done
            status = TRUE;
        }
        default:
        {
            status = FALSE;
            break;
        }
    }

    return status;
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


//function to allow zoom in and out on the image
static void update_image_with_zoom(AppData *app_data) 
{
    //get the width and height using standardgdk functions and multiply them by the zoom factor
    int width = gdk_pixbuf_get_width(app_data->original_pixbuf) * app_data->zoom_factor;
    int height = gdk_pixbuf_get_height(app_data->original_pixbuf) * app_data->zoom_factor;

    // Ensure width and height are greater than 0
    if (width > 0 && height > 0) 
    {
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(app_data->original_pixbuf, width, height, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(app_data->image), scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
    }   

}

//signal handler for the keyboard events
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) 
{
    //pass the adress of the g_pointer user data to an AppData pointer
    AppData *app_data = (AppData *)data;

    //here you can add more events if you'd like
    // Check for 'z' key (zoom in) or 'm' key (zoom out)
    if (event->keyval == GDK_KEY_z) 
    {
        //changes the zoom factor, increase it by 10%
        app_data->zoom_factor *= 1.05; // Zoom in
    }
    else if (event->keyval == GDK_KEY_m) 
    {
        //changes the zoom factor, decrease it by 10%
        app_data->zoom_factor /= 1.05; // Zoom out
    }

    //Clamp zoom factor within defined bounds
    app_data->zoom_factor = MAX(ZOOM_FACTOR_MIN, MIN(ZOOM_FACTOR_MAX, app_data->zoom_factor));

    //apply the zoom function
    update_image_with_zoom(app_data);

    return TRUE; // Event handled
}


int main(int argc, char *argv[]) 
{
    //flag for error handling, true when ok false when error is present
    //init as true( no error )
    gboolean error_flag = TRUE;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Timed_practice");
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 760);

    //initialize a variable with the struct appdata 
    AppData app_data;

    // Create a grid  container
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create a label for the top text
    main_label = gtk_label_new("Image Display");

    // Create a button widgets to allow changing image
    GtkWidget *next = gtk_button_new_with_label(">>>");
    GtkWidget *previous = gtk_button_new_with_label("<<<");

    //set a default zoom factor for the image
    app_data.zoom_factor = 1.0;
    //load the pixel data of the image using gdk
    app_data.original_pixbuf = gdk_pixbuf_new_from_file("file_1.png", NULL);
    //if the data couldn't be loaded display the error
    if (app_data.original_pixbuf == NULL) 
    {
        g_printerr("Error loading image\n");
        return 1;
    }

    // Create an image widget with the image from the pix buffer
    app_data.image = gtk_image_new_from_pixbuf(app_data.original_pixbuf);
    

    //Create a scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    // Ensure the scrolled window expands
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    //create and capture the focus to allow keyboard events on a scrolled window
    gtk_widget_set_can_focus(scrolled_window, TRUE);
    // add the image to the scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), app_data.image);

    //after creating the widgets add then to the grid at desired positions
    gtk_grid_attach(GTK_GRID(grid), main_label,         1, 0, 1, 1); // (column, row, width, height)
    gtk_grid_attach(GTK_GRID(grid), next,               2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), previous,           0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window,    1, 1, 1, 1);


//now we connect the necessary signals, destroy signal is separated for now
//---------------------------------------------------------------------------------------------------
    // Connect mouse events for scrolling
    g_signal_connect(scrolled_window, "button-press-event", G_CALLBACK(on_button_press), scrolled_window);
    g_signal_connect(scrolled_window, "motion-notify-event", G_CALLBACK(on_motion_notify), scrolled_window);
    g_signal_connect(scrolled_window, "button-release-event", G_CALLBACK(on_button_release), scrolled_window);

    //keyboard events on the scrolled window need have focus to allow keyboard events
    gtk_widget_grab_focus(scrolled_window);
    g_signal_connect(scrolled_window, "key-press-event", G_CALLBACK(on_key_press), &app_data);

    // Enable mouse events
    gtk_widget_add_events(scrolled_window, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
//---------------------------------------------------------------------------------------------------

    // Set up a timeout function to update the label text every second (1000 milliseconds)
    g_timeout_add(1000, update_label, main_label);

    // Connect the "destroy" signal to quit the GTK main loop
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets within the window
    gtk_widget_show_all(window);

    // Enter the GTK main loop
    gtk_main();

    g_object_unref(app_data.original_pixbuf);

    return 0;
}


