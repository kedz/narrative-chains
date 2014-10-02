#include "corenlp/document.h"
#include <math.h>
#include <glib.h>
#include <zlib.h>
#include "util.h"

typedef struct nchain_untyped_s {
    char *protag;
    char **events;
    int num_events;
} nchain_untyped_t;

typedef struct nevent_counts_s {
    GHashTable *joint;
    GHashTable *marginal;
    GHashTable *totals;
} nevent_counts_t;

void cu_untyped_nchain_free(nchain_untyped_t **);
void cu_nc_count_table_free (nevent_counts_t **);
GPtrArray *cu_extract_nar_chains_simple(document_t *);
gboolean is_verb(sentence_t *, unsigned int);
gboolean is_noun(sentence_t *, unsigned int);
nevent_counts_t *cu_nc_count_table_new();
void cu_count_untyped_nchains(nevent_counts_t *, nchain_untyped_t **, int);
void cu_count_untyped_nchain(nevent_counts_t *, nchain_untyped_t *);
double cu_ut_nchain_pmi(nevent_counts_t *, char *, char *, char *);
void cu_nc_count_table_dump (nevent_counts_t *ctables);

unsigned int cu_ut_nchain_marg_count(nevent_counts_t *, char *, char *);
unsigned int cu_ut_nchain_joint_count(
    nevent_counts_t *, char *, char *, char *);
