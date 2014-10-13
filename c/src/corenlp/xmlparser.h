#ifndef CORENLP_XMLPARSER_H
#define CORENLP_XMLPARSER_H

#include <libxml/parser.h>
#include "corenlp/document.h"
#include <glib.h>

typedef struct cnlp_xmlp_state_s {

    void *data;

    gboolean _in_sentences;
    gboolean _in_sentence;
    gboolean _in_dependencies;
    gboolean _in_dep;
    gboolean _in_governor;
    gboolean _in_dependent;
    gboolean _in_tokens;
    gboolean _in_token;
    gboolean _in_word;
    gboolean _in_lemma;
    gboolean _in_pos;

} cnlp_xmlp_state_s;

typedef struct cnlp_xml_dbuilder_t {
    DBuilder_t *db;
    document_t *doc;
    GString *token_buffer;
    GString *lemma_buffer;
    int gov_idx, dep_idx;
    dep_t dep_type;

} cnlp_xml_dbuilder_t;


xmlSAXHandler *get_corenlp_xml_sax_handler_ptr ();
cnlp_xmlp_state_s *cu_cnlp_xmlp_state_new ();

document_t *cu_build_cnlp_docs_memory_full(char *, int);
document_t *cu_build_cnlp_docs_file(char *);

#endif
