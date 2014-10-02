#include "corenlp/xmlparser.h"

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


static void
element_start(
    void *data,
    const xmlChar *name,
    const xmlChar **atts
)
{
    cnlp_xmlp_state_s *state = (cnlp_xmlp_state_s *)data;
    

    if (strcmp((char *)name, "word")==0) {
//        printf ("IN Word\n");
        state->_in_word = TRUE;
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        xdb->token_buffer = g_string_new(NULL); 
    } else if (strcmp((char *)name, "lemma")==0) {
//        printf ("IN lemma\n");
        state->_in_lemma = TRUE;
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        xdb->lemma_buffer = g_string_new(NULL); 
    } else if (strcmp((char *)name, "POS")==0) {
 //       printf ("IN pos\n");
        state->_in_pos = TRUE;
    } else if (strcmp((char *)name, "token")==0) {
        state->_in_token = TRUE;
    } else if (strcmp((char *)name, "dep")==0) {
        if (state->_in_dependencies) {
            state->_in_dep = TRUE;            
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            xdb->dep_type = cu_char_to_dep_t(atts[1]);
        }
    } else if (strcmp((char *)name, "governor")==0) {
        if (state->_in_dep) {
            state->_in_governor = TRUE;   
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            xdb->gov_idx = atoi(atts[1]);
        }
    } else if (strcmp((char *)name, "dependent")==0) {
        if (state->_in_dep) {
            state->_in_dependent = TRUE;
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            xdb->dep_idx = atoi(atts[1]);
            cu_doc_builder_add_dep (
                xdb->db, xdb->gov_idx, xdb->dep_idx, xdb->dep_type);
        }

    } else if (strcmp((char *)name, "dependencies")==0) {
   //     printf ("In dependencies\n");
        if (strcmp("collapsed-ccprocessed-dependencies", atts[1])==0) {
            state->_in_dependencies = TRUE;
        }
    } else if (strcmp((char *)name, "tokens")==0) {
     //   printf ("In tokens\n");
        state->_in_tokens = TRUE;
    } else if (strcmp((char *)name, "sentence")==0) {
       // printf ("In sentence\n");
        state->_in_sentence = TRUE;
        if (state->_in_sentences==TRUE) {
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            cu_doc_builder_new_sentence (xdb->db);
        }
    } else if (strcmp((char *)name, "sentences")==0) {
//        printf ("In sentences\n");
        state->_in_sentences = TRUE;
    } else if (strcmp((char *)name, "document")==0) {
  //      printf ("In doc\n");
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        xdb->db = cu_doc_builder_new();
    }
}

static void
element_end(
    void *data,
    const xmlChar *name
)
{
    cnlp_xmlp_state_s *state = (cnlp_xmlp_state_s *) data;

    if (strcmp((char *)name, "word")==0) {
   //     printf ("OUT Word\n");
        state->_in_word = FALSE;
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        cu_doc_builder_add_token (
            xdb->db, g_string_free (xdb->token_buffer, FALSE));
         
    } else if (strcmp((char *)name, "lemma")==0) {
     //   printf ("OUT lemma\n");
        state->_in_lemma = FALSE;
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        cu_doc_builder_add_lemma (
            xdb->db, g_string_free (xdb->lemma_buffer, FALSE));
    } else if (strcmp((char *)name, "POS")==0) {
//        printf ("OUT pos\n");
        state->_in_pos = FALSE;
    } else if (strcmp((char *)name, "token")==0) {
        state->_in_token = FALSE;
    } else if (strcmp((char *)name, "tokens")==0) {
        state->_in_tokens = FALSE;
    } else if (strcmp((char *)name, "dep")==0) {
        state->_in_dep = FALSE;
    } else if (strcmp((char *)name, "governor")==0) {
        state->_in_governor = FALSE;
    } else if (strcmp((char *)name, "dependent")==0) {
        state->_in_dependent = FALSE;
    } else if (strcmp((char *)name, "dependencies")==0) {
        state->_in_dependencies = FALSE;
    } else if (strcmp((char *)name, "sentence")==0) {
        state->_in_sentence = FALSE;
    } else if (strcmp((char *)name, "sentences")==0) {
        state->_in_sentences = FALSE;
    } else if (strcmp((char *)name, "document")==0) {
 //       printf ("Leaving doc\n");
        cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
        xdb->doc = cu_doc_builder_finalize (xdb->db);
        cu_doc_builder_free (&(xdb->db), TRUE);
    }
}

