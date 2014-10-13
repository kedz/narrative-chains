#include "corenlp/document.h"

#ifndef CORENLP_CNTR_OPTS_H
#define CORENLP_CNTR_OPTS_H

typedef struct opt_s {
    void *data;
    void (*_process_document) (const void *, const document_t *);
    void (* _finish_processing) (void *data, GOutputStream *ostream);
    void (* _free) (void *data);
} opt_s;

#endif

