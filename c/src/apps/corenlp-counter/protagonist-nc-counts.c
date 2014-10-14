#include "apps/corenlp-counter/protagonist-nc-counts.h"

char *_prot_joint_key(p, a, b)
    const char *p;
    const char *a;
    const char *b;
{

    char *key = NULL;
    int cmp = strcmp(a, b);
    if (cmp < 0) {
        if (asprintf (&key, "%s\t%s\t%s", p, a, b)==-1) {
            fprintf (stderr, "Could not allocate space for hash key!\n");
            exit (EXIT_FAILURE);
        }
    } else {
        if(asprintf (&key, "%s\t%s\t%s", p, b, a)==-1) {
            fprintf (stderr, "Could not allocate space for hash key!\n");
            exit (EXIT_FAILURE);
        }
    }
    return key;
}

prot_nc_counts_t *
__protagonist_nc_counts_new()
{
    prot_nc_counts_t *counts = malloc (sizeof(prot_nc_counts_t));
    counts->prot = g_hash_table_new_full (
        g_str_hash, g_str_equal, free, free);
    counts->event = g_hash_table_new_full (
        g_str_hash, g_str_equal, free, free);
    counts->joint = g_hash_table_new_full (
        g_str_hash, g_str_equal, free, free);
    return counts;
}

void
_protagonist_nc_count_chains_directed(data, doc)
    const void *data;
    const document_t *doc;
{
    prot_nc_counts_t *counts = (prot_nc_counts_t *) data;
    chambers_nc_untyped_array_t *nchains =
        cu_extract_chambers_nc_untyped_array (doc);
    if (nchains->num_chains > 0) {
        chambers_nc_untyped_t *chain = nchains->chains[0];

        unsigned int *pcount = NULL;
        pcount = g_hash_table_lookup (counts->prot, chain->protag); 
        if (pcount==NULL) {
            pcount = malloc (sizeof(unsigned int));
            *pcount = chain->num_events;
            CU_NEWSTRCPY(prot_cpy, chain->protag);
            g_hash_table_insert (counts->prot, prot_cpy, pcount);
        } else {
            *pcount = *pcount + chain->num_events;
        }

        char *p = chain->protag;
        for (int i=0; i < chain->num_events; i++) {
            unsigned int *ecount = NULL;
            ecount = g_hash_table_lookup (counts->event, chain->events[i]); 
            if (ecount==NULL) {
                ecount = malloc (sizeof(unsigned int));
                *ecount = 1;
                CU_NEWSTRCPY(event_cpy, chain->events[i]);
                g_hash_table_insert (counts->event, event_cpy, ecount);
            } else {
                *ecount = *ecount + 1;
            }           
            
            char *e1 = chain->events[i];
            for (int j=i+1; j < chain->num_events; j++) {
                char *e2 = chain->events[j];
                char *key = NULL;
                if (asprintf (&key, "%s\t%s\t%s", p, e1, e2)==-1) {
                    fprintf (stderr, "Could not allocate space for key\n");
                    exit(EXIT_FAILURE);
                }
           
                unsigned int *jcount = NULL; 
                jcount = g_hash_table_lookup (counts->joint, key); 
                if (jcount==NULL) {
                    jcount = malloc (sizeof(unsigned int));
                    *jcount = 1;
                    //CU_NEWSTRCPY(event_cpy, chain->events[i]);
                    g_hash_table_insert (counts->joint, key, jcount);
                } else {
                    *jcount = *jcount + 1;
                    free (key);
                }
            }
        }
    }
    cu_chambers_nc_untyped_array_free (&nchains);

}

