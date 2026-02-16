#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *button;
    GtkWidget *label;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(window), entry);

    button = gtk_button_new_with_label("Calculate");
    gtk_container_add(GTK_CONTAINER(window), button);

    label = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(window), label);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry, label);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

void on_button_clicked(GtkWidget *button, gpointer user_data)
{
    GtkWidget *entry = GTK_WIDGET(user_data);
    GtkWidget *label = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "label"));

    char *expression = gtk_entry_get_text(GTK_ENTRY(entry));
    double result = calculate_expression(expression);

    g_object_set_data(G_OBJECT(button), "label", label);
    gtk_label_set_text(GTK_LABEL(label), "Result: ");
    gtk_label_set_text(GTK_LABEL(label), "Result: ");
    gtk_label_set_text(GTK_LABEL(label), g_strdup_printf("Result: %f", result));

    g_free(expression);
}

double calculate_expression(char *expression)
{
    // Implement the calculation logic here
    // For example, let's implement a simple calculator that can add, subtract, multiply, and divide
    if (strcmp(expression, "+") == 0)
    {
        return 10 + 5;
    }
    else if (strcmp(expression, "-") == 0)
    {
        return 10 - 5;
    }
    else if (strcmp(expression, "*") == 0)
    {
        return 10 * 5;
    }
    else if (strcmp(expression, "/") == 0)
    {
        return 10 / 5;
    }
    else
    {
        // If the expression is not one of the above, return an error message
        return -1;
    }
}
