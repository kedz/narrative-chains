#ifndef _GNU_SOURCE 
#define _GNU_SOURCE 
#endif 
#include <stdio.h>

#ifndef CC_CHAMBERS_COUNTER_H
#define CC_CHAMBERS_COUNTER_H
#include "corenlp/document.h"
#include "corenlp/xmlparser.h"
#include "cooc/nchains.h"
#include "apps/corenlp-counter/options.h"
#include <glib.h>
#include <gio/gio.h>
#include "util.h"

typedef struct chambers_nc_counts_s {
    GHashTable *event;
    GHashTable *joint;    
    gboolean is_directed;
    gboolean is_sequence;
} chambers_nc_counts_t;

opt_s *
cu_corenlp_counter_chambers_options_new(
    gboolean is_directed,
    gboolean is_sequence);

#endif