void 
_handle_chars(
    void *data,
    const xmlChar *ch,
    int len
) {
    
    cnlp_xmlp_state_s *state = (cnlp_xmlp_state_s *) data;
    if (state->_in_tokens && state->_in_token) {
        if (state->_in_word) {
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            if (xdb->token_buffer->len > 0) {
                g_string_append_c (xdb->token_buffer, ' ');
            } 
            g_string_append_len (xdb->token_buffer, ch, len);
            
        } else if (state->_in_lemma) {
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            if (xdb->lemma_buffer->len > 0) {
                g_string_append_c (xdb->lemma_buffer, ' ');
            } 
            g_string_append_len (xdb->lemma_buffer, ch, len);
            
        } else if (state->_in_pos) {
            cnlp_xml_dbuilder_t *xdb = (cnlp_xml_dbuilder_t *) state->data;
            char *pos_token = NULL;
            pos_token = (char *) malloc(sizeof(char) * (len + 1));
            pos_token[len] = '\0';           
            strncpy (pos_token, (char *) ch, len); 
            pos_t tag = cu_char_to_pos_t (pos_token);
            cu_doc_builder_add_pos (xdb->db, tag); 
            free (pos_token);
        } 

    } else if (state->_in_governor) {

        char *pos_token = NULL;
        pos_token = (char *) malloc(sizeof(char) * (len + 1));
        strncpy (pos_token, (char *) ch, len); 
        pos_token[len] = '\0';           
        free (pos_token);
        
    } else if (state->_in_dependent) {
        char *pos_token = NULL;
        pos_token = (char *) malloc(sizeof(char) * (len + 1));
        strncpy (pos_token, (char *) ch, len); 
        pos_token[len] = '\0';           
        free (pos_token);
        

    }

}

static void
my_error(void *user_data, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    g_logv("XML", G_LOG_LEVEL_CRITICAL, msg, args);
    va_end(args);
}


static xmlSAXHandler corenlp_xml_parser = {
    NULL,                       /* internalSubset */
    NULL,                       /* isStandalone */
    NULL,                       /* hasInternalSubset */
    NULL,                       /* hasExternalSubset */
    NULL,                       /* resolveEntity */
    NULL,                       /* getEntity */
    NULL,                       /* entityDecl */
    NULL,                       /* notationDecl */
    NULL,                       /* attributeDecl */
    NULL,                       /* elementDecl */
    NULL,                       /* unparsedEntityDecl */
    NULL,                       /* setDocumentLocator */
    NULL,            /* startDocument */
    NULL,              /* endDocument */
    element_start,             /* startElement */
    element_end,               /* endElement */
    NULL,                       /* reference */
    _handle_chars,               /* characters */
    NULL,                       /* ignorableWhitespace */
    NULL,                       /* processingInstruction */
    NULL, //mycomments,                 /* comment */
    my_error, //(warningSAXFunc) & mywarn,  /* xmlParserWarning */
    my_error,     /* xmlParserError */
    my_error, //(fatalErrorSAXFunc) & myerr,        /* xmlfatalParserError */
    NULL,                       /* getParameterEntity */
    NULL,                       /* cdataBlock -- should we handle this too *
                                 * ?? */
    NULL,                       /* externalSubset; */
    XML_SAX2_MAGIC,
    NULL,
    NULL, //mystartElementNs,           /* startElementNs */
    NULL, //myendElementNs,             /* endElementNs */
    NULL                        /* xmlStructuredErrorFunc */
};


document_t *
cu_build_cnlp_docs_memory_full(
    char *buffer,
    int size
) {
    cnlp_xml_dbuilder_t xdb = {NULL, NULL, 0, 0, 0};
    cnlp_xmlp_state_s state = 
        {&xdb, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
    xmlSAXUserParseMemory(&corenlp_xml_parser, &state, buffer, size);
    
    //char str* = cu_document_as_string (
    return xdb.doc;

}

document_t *
cu_build_cnlp_docs_file(
    char *path
) {
    cnlp_xml_dbuilder_t xdb = {NULL, NULL, 0, 0, 0};
    cnlp_xmlp_state_s state =
        {&xdb, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
    xmlSAXUserParseFile(&corenlp_xml_parser, &state, path);

    return xdb.doc;
}

