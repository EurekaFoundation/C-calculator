////////////////////////////////////////////////////////////////////////
// Copyright(c) Eureka Foundation 2025
///////////////////////////////////////////////////////////////////////

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Funzione per calcolare il risultato di una semplice espressione
double evaluate_expression(const gchar *expression) {
    double result = 0;
    double current_value = 0;
    double decimal_place = 1;  // Variabile per gestire la parte decimale
    char operator = '+';
    const gchar *ptr = expression;

    while (*ptr != '\0') {
        if (isdigit(*ptr)) {
            // Se il carattere è un numero, costruisci il valore corrente
            if (decimal_place == 1) {
                current_value = current_value * 10 + (*ptr - '0');  // Parte intera
            } else {
                current_value += (*ptr - '0') / decimal_place;  // Parte decimale
                decimal_place *= 10;  // Aumenta la posizione decimale
            }
        } else if (*ptr == '.') {
            // Se incontriamo un punto, cambiamo in modalità decimale
            decimal_place = 10;  // Iniziamo a dividere per 10 per la parte decimale
        } else if (*ptr == '+' || *ptr == '-' || *ptr == '*' || *ptr == '/') {
            // Calcola il risultato con l'operatore precedente
            if (operator == '+') {
                result += current_value;
            } else if (operator == '-') {
                result -= current_value;
            } else if (operator == '*') {
                result *= current_value;
            } else if (operator == '/') {
                result /= current_value;
            }
            current_value = 0;  // Reset del valore corrente
            decimal_place = 1;  // Reset della parte decimale
            operator = *ptr;    // Nuovo operatore
        }
        ptr++;
    }
    // Ultima operazione
    if (operator == '+') {
        result += current_value;
    } else if (operator == '-') {
        result -= current_value;
    } else if (operator == '*') {
        result *= current_value;
    } else if (operator == '/') {
        result /= current_value;
    }

    return result;
}

// Funzione per gestire i click dei pulsanti
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_button_get_label(GTK_BUTTON(widget));
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(data));

    // Se il tasto è "=", calcola il risultato
    if (strcmp(text, "=") == 0) {
        double result = evaluate_expression(current_text);  // Usa il parser per calcolare l'espressione
        char result_str[100];
        snprintf(result_str, 100, "%lf", result);  // Converti il risultato in stringa
        gtk_entry_set_text(GTK_ENTRY(data), result_str);  // Mostra il risultato nell'entry
    }
    // Se il tasto è "C", resetta il display
    else if (strcmp(text, "C") == 0) {
        gtk_entry_set_text(GTK_ENTRY(data), "");
    }
    // Se il tasto è "←", elimina un singolo carattere
    else if (strcmp(text, "←") == 0) {
        const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(data));
        int length = strlen(current_text);
        if (length > 0) {
            gchar *new_text = g_strdup(current_text);
            new_text[length - 1] = '\0';  // Elimina l'ultimo carattere
            gtk_entry_set_text(GTK_ENTRY(data), new_text);
            g_free(new_text);
        }
    }
    // Altrimenti aggiungi il testo al display
    else {
        gchar *new_text = g_strconcat(current_text, text, NULL);  // Concatenazione del testo corrente con il nuovo valore
        gtk_entry_set_text(GTK_ENTRY(data), new_text);  // Imposta il nuovo testo nell'entry
        g_free(new_text);  // Libera la memoria utilizzata
    }
}

// Funzione principale
int main(int argc, char *argv[]) {
    GtkWidget *window, *grid, *entry;
    GtkWidget *button;
    GtkCssProvider *css_provider;
    GdkDisplay *display;
    GdkScreen *screen;

    // Inizializza GTK
    gtk_init(&argc, &argv);

    // Creazione della finestra principale
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calcolatrice");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Creazione della griglia
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Creazione del campo di input
    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);
    gtk_entry_set_text(GTK_ENTRY(entry), "");
    gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);  // Permette di inviare con "Enter"

    // Imposta il comportamento di ridimensionamento
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    // Imposta la griglia per ridimensionare i componenti
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Abilita il ridimensionamento del testo all'interno dell'entry
    gtk_widget_set_size_request(entry, 300, 50);  // Imposta una dimensione di base per il campo di testo
    gtk_entry_set_alignment(GTK_ENTRY(entry), 0.5);  // Centra il testo orizzontalmente

    // Carica il CSS
    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_file(css_provider, g_file_new_for_path("style.css"), NULL);

    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Definizione dei pulsanti
    const gchar *button_labels[18] = {   // Ora 18, inclusi i 17 pulsanti e il tasto "←"
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "=", "+",
        "C", "←"   // Pulsante di reset e il tasto di eliminazione
    };

    // Creazione dei pulsanti e aggiunta alla griglia
    for (int i = 0; i < 18; i++) {
        button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, 1 + (i / 4), 1, 1);

        // Imposta ogni pulsante per ridimensionarsi nella griglia
        gtk_widget_set_halign(button, GTK_ALIGN_FILL);
        gtk_widget_set_valign(button, GTK_ALIGN_FILL);
        gtk_widget_set_size_request(button, 60, 60); // Imposta una dimensione minima per i pulsanti
    }

    // Mostra tutti i widget
    gtk_widget_show_all(window);

    // Avvio del ciclo principale GTK
    gtk_main();

    return 0;
}








