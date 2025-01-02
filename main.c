#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <zlib.h>
#include <sys/stat.h>

#define CHUNK 16384

// Function prototypes
void compressFileWithLevel(const char *inputFile, const char *outputFile, int level);
void decompressFile(const char *inputFile, const char *outputFile);
long getFileSize(const char *filename);
void on_compress_button_clicked(GtkWidget *widget, gpointer data);
void on_decompress_button_clicked(GtkWidget *widget, gpointer data);
void on_file_chosen(GtkWidget *widget, gpointer data);
void update_progress_bar(GtkProgressBar *progressBar, double fraction);

GtkWidget *progressBar;
GtkWidget *statusLabel;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Compressor/Decompressor");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Create the box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create and add buttons
    GtkWidget *compressButton = gtk_button_new_with_label("Compress File");
    GtkWidget *decompressButton = gtk_button_new_with_label("Decompress File");
    gtk_box_pack_start(GTK_BOX(vbox), compressButton, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), decompressButton, TRUE, TRUE, 0);

    // Create progress bar
    progressBar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), progressBar, FALSE, FALSE, 0);

    // Create status label
    statusLabel = gtk_label_new("Status: Ready");
    gtk_box_pack_start(GTK_BOX(vbox), statusLabel, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(compressButton, "clicked", G_CALLBACK(on_compress_button_clicked), NULL);
    g_signal_connect(decompressButton, "clicked", G_CALLBACK(on_decompress_button_clicked), NULL);

    // Show all components
    gtk_widget_show_all(window);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();

    return 0;
}

void on_compress_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select File to Compress", NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *inputFile = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *outputFile = "compressed_file.gz";  // Hardcoded for demo purposes
        int level = Z_BEST_COMPRESSION;          // Default compression level

        compressFileWithLevel(inputFile, outputFile, level);
        g_free(inputFile);
    }
    gtk_widget_destroy(dialog);
}

void on_decompress_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select File to Decompress", NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "Cancel", GTK_RESPONSE_CANCEL,
                                                    "Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *inputFile = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        char *outputFile = "decompressed_file.txt";  // Hardcoded for demo purposes

        decompressFile(inputFile, outputFile);
        g_free(inputFile);
    }
    gtk_widget_destroy(dialog);
}

void compressFileWithLevel(const char *inputFile, const char *outputFile, int level) {
    FILE *in = fopen(inputFile, "rb");
    if (in == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *out = fopen(outputFile, "wb");
    if (out == NULL) {
        perror("Error opening output file");
        fclose(in);
        return;
    }

    z_stream strm = {0};
    if (deflateInit(&strm, level) != Z_OK) {
        fprintf(stderr, "Error initializing zlib compression.\n");
        fclose(in);
        fclose(out);
        return;
    }

    unsigned char inBuf[CHUNK];
    unsigned char outBuf[CHUNK];
    long totalFileSize = getFileSize(inputFile);
    long processedSize = 0;

    do {
        strm.avail_in = fread(inBuf, 1, CHUNK, in);
        processedSize += strm.avail_in;

        // Update the progress bar
        update_progress_bar(GTK_PROGRESS_BAR(progressBar), (double)processedSize / totalFileSize);

        if (ferror(in)) {
            fprintf(stderr, "Error reading input file.\n");
            deflateEnd(&strm);
            fclose(in);
            fclose(out);
            return;
        }

        int flush = feof(in) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = inBuf;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = outBuf;
            deflate(&strm, flush);
            fwrite(outBuf, 1, CHUNK - strm.avail_out, out);
        } while (strm.avail_out == 0);

    } while (!feof(in));

    deflateEnd(&strm);
    fclose(in);
    fclose(out);

    gtk_label_set_text(GTK_LABEL(statusLabel), "Compression complete!");
}

void decompressFile(const char *inputFile, const char *outputFile) {
    FILE *in = fopen(inputFile, "rb");
    if (in == NULL) {
        perror("Error opening input file");
        return;
    }

    FILE *out = fopen(outputFile, "wb");
    if (out == NULL) {
        perror("Error opening output file");
        fclose(in);
        return;
    }

    z_stream strm = {0};
    if (inflateInit(&strm) != Z_OK) {
        fprintf(stderr, "Error initializing zlib decompression.\n");
        fclose(in);
        fclose(out);
        return;
    }

    unsigned char inBuf[CHUNK];
    unsigned char outBuf[CHUNK];
    long totalFileSize = getFileSize(inputFile);
    long processedSize = 0;

    do {
        strm.avail_in = fread(inBuf, 1, CHUNK, in);
        processedSize += strm.avail_in;

        // Update the progress bar
        update_progress_bar(GTK_PROGRESS_BAR(progressBar), (double)processedSize / totalFileSize);

        if (ferror(in)) {
            fprintf(stderr, "Error reading input file.\n");
            inflateEnd(&strm);
            fclose(in);
            fclose(out);
            return;
        }

        if (strm.avail_in == 0)
            break;

        strm.next_in = inBuf;

        do {
            strm.avail_out = CHUNK;
            strm.next_out = outBuf;
            inflate(&strm, Z_NO_FLUSH);
            fwrite(outBuf, 1, CHUNK - strm.avail_out, out);
        } while (strm.avail_out == 0);

    } while (!feof(in));

    inflateEnd(&strm);
    fclose(in);
    fclose(out);

    gtk_label_set_text(GTK_LABEL(statusLabel), "Decompression complete!");
}

long getFileSize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    } else {
        perror("Error retrieving file size");
        return -1;
    }
}

void update_progress_bar(GtkProgressBar *progressBar, double fraction) {
    gtk_progress_bar_set_fraction(progressBar, fraction);
}
