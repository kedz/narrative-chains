#include "corenlp/document.h"


static char *PTB_POS_STRINGS[] = { 
    "", 
    "CC", 
    "CD", 
    "DT", 
    "EX", 
    "FW", 
    "IN",
    "JJ",
    "JJR",
    "LS",
    "MD",
    "NN",
    "NNS",
    "NNP",
    "NNPS",
    "PDT",
    "POS",
    "PRP",
    "PRP$",
    "RB",
    "RBR",
    "RBS",
    "RP",
    "SYM",
    "TO",
    "UH",
    "VB",
    "VBD",
    "VBG",
    "VBN",
    "VBP",
    "VBZ",
    "WDT",
    "WP",
    "WP$",
    "WRB",
    ".",
    ",",
    "''",
    "\"",
    "``",
    "''",
    ":"
};

static char *SU_CC_TYPED_DEPS[] = {
    "NONE",
    "root",
    "nsubj",
    "nn",
    "cop",
    "det",
    "poss",
    "amod",
    "dobj",
    "prep_of",
    "prep_in",
    "prep_for",
    "prep_over",
    "prep_after",
    "conj_and",
    "conj_or",
    "tmod",
    "mark",
    "vmod",
    "advcl",
    "agent",
    "aux",
    "ccomp",
    "neg",
    "advmod",
    "appos",
    "nsubjpass",
    "auxpass",
    "num",
    "dep",
    "pobj",
    "prep_from",
    "conj_but",
    "prepc_including",
    "prt",
    "cc",
    "xcomp",
    "prep_with",
    "prep_at",
    "prep_under",
    "prep_to",
    "rcmod",
    "prepc_outside",
    "possessive",
    "prep",
    "pcomp",
    "prep_on",
    "prep_into",
    "prepc_for",
    "prep_by",
    "prep_as",
    "prepc_to",
    "preconj",
    "conj_nor",
    "prep_about",
    "prepc_after",
    "prep_against",
    "prep_along",
    "prep_behind"
};


void
cu_sentence_free(
    sentence_t **sentence,
    gboolean free_seg
) {

    if (free_seg==TRUE) {
        for (int i=0; i < (*sentence)->t_len; i++) {
            free ((*sentence)->tokens[i]);
            (*sentence)->tokens[i] = NULL;
        }
        free ((*sentence)->tokens);
        (*sentence)->tokens = NULL;

        if ((*sentence)->pos!=NULL) {
            free ((*sentence)->pos);
            (*sentence)->pos = NULL;
        }

        if ((*sentence)->deps!=NULL) {
            int dep_mat_dim = (*sentence)->t_len + 1;
            for (int i=0; i < dep_mat_dim; i++)
                free ((*sentence)->deps[i]);
            free ((*sentence)->deps);
            (*sentence)->deps = NULL;
        }
    }

    free (*sentence);
    *sentence = NULL;

}

void
cu_document_free(
    document_t **document,
    gboolean free_seg
) {

    if (free_seg==TRUE) {
        for (int i=0; i < (*document)->s_len; i++) {
            cu_sentence_free (&((*document)->sentences[i]), TRUE);
        }
        free ((*document)->sentences);
        (*document)->sentences = NULL;
    }
    
    free (*document);
    *document = NULL;

}


char *
cu_sentence_as_string(
    sentence_t *sentence, 
    gboolean inc_pos
)
{
    GString *buffer = NULL;
    buffer = g_string_new (NULL);
    for (int i=0; i < sentence->t_len; i++) {
        g_string_append (buffer, sentence->tokens[i]);
        if (inc_pos==TRUE) {
            g_string_append (buffer, "/");            
            g_string_append (buffer, cu_pos_string (sentence->pos[i]));        
        }
        if (i < sentence->t_len - 1)
            g_string_append (buffer, " ");    
    }

    return g_string_free (buffer, FALSE);




//    char *buffer = NULL;
//    int w_pos = 0;

//    buffer = (char *) malloc(sizeof(char) * (sentence->c_len));
//    for (int i=0; i < sentence->t_len; i++) {
//        int j = 0;
//        char *token = sentence->tokens[i];
//        for (;;) {
//            if (token[j] == '\0')
//                break;
//            else
//                buffer[w_pos++] = token[j++];
//        } 
//        if (i < sentence->t_len - 1)
//            buffer[w_pos++] = ' ';
//    }
//
//    buffer[w_pos] = '\0';
//
//    return buffer;

}



char *
cu_document_as_string(document_t *document)
{

    unsigned int buffer_size = 0;
    for (int i=0; i < document->s_len; i++) {
        buffer_size += document->sentences[i]->c_len;
    }

    char *buffer = NULL;
    buffer = (char *) malloc (sizeof(char) * buffer_size);
    buffer[buffer_size-1] = '\0';
    int w_pos = 0;
        
    for (int i=0; i < document->s_len; i++) {
        for (int j=0; j < document->sentences[i]->t_len; j++) {
            int k = 0;
            for (;;) {
                if (document->sentences[i]->tokens[j][k] == '\0')
                    break;
                else
                    buffer[w_pos++] = document->sentences[i]->tokens[j][k++];
            }
            if (j < document->sentences[i]->t_len - 1)
                buffer[w_pos++] = ' ';
        }
        if (i < document->s_len - 1)
            buffer[w_pos++] = ' ';
    }
    return buffer;

}



DBuilder_t * 
cu_doc_builder_new ()
{
    DBuilder_t *db = NULL;
    db = (DBuilder_t *) malloc (sizeof(DBuilder_t));
    // add test here

    db->sentences = g_ptr_array_new ();
    db->tokens = NULL; //g_ptr_array_new (); 
    db->pos = NULL;//g_array_new (FALSE, FALSE, sizeof(pos_t));


    db->cur_t_len = 0;
    db->cur_c_len = 0;
   
    db->deps = NULL;
    
    return db;
}

