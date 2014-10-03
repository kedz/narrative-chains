#include "apps/corenlp-counter/nc_counter.h"

typedef struct ncc_data_t {
    nevent_counts_t* ctables;

} ncc_data_t;

void _nc_xml_file_handler(
    gchar *path, 
    gpointer data
) {
    ncc_data_t *ncc_data = (ncc_data_t *) data;

    document_t *doc = NULL;
    doc = cu_build_cnlp_docs_file (path);

        
    if (doc!=NULL) {
        printf ("Extracting doc %s\n", path); 
        GPtrArray *nchains = cu_extract_nar_chains_simple (doc);
        cu_count_untyped_nchains (
            ncc_data->ctables, (nchain_untyped_t **) nchains->pdata, nchains->len);

        for (int c=0; c < nchains->len; c++) {
            nchain_untyped_t *nchain =
                (nchain_untyped_t *) g_ptr_array_index (nchains, c);
            cu_untyped_nchain_free (&nchain);
        }
        g_ptr_array_free (nchains, TRUE);

        cu_document_free (&doc, TRUE);
    }
    free (path);

} 

void 
_nc_x_gzip_file_handler(
    gchar *path, 
    gpointer data)
{

    //stats_data_t *s_dat = (stats_data_t *) data;
    //s_dat->num_gz++;
 
    gzFile fp;
    fp = gzopen(path, "r");
    
    char buffer[512];
    int read = gzread(fp, buffer, 512);
    
    gboolean is_uncertain = FALSE; 
    char *content_type = 
        g_content_type_guess (NULL, buffer, read, &is_uncertain);

    if (content_type != NULL) {
        
        char *mime_type = g_content_type_get_mime_type (content_type);
        g_print ("Content type for file '%s': %s (uncertain: %s)\n"
                 "MIME type for content type: %s\n",
                     path,
                     content_type,
                     is_uncertain ? "yes" : "no",
                     mime_type);

        gzclose(fp);
        if (strcmp(mime_type, "application/xml")==0) {
        //    stats_xml_file_handler(path, data);
        } else if (strcmp(mime_type, "application/x-tar")==0) {
            _nc_tar_file_handler(path, data);
        }
        free (mime_type);
        free (content_type);
    } else {       
        gzclose(fp);  
    }

    free (path);

}

void
_nc_tar_file_handler(
    gchar *path,
    gpointer data)
{

    ncc_data_t *ncc_data = (ncc_data_t *) data;

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
            GPtrArray *nchains = cu_extract_nar_chains_simple (doc);
            cu_count_untyped_nchains (
                ncc_data->ctables, (nchain_untyped_t **) nchains->pdata, 
                nchains->len);

            cu_document_free (&doc, TRUE);
        
            for (int c=0; c < nchains->len; c++) {
                nchain_untyped_t *nchain =
                    (nchain_untyped_t *) g_ptr_array_index (nchains, c);
                cu_untyped_nchain_free (&nchain);
            }
            g_ptr_array_free (nchains, TRUE);
        }
        free(file_buffer);
        gzseek (fp, pad, SEEK_CUR);

    }
    gzclose(fp);

}

void _nc_finish_processing (gpointer data) {
    ncc_data_t *ncc_data = (ncc_data_t *) data;


    cu_nc_count_table_dump (ncc_data->ctables);
}

void _nc_free(
    gpointer data
) {
    ncc_data_t *ncc_data = (ncc_data_t *) data;
    cu_nc_count_table_free (&(ncc_data->ctables));
}

opt_s *nc_options_new()
{
    opt_s *options = NULL;
    options = (opt_s *) malloc (sizeof(opt_s));
    ncc_data_t *nc_dat = (ncc_data_t *) malloc (sizeof(ncc_data_t));
    nc_dat->ctables = cu_nc_count_table_new();
    options->data = (void *) nc_dat;
    
    options->_xml_file_handler = _nc_xml_file_handler;
    options->_x_gzip_file_handler = _nc_x_gzip_file_handler;
    options->_finish_processing = _nc_finish_processing;
    options->_free = _nc_free;
    return options;
}

int octal_string_to_int(char *current_char, unsigned int size){
    unsigned int output = 0;
    while (size > 0) {
        output = output * 8 + *current_char - '0';
        current_char++;
        size--;
    }
    return output;
}
