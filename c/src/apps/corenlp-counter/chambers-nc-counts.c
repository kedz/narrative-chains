#include "apps/corenlp-counter/chambers-nc-counts.h"

// Forward Declarations of private functions
chambers_nc_counts_t *
__cu_cnc_counts_new(
    gboolean is_directed,
    gboolean is_sequence);

void
__cu_cnc_free(
    void *data);

void
__cu_cnc_count_chains(
    const void *data,
    const document_t *doc);

void
__cu_cnc_finish_processing(
    void *data,
    GOutputStream *ostream);

void
__cnc_count_singleton(
    GHashTable *table,
    char *token);

void
__cnc_count_joint(
    GHashTable *table,
    char *event1,
    char *event2, 
    gboolean is_directed);        

char *
__cnc_joint_key(
    const char *a,
    const char *b,
    const gboolean is_directed);


/**
 * Create Chambers model implementation of options interface.
 * That is, this struct tells corenlp-counter how to make Chambers model
 * counts from a corenlp document.
 **/
opt_s *
cu_corenlp_counter_chambers_options_new(is_directed, is_sequence)
    gboolean is_directed;
    gboolean is_sequence;
{
    opt_s *options = NULL;
    options = (opt_s *) malloc (sizeof(opt_s));
    options->data = (void *) __cu_cnc_counts_new (is_directed, is_sequence);
    options->_process_document = __cu_cnc_count_chains;
    options->_finish_processing = __cu_cnc_finish_processing;
    options->_free = __cu_cnc_free;
    return options;
}

/**
 *  Create a new counts struct to keep narrative chain counts.
 * 
 **/
chambers_nc_counts_t *
__cu_cnc_counts_new(is_directed, is_sequence)
    gboolean is_directed;
    gboolean is_sequence;
{
    chambers_nc_counts_t *counts = malloc (sizeof(chambers_nc_counts_t));
    counts->event = g_hash_table_new_full (
        g_str_hash, g_str_equal, free, free);
    counts->joint = g_hash_table_new_full (
        g_str_hash, g_str_equal, free, free);
    counts->is_directed = is_directed;
    counts->is_sequence = is_sequence;
    return counts;
}

void
__cu_cnc_free(data)
    void *data;
{
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    g_hash_table_destroy (counts->event);
    g_hash_table_destroy (counts->joint);
    free (counts);
}

/**
 * Count the events in the longest narrative chain in doc.
 **/
void
__cu_cnc_count_chains(data, doc)
    const void *data;
    const document_t *doc;
{

    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    chambers_nc_untyped_array_t *nchains =
        cu_extract_chambers_nc_untyped_array (doc);

    if (nchains->num_chains > 0) {
        chambers_nc_untyped_t *chain = nchains->chains[0];

        for (int i=0; i < chain->num_events; i++) {
            __cnc_count_singleton (counts->event, chain->events[i]); 
                      
            if (counts->is_sequence==TRUE) {
                if (i + 1 < chain->num_events) {
                    __cnc_count_joint (
                        counts->joint, 
                        chain->events[i], chain->events[i+1], 
                        counts->is_directed);        
                }
            } else {
                for (int j=i+1; j < chain->num_events; j++) {
                    __cnc_count_joint (
                        counts->joint,
                        chain->events[i], chain->events[j], 
                        counts->is_directed);        
                }
            }
        }
    }
    cu_chambers_nc_untyped_array_free (&nchains);
}

/**
 * Write out counts to output stream.
 **/
void
__cu_cnc_finish_processing (data, ostream) 
    void *data;
    GOutputStream *ostream;
{
    
    chambers_nc_counts_t *counts = (chambers_nc_counts_t *) data;
    
    GHashTableIter iter;
    gpointer event, event_count;
    g_hash_table_iter_init (&iter, counts->event);
    while (g_hash_table_iter_next (&iter, &event, &event_count)) {
        char *line = NULL;
        gsize line_size = asprintf (&line, "%s\t%d\n",
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
}

/**
 * Add 'count' to the count of 'token' in 'table'. Helper func for counting
 * events. 
 **/
void __cnc_count_singleton(table, token)
    GHashTable *table;
    char *token;
{
    unsigned int *count_ptr = NULL;
    count_ptr = g_hash_table_lookup (table, token); 
    if (count_ptr==NULL) {
        count_ptr = malloc (sizeof(unsigned int));
        *count_ptr = 1;
        CU_NEWSTRCPY(token_cpy, token);
        g_hash_table_insert (table, token_cpy, count_ptr);
    } else {
        *count_ptr = *count_ptr + 1;
    }
}

/**
 * Increment count of (event1,event2) in 'table'. 
 * Helper func for counting narrative chain segments.
 * If is_directed is FALSE, events are put in lexical order. 
 **/
void
__cnc_count_joint(table, event1, event2, is_directed)
    GHashTable *table;
    char *event1;
    char *event2; 
    gboolean is_directed;        
{
    char *key = __cnc_joint_key (event1, event2, is_directed);
    unsigned int *count_ptr = NULL; 
    count_ptr = g_hash_table_lookup (table, key); 
    if (count_ptr==NULL) {
        count_ptr = malloc (sizeof(unsigned int));
        *count_ptr = 1;
        g_hash_table_insert (table, key, count_ptr);
    } else {
        *count_ptr = *count_ptr + 1;
        free (key);
    }
}

/**
 * Create hash key for event pair 'a' and 'b'.
 * If is_directed is False, 'a' and 'b' are put into lexical order.
 **/
char *
__cnc_joint_key(a, b, is_directed)
    const char *a;
    const char *b;
    const gboolean is_directed;
{
    char *key = NULL;
    if (is_directed==TRUE || strcmp(a, b) < 0) {
        if (asprintf (&key, "%s\t%s", a, b)==-1) {
            fprintf (stderr, "Could not allocate space for hash key!\n");
            exit (EXIT_FAILURE);
        }
    } else {
        if(asprintf (&key, "%s\t%s", b, a)==-1) {
            fprintf (stderr, "Could not allocate space for hash key!\n");
            exit (EXIT_FAILURE);
        }
    }
    return key;
}