void
cu_doc_builder_new_sentence(
    DBuilder_t *db
) {

    cu_doc_builder_finish_sentence (db);
    db->tokens = g_ptr_array_new ();
    db->pos = g_array_new (FALSE, FALSE, sizeof(pos_t));
    db->cur_t_len = 0;
    db->cur_c_len = 0;
}

void
cu_doc_builder_finish_sentence(
    DBuilder_t *db
) {

    if (db->tokens != NULL && db->tokens->len > 0) {
        sentence_t* sentence = NULL;
        sentence = (sentence_t *) malloc (sizeof(sentence_t));
        sentence->tokens = 
            (char **) malloc (sizeof(char *) * db->tokens->len);
        for (int i=0; i < db->tokens->len; i++) {
            sentence->tokens[i] = db->tokens->pdata[i];
        }
        sentence->t_len = db->cur_t_len;
        sentence->c_len = db->cur_c_len;
        sentence->deps = db->deps;

        if (db->pos->len > 0) {
            sentence->pos = (pos_t *) malloc (sizeof(pos_t) * db->pos->len);
            for (int i=0; i < db->pos->len; i++) {
                sentence->pos[i] = ((pos_t *) db->pos->data)[i];        
            }
        } else {
            sentence->pos = NULL;
        }
 
        g_ptr_array_add (db->sentences, sentence);

        g_ptr_array_free (db->tokens, TRUE);
        db->tokens = NULL;
        db->deps = NULL;
        g_array_free(db->pos, TRUE);
        db->pos = NULL;
    
    }
    
}

void
cu_doc_builder_add_token(
    DBuilder_t *db,
    char *token,
    unsigned int len
) {
    char *token_cpy = NULL;
    token_cpy = (char *) malloc (sizeof(char) * (len + 1));
    strncpy (token_cpy, token, len);
    token_cpy[len] = '\0'; 
    g_ptr_array_add (db->tokens, token_cpy);
    db->cur_t_len++;
    db->cur_c_len += len + 1;
}

void
cu_doc_builder_add_pos (
    DBuilder_t *db,
    pos_t pos_tag
) {
    g_array_append_val (db->pos, pos_tag);

}

void
cu_doc_builder_add_dep(
    DBuilder_t *db,
    int gov_idx,
    int dep_idx,
    dep_t dep_type
) {

    if (db->deps==NULL) {
        int dep_mat_dim = db->tokens->len + 1;
        db->deps = (dep_t **) malloc (sizeof(dep_t *) * dep_mat_dim);
        for (int i=0; i < dep_mat_dim; i++)
            db->deps[i] = (dep_t *) calloc (dep_mat_dim, sizeof(dep_t));
    }

    db->deps[gov_idx][dep_idx] = dep_type;

   

}

document_t *
cu_doc_builder_finalize(
    DBuilder_t *db
) {

    cu_doc_builder_finish_sentence (db);
    document_t *doc = NULL;

    if (db->sentences->len > 0) {    
        doc = (document_t *) malloc (sizeof(document_t));
        doc->s_len = db->sentences->len;
        doc->sentences =  
            (sentence_t **) malloc (sizeof(sentence_t *) * doc->s_len);
        for (int i=0; i < doc->s_len; i++) {
            doc->sentences[i] = db->sentences->pdata[i];
        }
        g_ptr_array_free (db->sentences, TRUE);
        db->sentences = NULL;
    }
    return doc;
}

void
cu_doc_builder_free(
    DBuilder_t **db,
    gboolean free_seg
) {



    if ((*db)->deps!=NULL) {
        int dep_mat_dim = (*db)->tokens->len + 1;
        for (int i=0; i < dep_mat_dim; i++)
            free ((*db)->deps[i]);
        free ((*db)->deps);
        (*db)->deps = NULL;
    }

    
    if ((*db)->pos!=NULL) {
        g_array_free ((*db)->pos, TRUE);
        (*db)->pos = NULL;
    }


    if ((*db)->tokens != NULL) {
        if (free_seg == TRUE) {
            for (int i=0; i < (*db)->tokens->len; i ++)
                free ((*db)->tokens->pdata[i]);
        }
        g_ptr_array_free ((*db)->tokens, free_seg);
    }

    if ((*db)->sentences != NULL) {
        if (free_seg == TRUE) {
            for (int i=0; i < (*db)->sentences->len; i ++)
                cu_sentence_free ((sentence_t **) &((*db)->sentences->pdata[i]), TRUE);
        }    
        g_ptr_array_free ((*db)->sentences, free_seg);        
    }


    
    free (*db);
    *db = NULL;
}


char *
cu_pos_string(
    pos_t pos
) {    
    return PTB_POS_STRINGS[pos];
}

char *
cu_dep_string(
    dep_t dep
) {    
    return SU_CC_TYPED_DEPS[dep];
}

pos_t cu_char_to_pos_t(
    char *pos_token
) {

    for (int i=0; i < 44 ; i++) {
        if (strcmp(PTB_POS_STRINGS[i], pos_token)==0) {
            if (i < 36)
                return (pos_t) i;
            else
                return PTB_PUNC;
        }
    }

    printf ("%s not a PTB POS tag!\n", pos_token);
    exit(1);
    
}

dep_t 
cu_char_to_dep_t(
    char *dep_str
) {

    for (int i=0; i < 59; i++) {
        if (strcmp(SU_CC_TYPED_DEPS[i], dep_str)==0) {
                return (dep_t) i;
        }
    }

    
    printf ("%s not a SU dependency!\n", dep_str);
    exit(1);


}
