#ifndef _GNU_SOURCE 
#define _GNU_SOURCE 
#endif 
#include <stdio.h>

#ifndef CC_PROTAGONIST_COUNTER_H
#define CC_PROTAGONIST_COUNTER_H
#include "corenlp/document.h"
#include "corenlp/xmlparser.h"
#include "cooc/nchains.h"
#include "apps/corenlp-counter/options.h"
#include <glib.h>
#include <gio/gio.h>
#include "util.h"

typedef struct protagonist_nc_counts_s {
    GHashTable *prot;
    GHashTable *event;
    GHashTable *joint;    
} prot_nc_counts_t;

opt_s *cu_corenlp_counter_protagonist_options_new(gboolean);

#endif
