#ifndef CORENLP_COUNTER_H
#define CORENLP_COUNTER_H
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include "util.h"
#include "apps/corenlp-counter/nc_counter.h"
#include "apps/corenlp-counter/chambers-nc-counts.h"
#include "apps/corenlp-counter/options.h"


typedef enum {
    CHAMBERS,
    CHAMBERS_DIR,
    EMBEDDING,
    EMBEDDING_DIR
} cc_mode_t;

#endif
