#include "apps/corenlp-counter/chambers-nc-counts.h"

void _chambers_nc_xml_file_handler(
    gchar *path, 
    gpointer data
) {
    //ncc_data_t *ncc_data = (ncc_data_t *) data;

    document_t *doc = NULL;
    doc = cu_build_cnlp_docs_file (path);
        
    if (doc!=NULL) {
        chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;         
        cu_nc_prot_model_count(counts, doc);
        cu_document_free (&doc, TRUE);

    }
    free (path);

} 

void
_chambers_nc_tar_file_handler(
    gchar *path,
    gpointer data)
{

    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;  

    int read = 0;
    int pad = 0;
    char header[512];
    char *file_buffer;
    int size;
    gzFile fp = NULL;


    fp = gzopen(path, "r"); 
    if (fp == NULL) {
        fprintf (stderr, "Error reading file: %s\n", path);
        return;
    }

    for (;;)
    {

        if (gzread(fp, header, 512) != 512) {
            break;
        }

        if (header[0] == '\0') {
            break;    
        }   

        printf ("Doc filename: %s\n", header); 

        size = octal_string_to_int(&header[124], 11);
        pad = (512 - (size % 512)) % 512;

        file_buffer = (char *) malloc(sizeof(char) * size);
        if (gzread(fp, file_buffer, size) != size) {
            fprintf (stderr, "Error reading file: %s\n", path);
            if (file_buffer != NULL)
                free (file_buffer);
            gzclose(fp);
            return;
        }
       
        document_t *doc = NULL; 
        doc = cu_build_cnlp_docs_memory_full (file_buffer, size);
        if (doc!=NULL) {

            cu_nc_prot_model_count(counts, doc);
            cu_document_free (&doc, TRUE);
        
        }
        free(file_buffer);
        gzseek (fp, pad, SEEK_CUR);

    }
    gzclose(fp);

}

void 
_chambers_nc_x_gzip_file_handler(
    gchar *path, 
    gpointer data)
{

 
    gzFile fp;
    fp = gzopen(path, "r");
    
    char buffer[512];
    int read = gzread(fp, buffer, 512);
    
    gboolean is_uncertain = FALSE; 
    char *content_type = 
        g_content_type_guess (NULL, buffer, read, &is_uncertain);

    if (content_type != NULL) {
        
        char *mime_type = g_content_type_get_mime_type (content_type);
        gzclose(fp);
        if (strcmp(mime_type, "application/xml")==0) {
        //    stats_xml_file_handler(path, data);
        } else if (strcmp(mime_type, "application/x-tar")==0) {
            _chambers_nc_tar_file_handler(path, data);
        }
        free (mime_type);
        free (content_type);
    } else {       
        gzclose(fp);  
    }

    free (path);

}


void chambers_nc_free(gpointer data) {
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    cu_chambers_nc_counts_free (&counts);

}

void chambers_nc_finish_processing (gpointer data) {
    cu_chambers_nc_counts_dump ((chambers_nc_counts_t *) data);
}

opt_s *cu_corenlp_counter_chambers_options_new()
{
    opt_s *options = NULL;
    options = (opt_s *) malloc (sizeof(opt_s));
    options->data = (void *) cu_chambers_nc_counts_new();    
    
    options->_xml_file_handler = _chambers_nc_xml_file_handler;
    options->_x_gzip_file_handler = _chambers_nc_x_gzip_file_handler;
    options->_finish_processing = chambers_nc_finish_processing;
    options->_free = chambers_nc_free;
    return options;
}

