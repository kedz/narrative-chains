#include "apps/corenlp-counter/chambers-nc-counts.h"


void _chambers_nc_count_chains_directed (data, doc)
    const void *data;
    const document_t *doc;
{
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    chambers_nc_untyped_array_t *nchains = 
        cu_extract_chambers_nc_untyped_array (doc); 
    cu_chambers_nc_untyped_count_chains (counts, nchains, TRUE);
    cu_chambers_nc_untyped_array_free (&nchains);
}

void _chambers_nc_count_chains_undirected (data, doc)
    const void *data;
    const document_t *doc;
{
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    chambers_nc_untyped_array_t *nchains = 
        cu_extract_chambers_nc_untyped_array (doc); 
    cu_chambers_nc_untyped_count_chains (counts, nchains, FALSE);
    cu_chambers_nc_untyped_array_free (&nchains);
}

void 
_chambers_nc_free(data)
    const void *data; 
{
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    cu_chambers_nc_counts_free (&counts);
}

void _chambers_nc_finish_processing (data, ostream) 
    void *data;
    GOutputStream *ostream;
{
    cu_chambers_nc_counts_dump ((chambers_nc_counts_t *) data, ostream);
}

opt_s *
cu_corenlp_counter_chambers_options_new(directed)
    gboolean directed;
{
    opt_s *options = NULL;
    options = (opt_s *) malloc (sizeof(opt_s));
    options->data = (void *) cu_chambers_nc_counts_new ();    
    if (directed==TRUE) {
        options->_process_document = _chambers_nc_count_chains_directed;
    } else {
        options->_process_document = _chambers_nc_count_chains_undirected;
    }
    options->_finish_processing = _chambers_nc_finish_processing;
    options->_free = _chambers_nc_free;
    return options;
}

