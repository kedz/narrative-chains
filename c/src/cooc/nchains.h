#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef CU_COOC_NCHAINS_H 
#define CU_COOC_NCHAINS_H 

#include "corenlp/document.h"
#include <glib.h>
#include <gio/gio.h>
//#include <math.h>
//#include <glib.h>
//#include <zlib.h>
#include "util.h"

typedef struct chambers_nc_untyped_s {
    char *protag;
    char **events;
    int num_events;
} chambers_nc_untyped_t;

typedef struct chambers_nc_untyped_array_s {
    chambers_nc_untyped_t **chains;
    int num_chains;            
} chambers_nc_untyped_array_t;

typedef struct chambers_nc_counts_t {
    GHashTable *joint;
    GHashTable *marginal;
    unsigned int marginal_total;
    unsigned int joint_total;
} chambers_nc_counts_t;

chambers_nc_counts_t *cu_chambers_nc_counts_new();
void cu_chambers_nc_counts_free(chambers_nc_counts_t **);
//void cu_chambers_nc_count_chains(chambers_nc_counts_t *, GPtrArray *); 
//void cu_chambers_nc_counts_dump (chambers_nc_counts_t *);

chambers_nc_untyped_array_t *
cu_extract_chambers_nc_untyped_array(const document_t *);
void cu_chambers_nc_untyped_free(chambers_nc_untyped_t **nchain);
void cu_chambers_nc_untyped_array_free(chambers_nc_untyped_array_t **chains);
void cu_chambers_nc_untyped_count_chains(
    chambers_nc_counts_t *counts,
    const chambers_nc_untyped_array_t *chains,
    gboolean directed);
void cu_chambers_nc_counts_dump(
    chambers_nc_counts_t *counts,
    GOutputStream *ostream);

//typedef struct nevent_counts_s {
//    GHashTable *joint;
//    GHashTable *marginal;
//
//    unsigned int marginal_total;
//    unsigned int joint_total;
//    GHashTable *totals;
//} nevent_counts_t;
//
typedef struct count_data_s {
    unsigned int count;
} cd_t;

//typedef struct tot_data_s {
//    unsigned int marg_events;
//    unsigned int joint_events;
//} td_t;

//void cu_untyped_nchain_free(nchain_untyped_t **);
//void cu_nc_count_table_free (nevent_counts_t **);
//GPtrArray *cu_extract_nar_chains_simple(document_t *);
//gboolean is_verb(sentence_t *, unsigned int);
//gboolean is_noun(sentence_t *, unsigned int);
//nevent_counts_t *cu_nc_count_table_new();
//void cu_count_untyped_nchains(nevent_counts_t *, nchain_untyped_t **, int);
//void cu_count_untyped_nchain(nevent_counts_t *, nchain_untyped_t *);
//double cu_ut_nchain_pmi(nevent_counts_t *, char *, char *, char *);
//void cu_nc_count_table_dump (nevent_counts_t *ctables);
//
//unsigned int cu_ut_nchain_marg_count(nevent_counts_t *, char *, char *);
//unsigned int cu_ut_nchain_joint_count(
//    nevent_counts_t *, char *, char *, char *);
//
#endif
