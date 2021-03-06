#include "cooc/nchains.h"

//Forward declaration of private functions

//char *
//_joint_key(
//    const char *a,
//    const char *b);

char *
_recover_verb_lemma_prt(
    const sentence_t *s,
    const int verb_idx);

GPtrArray *
_recover_nn(
    const sentence_t *s,
    const int noun_idx);

gboolean _is_noun(
    const sentence_t *s,
    const unsigned int idx);

gboolean _is_verb(
    const sentence_t *s,
    const unsigned int idx);

int _chambers_untyped_len_cmp(
    const gpointer a,
    const gpointer b);

chambers_nc_untyped_array_t *
cu_extract_chambers_nc_untyped_array(doc)
    const document_t *doc;    
{

    GHashTable *nchains = NULL;
    nchains = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, NULL);
     
    sentence_t *sent = NULL;
    for (int s=0; s < doc->s_len; s++) {
        sent = doc->sentences[s];
        for (int t=0; t < sent->t_len; t++) {
            char *verb = NULL;
            if ((verb=_recover_verb_lemma_prt (sent, t))!=NULL) {
                for (int n=0; n < sent->t_len; n++) {
                    if (sent->deps[t+1][n+1]>0 && _is_noun (sent, n)) {

                        GPtrArray *nouns = _recover_nn (sent, n);
                        CU_NEWSTRCPY(noun, sent->lemmas[n]);
                        g_ptr_array_add (nouns, noun);
                        for (int nn=0; nn < nouns->len; nn++) {
                            char *prot = (char *) nouns->pdata[nn];
                            GPtrArray *verbs = 
                                (GPtrArray * ) g_hash_table_lookup (
                                    nchains, prot);
                            if (verbs==NULL) {
                                verbs = g_ptr_array_new();    
                                g_hash_table_insert (nchains, prot, verbs); 
                                CU_NEWSTRCPY(verb_cpy, verb) 
                                g_ptr_array_add (verbs, verb_cpy);    
                            } else {
                                CU_NEWSTRCPY(verb_cpy, verb) 
                                g_ptr_array_add (verbs, verb_cpy);    
                                free (prot);
                            }
                        
                        }
                        g_ptr_array_free (nouns, TRUE);
                    }
                }
                free (verb);
                verb = NULL;
            }
        }
    }
    GPtrArray *nchains_arr = g_ptr_array_sized_new (
        g_hash_table_size (nchains));

    GHashTableIter iter;
    gpointer prot;
    gpointer events;

    g_hash_table_iter_init (&iter, nchains);
    while (g_hash_table_iter_next (&iter, &prot, &events))
    {

        g_ptr_array_set_size (
            (GPtrArray *) events, ((GPtrArray *) events)->len);

        chambers_nc_untyped_t *nchain = NULL;
        nchain = malloc (sizeof(chambers_nc_untyped_t));
        nchain->protag = (char *) prot;
        nchain->events = (char **) ((GPtrArray *) events)->pdata;
        nchain->num_events = ((GPtrArray *) events)->len;
        g_ptr_array_free ((GPtrArray *) events, FALSE);
        g_ptr_array_add (nchains_arr, (gpointer) nchain);
    }

    g_ptr_array_sort (nchains_arr, (GCompareFunc) _chambers_untyped_len_cmp);
    g_hash_table_destroy (nchains);
    g_ptr_array_set_size (nchains_arr, nchains_arr->len);
    chambers_nc_untyped_array_t *arr = malloc (
        sizeof(chambers_nc_untyped_array_t));
    arr->chains = (chambers_nc_untyped_t **) nchains_arr->pdata;
    arr->num_chains = nchains_arr->len;
    g_ptr_array_free (nchains_arr, FALSE);
    return arr; 
}

void
cu_chambers_nc_untyped_free(chain)
    chambers_nc_untyped_t **chain;
{

    for (int e=0; e < (*chain)->num_events; e++) {
        free ((*chain)->events[e]);
    }
    free ((*chain)->events);
    free ((*chain)->protag);
    free ((*chain));
    *chain = NULL;
}

void 
cu_chambers_nc_untyped_array_free(chains)
    chambers_nc_untyped_array_t **chains;
{
    for (int c=0; c < (*chains)->num_chains; c++) {
        cu_chambers_nc_untyped_free (&((*chains)->chains[c]));
    }
    free ((*chains)->chains);
    free (*chains);
    *chains = NULL;
}

