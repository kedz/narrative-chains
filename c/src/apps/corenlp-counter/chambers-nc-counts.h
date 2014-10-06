#ifndef CC_CHAMBERS_COUNTER_H
#define CC_CHAMBERS_COUNTER_H
#include "corenlp/document.h"
#include "corenlp/xmlparser.h"
#include "cooc/nchains.h"
#include "apps/corenlp-counter/options.h"
#include <glib.h>
#include <gio/gio.h>
#include <zlib.h>

void _chambers_nc_xml_file_handler(gchar *, gpointer);        
opt_s *cu_corenlp_counter_chambers_options_new();

#endif
