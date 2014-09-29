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
    free (path);

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
    options->_x_gzip_file_handler = NULL;
    options->_finish_processing = _nc_finish_processing;
    options->_free = _nc_free;
    return options;
}