//chambers_nc_counts_t *
//cu_chambers_nc_counts_new()
//{
//    chambers_nc_counts_t *counts = NULL;
//    counts = (chambers_nc_counts_t *) malloc (sizeof(chambers_nc_counts_t));
//    counts->joint = g_hash_table_new_full (
//        g_str_hash, g_str_equal, free, free);
//    counts->marginal = g_hash_table_new_full (
//        g_str_hash, g_str_equal, free, free);
//    counts->joint_total = 0;
//    counts->marginal_total = 0;
//
//    return counts;
//}
//
//void cu_chambers_nc_counts_free(counts)
//    chambers_nc_counts_t **counts;
//{
//    g_hash_table_destroy ((*counts)->joint);
//    g_hash_table_destroy ((*counts)->marginal);
//    free ((*counts));
//    *counts = NULL;
//}
//
//
//void cu_chambers_nc_untyped_count_chains(counts, chains, directed)
//    chambers_nc_counts_t *counts;
//    const chambers_nc_untyped_array_t *chains;
//    gboolean directed;
//{
//
//    if (chains->num_chains > 0) {
//        chambers_nc_untyped_t *chain = chains->chains[0]; 
//        for (int e1=0; e1 < chain->num_events; e1++) {
//            counts->marginal_total++;
//            char *event1 = chain->events[e1];
//            cd_t *cdata = g_hash_table_lookup (counts->marginal, event1);
//            if (cdata==NULL) {
//                cdata = (cd_t *) malloc (sizeof(cd_t));
//                cdata->count = 1;
//                CU_NEWSTRCPY(event, event1)
//                g_hash_table_insert (counts->marginal, event, cdata);
//            } else {
//                cdata->count++;
//            }
//            for (int e2=e1+1; e2 < chain->num_events; e2++) {
//                counts->joint_total++;
//                char *event2 = chain->events[e2];
//                
//                char *key = NULL;
//                if (directed) {
//                    if (asprintf (&key, "%s\t%s", event1, event2)==-1) {
//                        fprintf (stderr, 
//                            "Could not allocate space for key.\n");
//                        exit (EXIT_FAILURE);
//                    }
//                } else {
//                    key = _joint_key(event1, event2);       
//                }
//
//                cd_t *jdata = g_hash_table_lookup (counts->joint, key);
//                if (jdata==NULL) {
//                    jdata = (cd_t *) malloc (sizeof(cd_t));
//                    jdata->count = 1;
//                    g_hash_table_insert (counts->joint, key, jdata);
//                } else {
//                    jdata->count++;
//                    free (key);
//                }
//                         
//            }
//
//        }
//
//    }
//
////    for (int c=0; c < nchains->len; c++) {
////        cu_untyped_nchain_free (&nchains->pdata[c]);
////    }
////    g_ptr_array_free (nchains, TRUE);
//}


//char *_joint_key(a, b)
//    const char *a;
//    const char *b;
//{
//
//    char *key = NULL;
//    int cmp = strcmp(a, b);
//    if (cmp < 0) {
//        if (asprintf (&key, "%s\t%s", a, b)==-1) {
//            fprintf (stderr, "Could not allocate space for hash key!\n");
//            exit (EXIT_FAILURE);
//        }
//    } else {
//        if(asprintf (&key, "%s\t%s", b, a)==-1) {
//            fprintf (stderr, "Could not allocate space for hash key!\n");
//            exit (EXIT_FAILURE);
//        }
//    }
//    return key;
//}

int _chambers_untyped_len_cmp(a, b)
    const gpointer a;
    const gpointer b;
{
    chambers_nc_untyped_t *chain_a = *(chambers_nc_untyped_t **) a;
    chambers_nc_untyped_t *chain_b = *(chambers_nc_untyped_t **) b;
    if (chain_a->num_events < chain_b->num_events) 
        return 1;
    else if (chain_a->num_events == chain_b->num_events)
        return 0;
    else
        return -1;
}

