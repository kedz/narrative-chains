#ifndef CC_NC_COUNTER_H
#define CC_NC_COUNTER_H
#include "corenlp/document.h"
#include "corenlp/xmlparser.h"
#include "cooc/nchains.h"
#include "apps/corenlp-counter/corenlp-counter.h"
#include "apps/corenlp-counter/options.h"
#include <glib.h>
#include <gio/gio.h>
#include <zlib.h>

opt_s *nc_options_new();
int octal_string_to_int(char *, unsigned int);
void _nc_tar_file_handler(gchar *path, gpointer data);

#endif
