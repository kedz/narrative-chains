#ifndef CORENLP_CNTR_OPTS_H
#define CORENLP_CNTR_OPTS_H

typedef struct opt_s {
    void *data;
    void (* _xml_file_handler) (gchar* path, gpointer data);
    void (* _x_gzip_file_handler) (gchar* path, gpointer data);
    void (* _finish_processing) (gpointer data);
    void (* _free) (gpointer data);
} opt_s;

#endif

