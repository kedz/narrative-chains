#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef CORENLP_COUNTER_H
#define CORENLP_COUNTER_H
#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <libxml/parser.h>
#include <string.h>
#include "util.h"
#include "corenlp/xmlparser.h"
#include "corenlp/document.h"
//#include "apps/corenlp-counter/nc_counter.h"
#include "apps/corenlp-counter/chambers-nc-counts.h"
#include "apps/corenlp-counter/protagonist-nc-counts.h"
#include "apps/corenlp-counter/options.h"


typedef enum {
    CHAMBERS,
    CHAMBERS_DIR,
    PROTAGONIST,
    PROTAGONIST_DIR
} cc_mode_t;

#endif