char *
_recover_verb_lemma_prt(s, verb_idx)
    const sentence_t *s;
    const int verb_idx;
{

    if (_is_verb (s, verb_idx)) {
        
        if (strcmp(s->lemmas[verb_idx], "be")==0)
            return NULL; 
        if (strcmp(s->lemmas[verb_idx], "have")==0)
            return NULL; 
        GString *buffer = g_string_new (s->lemmas[verb_idx]);
        for (int p=0; p < s->t_len; p++) {
            if (s->deps[verb_idx+1][p+1]==TD_PRT) {
                if (p < verb_idx) {
                    g_string_prepend_c (buffer, ' ');
                    g_string_prepend (buffer, s->lemmas[p]);
                } else {
                    g_string_append_c (buffer, ' ');
                    g_string_append (buffer, s->lemmas[p]);
                }

            }
        }
        return g_string_free (buffer, FALSE);
    }

    return NULL;
}

GPtrArray *
_recover_nn(s, noun_idx)
    const sentence_t *s;
    const int noun_idx;
{
    GPtrArray *nouns = g_ptr_array_new ();
    for (int n=0; n < s->t_len; n++) {
        if (s->deps[noun_idx+1][n+1]>0 && _is_noun (s, n)) {
            CU_NEWSTRCPY(nn, s->lemmas[n])
            g_ptr_array_add (nouns, nn);    
        }
    }
    return nouns;
}

//
//void cu_nc_prot_model_count(
//    nevent_counts_t* counts,
//    document_t *doc
//) {
//
//    GPtrArray *nchains = cu_extract_nar_chains_simple (doc);
//    if (nchains->len > 0) {
//        nchain_untyped_t *chain = (nchain_untyped_t *) nchains->pdata[0]; 
//        for (int e1=0; e1 < chain->num_events; e1++) {
//            counts->marginal_total++;
//            char *event1 = chain->events[e1];
//            cd_t *cdata = g_hash_table_lookup (counts->marginal, event1);
//            if (cdata==NULL) {
//                cdata = (cd_t *) malloc (sizeof(cd_t));
//                cdata->count = 1;
//                CU_NEWSTRCPY(event, event1)
//                g_hash_table_insert (counts->marginal, event, cdata);
//            } else {
//                cdata->count++;
//            }
//            for (int e2=e1+1; e2 < chain->num_events; e2++) {
//                counts->joint_total++;
//                char *event2 = chain->events[e2];
//                char *key = _joint_key(event1, event2);       
//                cd_t *jdata = g_hash_table_lookup (counts->joint, key);
//                if (jdata==NULL) {
//                    jdata = (cd_t *) malloc (sizeof(cd_t));
//                    jdata->count = 1;
//                    g_hash_table_insert (counts->joint, key, jdata);
//                } else {
//                    jdata->count++;
//                    free (key);
//                }
//                         
//            }
//
//        }
//
//    }
//
//    for (int c=0; c < nchains->len; c++) {
//        cu_untyped_nchain_free (&nchains->pdata[c]);
//    }
//    g_ptr_array_free (nchains, TRUE);
//}
//