void
_protagonist_nc_count_chains_undirected(data, doc)
    const void *data;
    const document_t *doc;
{
    prot_nc_counts_t *counts = (prot_nc_counts_t *) data;
    chambers_nc_untyped_array_t *nchains =
        cu_extract_chambers_nc_untyped_array (doc);
    if (nchains->num_chains > 0) {
        chambers_nc_untyped_t *chain = nchains->chains[0];

        unsigned int *pcount = NULL;
        pcount = g_hash_table_lookup (counts->prot, chain->protag); 
        if (pcount==NULL) {
            pcount = malloc (sizeof(unsigned int));
            *pcount = chain->num_events;
            CU_NEWSTRCPY(prot_cpy, chain->protag);
            g_hash_table_insert (counts->prot, prot_cpy, pcount);
        } else {
            *pcount = *pcount + chain->num_events;
        }

        char *p = chain->protag;
        for (int i=0; i < chain->num_events; i++) {
            unsigned int *ecount = NULL;
            ecount = g_hash_table_lookup (counts->event, chain->events[i]); 
            if (ecount==NULL) {
                ecount = malloc (sizeof(unsigned int));
                *ecount = 1;
                CU_NEWSTRCPY(event_cpy, chain->events[i]);
                g_hash_table_insert (counts->event, event_cpy, ecount);
            } else {
                *ecount = *ecount + 1;
            }           
            
            char *e1 = chain->events[i];
            for (int j=i+1; j < chain->num_events; j++) {
                char *e2 = chain->events[j];
                char *key = _prot_joint_key (p, e1, e2);
                unsigned int *jcount = NULL; 
                jcount = g_hash_table_lookup (counts->joint, key); 
                if (jcount==NULL) {
                    jcount = malloc (sizeof(unsigned int));
                    *jcount = 1;
                    //CU_NEWSTRCPY(event_cpy, chain->events[i]);
                    g_hash_table_insert (counts->joint, key, jcount);
                } else {
                    *jcount = *jcount + 1;
                    free (key);
                }
            }
        }
    }
    cu_chambers_nc_untyped_array_free (&nchains);

}


void _protagonist_nc_finish_processing (data, ostream) 
    void *data;
    GOutputStream *ostream;
{
    
    prot_nc_counts_t *counts = (prot_nc_counts_t *) data;
    
    GHashTableIter iter;
    gpointer prot, prot_count;
    g_hash_table_iter_init (&iter, counts->prot);
    while (g_hash_table_iter_next (&iter, &prot, &prot_count)) {
        char *line = NULL;
        gsize line_size = asprintf (&line, "P_%s\t%d\n",
            (char *) prot, *((unsigned int *)prot_count));
        g_output_stream_write (
            ostream, line, line_size, NULL, NULL); 
        free (line);

    }

    gpointer event, event_count;
    g_hash_table_iter_init (&iter, counts->event);
    while (g_hash_table_iter_next (&iter, &event, &event_count)) {
        char *line = NULL;
        gsize line_size = asprintf (&line, "E_%s\t%d\n",
            (char *) event, *((unsigned int *)event_count));
        g_output_stream_write (
            ostream, line, line_size, NULL, NULL); 
        free (line);

    }

    gpointer joint, joint_count;
    g_hash_table_iter_init (&iter, counts->joint);
    while (g_hash_table_iter_next (&iter, &joint, &joint_count)) {
        char *line = NULL;
        gsize line_size = asprintf (&line, "%s\t%d\n",
            (char *) joint, *((unsigned int *)joint_count));
        g_output_stream_write (
            ostream, line, line_size, NULL, NULL); 
        free (line);

    }


//    cu_chambers_nc_counts_dump ((chambers_nc_counts_t *) data, ostream);
}

void
_protagonist_nc_free(data)
    const void *data;
{
    prot_nc_counts_t *counts = (prot_nc_counts_t *) data;
    g_hash_table_destroy (counts->prot);
    g_hash_table_destroy (counts->event);
    g_hash_table_destroy (counts->joint);
    free (counts);

}

opt_s *
cu_corenlp_counter_protagonist_options_new(directed)
    gboolean directed;
{
    opt_s *options = NULL;
    options = (opt_s *) malloc (sizeof(opt_s));
    options->data = __protagonist_nc_counts_new ();
    if (directed==TRUE) {
        options->_process_document = _protagonist_nc_count_chains_directed;
    } else {
        options->_process_document = _protagonist_nc_count_chains_undirected;
    }

    options->_finish_processing = _protagonist_nc_finish_processing;
    options->_free = _protagonist_nc_free;
    return options;
}