//
//nevent_counts_t *cu_nc_count_table_new() 
//{
//
//    nevent_counts_t *nc_table = NULL;
//    nc_table = (nevent_counts_t *) malloc (sizeof(nevent_counts_t));
//    nc_table->joint = g_hash_table_new_full (
//        g_str_hash, g_str_equal, free, (GDestroyNotify) g_hash_table_destroy);
//    nc_table->marginal = g_hash_table_new_full (
//        g_str_hash, g_str_equal, free, (GDestroyNotify) g_hash_table_destroy);
//    nc_table->totals = g_hash_table_new_full (
//        g_str_hash, g_str_equal, free, free);
//    nc_table->marginal_total = 0;
//    nc_table->joint_total = 0;
//
//    return nc_table;
//}
//
//
//
//void cu_count_untyped_nchains(
//        nevent_counts_t *ctable,
//        nchain_untyped_t **nchains,
//        int num_chains
//) {
//    for (int c=0; c < num_chains; c++) 
//        cu_count_untyped_nchain (ctable, nchains[c]);
//}
//
//
//
//
//
//
//void cu_count_untyped_nchain(
//    nevent_counts_t *ctable,
//    nchain_untyped_t *nchain
//) {
//
//    td_t *totals = (td_t *) g_hash_table_lookup (
//        ctable->totals, nchain->protag);
//    if (totals==NULL) {
//        totals = (td_t *) malloc (sizeof(td_t)); 
//        totals->marg_events = 0;
//        totals->joint_events = 0;
//        CU_NEWSTRCPY(prot, nchain->protag)
//        g_hash_table_insert (ctable->totals, prot, totals);
//    }
//
//
//
//    GHashTable *pr_joint = (GHashTable *) g_hash_table_lookup (
//        ctable->joint, nchain->protag);
//    if (pr_joint==NULL) {
//        pr_joint = g_hash_table_new_full (g_str_hash, g_str_equal, free, free);
//        CU_NEWSTRCPY(prot, nchain->protag);
//        g_hash_table_insert (ctable->joint, prot, pr_joint);
//    }
//
//    GHashTable *pr_marg = (GHashTable *) g_hash_table_lookup (
//        ctable->marginal, nchain->protag);
//    if (pr_marg==NULL) {
//        pr_marg = g_hash_table_new_full (g_str_hash, g_str_equal, free, free);
//        CU_NEWSTRCPY(prot, nchain->protag);
//        g_hash_table_insert (ctable->marginal, prot, pr_marg);
//    }
//
//    for (int e=0; e < nchain->num_events; e++) {
//        totals->marg_events++;
//        cd_t *cdata = g_hash_table_lookup (pr_marg, nchain->events[e]);
//        if (cdata==NULL) {
//            cdata = (cd_t *) malloc (sizeof(cd_t));
//            cdata->count = 1;
//            CU_NEWSTRCPY(event, nchain->events[e])
//            g_hash_table_insert (pr_marg, event, cdata);
//        } else {
//            cdata->count++;
//        }
//
//        for (int e2=e+1; e2 < nchain->num_events; e2++) {
//            totals->joint_events++;
//            char *key = _joint_key (nchain->events[e], nchain->events[e2]);
//            cd_t *jcdata = g_hash_table_lookup (pr_joint, key);
//            if (jcdata==NULL) {
//                jcdata = (cd_t *) malloc (sizeof(cd_t));
//                jcdata->count = 1;
//                g_hash_table_insert (pr_joint, key, jcdata);
//            } else {
//                jcdata->count++;
//                free (key);
//            }
//
//        }
//
//    }
//}
//
//unsigned int cu_ut_nchain_marg_count(
//    nevent_counts_t *ctable,
//    char *prot,
//    char *event
//) {
//    GHashTable *pr_table = (GHashTable *) g_hash_table_lookup (
//        ctable->marginal, prot);
//    if (pr_table==NULL)
//        return 0;
//    cd_t *cdat = (cd_t *) g_hash_table_lookup (pr_table, event);
//    if (cdat==NULL)
//        return 0;
//    else
//        return cdat->count;
//}
//
//unsigned int cu_ut_nchain_joint_count(
//    nevent_counts_t *ctable,
//    char *prot,
//    char *event1,
//    char *event2
//) {
//    GHashTable *pr_table = (GHashTable *) g_hash_table_lookup (
//        ctable->joint, prot);
//    if (pr_table==NULL) 
//        return 0;
//    char *key = _joint_key (event1, event2);
//    cd_t *cdat = (cd_t *) g_hash_table_lookup (pr_table, key);
//    free (key);
//    if (cdat==NULL) {
//        return 0;
//    } else {
//        return cdat->count;
//    }
//}
//
//


//
//double cu_ut_nchain_pmi(
//    nevent_counts_t *ctables,
//    char *prot,
//    char *event1,
//    char *event2
//) {
//
//    td_t *totals = (td_t *) g_hash_table_lookup (ctables->totals, prot);
//    if (totals==NULL)
//        return -INFINITY;
//
//    unsigned int joint_cnt = 
//        cu_ut_nchain_joint_count (ctables, prot, event1, event2);
//    if (joint_cnt == 0)
//        return -INFINITY;
// 
//    unsigned int marg_cnt1 = 
//        cu_ut_nchain_marg_count (ctables, prot, event1);
//
//    unsigned int marg_cnt2 = 
//        cu_ut_nchain_marg_count (ctables, prot, event2);
//
//    double pmi = log ((joint_cnt * totals->marg_events * totals->marg_events) 
//         / ((double) marg_cnt1 * marg_cnt2 * totals->joint_events)); 
//    return pmi;
//}
//


//void cu_nc_count_table_free(
//    nevent_counts_t ** ctables
//) {
//
//    g_hash_table_destroy ((*ctables)->joint);
//    g_hash_table_destroy ((*ctables)->marginal);
//    g_hash_table_destroy ((*ctables)->totals);
//
//    free ((*ctables));
//    *ctables = NULL;
//
//}
//
//
//void cu_chambers_nc_counts_dump(counts, ostream)
//    chambers_nc_counts_t *counts;
//    GOutputStream *ostream;
//{
//    GHashTableIter iter;
//    gpointer event, event_count;  
//
//    g_hash_table_iter_init (&iter, counts->marginal);
//    while (g_hash_table_iter_next (&iter, &event, &event_count)) {
//
//        char *line = NULL;
//        gsize line_size = asprintf (&line, "%s\t%d\n",
//            (char *) event, ((cd_t *)event_count)->count);
//        g_output_stream_write (
//            ostream, line, line_size, NULL, NULL); 
//        free (line);
//    }
//
//    g_hash_table_iter_init (&iter, counts->joint);
//    while (g_hash_table_iter_next (&iter, &event, &event_count)) {
//
//        char *line = NULL;
//        gsize line_size = asprintf (&line, "%s\t%d\n",
//            (char *) event, ((cd_t *)event_count)->count);
//        g_output_stream_write (
//            ostream, line, line_size, NULL, NULL); 
//        free (line);
//    } 
//}


//void cu_nc_count_table_dump(
//    nevent_counts_t *ctables
//) {
//
//
//    gzFile counts_file;
//    counts_file = gzopen ("test_counts.gz", "w");
//    GHashTableIter pr_iter;
//    gpointer prot, totals;  
//    //char *prot;
//    //GPtrArray *events;
//
//    g_hash_table_iter_init (&pr_iter, ctables->totals);
//    while (g_hash_table_iter_next (&pr_iter, &prot, &totals))
//    {
//       
//      
//         
//        GHashTableIter event_iter;
//        gpointer event, counts;  
//        gzprintf (counts_file, "%s\t%d\t%d\n", 
//            (char *) prot , ((td_t *)totals)->marg_events, 
//            ((td_t *)totals)->joint_events); 
//
//        GHashTable *marg_counts = g_hash_table_lookup (
//            ctables->marginal, (char *) prot);
//        
//        if (marg_counts==NULL) {
//            fprintf (stderr, 
//                "Error finding marginal counts for %s -- skipping.\n", 
//                (char *) prot);
//            continue;
//        }
//
//        GHashTable *joint_counts = g_hash_table_lookup (
//                    ctables->joint, (char *) prot);
//
//        if (joint_counts==NULL) {
//            fprintf (stderr, 
//                "Error finding joint counts for %s -- skipping.\n", 
//                (char *) prot);
//            continue;
//        }
//
//
//        g_hash_table_iter_init (&event_iter, marg_counts);
//        while (g_hash_table_iter_next (&event_iter, &event, &counts)) {
//        
//            gzprintf (counts_file, "%s\t%s\t%d\n", 
//                (char *) prot, (char *) event, 
//                ((cd_t *)counts)->count);
//
//           
//        }
//
//        g_hash_table_iter_init (&event_iter, joint_counts);
//        while (g_hash_table_iter_next (&event_iter, &event, &counts)) {
//        
//            gzprintf (counts_file, "%s\t%s\t%d\n", 
//            (char *) prot, (char *) event, 
//            ((cd_t *)counts)->count);
//
//           
//        }
//        gzprintf (counts_file, "\n");
//
//
//    }
//
//    gzclose (counts_file);
//
//}

gboolean _is_noun(s, idx)
    const sentence_t *s;
    const unsigned int idx;
{
    if (cu_pos_string (s->pos[idx])[0] == 'N')
        return TRUE;
    else
        return FALSE;
}

gboolean _is_verb(s, idx)
    const sentence_t *s;
    const unsigned int idx;
{
    if (cu_pos_string (s->pos[idx])[0] == 'V')
        return TRUE;
    else
        return FALSE;
}

