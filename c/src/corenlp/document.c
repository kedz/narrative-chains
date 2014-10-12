#include "corenlp/document.h"


static char *PTB_POS_STRINGS[] = { 
    "0", 
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
    "-LRB-",
    "-RRB-",
    "JJS",
    "#",
    "$",
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
    "prep_behind",
    "iobj",
    "npadvmod",
    "prepc_by",
    "prep_outside",
    "prep_during",
    "prepc_from",
    "prep_including",
    "expl",
    "csubj",
    "mwe",
    "quantmod",
    "prepc_because_of",
    "prepc_in",
    "number",
    "prep_since",
    "prepc_of",
    "parataxis",
    "prep_between",
    "prep_like",
    "prepc_while",
    "prep_without",
    "prep_toward",
    "prep_alongside",
    "acomp",
    "prep_around",
    "prep_onto",
    "prepc_before",
    "prep_until",
    "prep_because",
    "prep_because_of",
    "prep_among",
    "predet",
    "prep_despite",
    "prep_that",
    "prep_than",
    "conj_negcc",
    "prep_through",
    "discourse",
    "prepc_about",
    "prep_across",
    "prep_out",
    "prep_away_from",
    "prep_inside",
    "prep_near",
    "prep_throughout",
    "prep_down",
    "prepc_with",
    "prep_above",
    "prep_such_as",
    "csubjpass",
    "prep_per",
    "prep_upon",
    "prepc_despite",
    "conj_plus",
    "prep_out_of",
    "prep_amid",
    "prepc_at",
    "prepc_compared_with",
    "prepc_according_to",
    "prepc_on",
    "prep_within",
    "prepc_without",
    "prep_atop",
    "prep_as_of",
    "prepc_as_of",
    "prepc_inside",
    "prepc_close",
    "prepc_close_to",
    "prep_before",
    "prep_ahead_of",
    "prep_in_addition_to",
    "prepc_since",
    "prep_next_to",
    "prep_whether",
    "prep_off",
    "prep_beside",
    "prep_regarding",
    "prep_via",
    "prep_besides",
    "prep_aboard",
    "punct",
    "prepc_until",
    "prep_except",
    "prep_along_with",
    "prep_involving",
    "conj_and/or",
    "prepc_between",
    "prep_close_to",
    "prep_past",
    "prep_pending",
    "prep_outside_of",
    "prepc_as",
    "prep_following",
    "prepc_such_as",
    "conj",
    "prep_up",
    "prepc_than",
    "prep_in_place_of",
    "prep_in_front_of",
    "prep_starting",
    "prep_due_to",
    "prepc_as_to",
    "prepc_over",
    "prep_beyond",
    "prep_regardless_of",
    "prep_inside_of",
    "prep_prior_to",
    "prep_next",
    "prep_thanks_to",
    "prepc_compared_to",
    "prep_towards",
    "prep_on_behalf_of",
    "prep_off_of",
    "prep_till",
    "prepc_across_from",
    "prep_while",
    "prepc_through",
    "prepc_based_on",
    "prepc_besides",
    "prepc_out_of",
    "prep_if",
    "prep_below",
    "prep_instead_of",
    "prep_excluding",
    "prepc_near",
    "prepc_toward",
    "prepc_upon",
    "conj_just",
    "prep_de",
    "prep_across_from",
    "conj_only",
    "prepc_below",
    "prepc_together_with",
    "prepc_like",
    "prepc_along_with",
    "prepc_depending_on",
    "prep_worth",
    "prepc_into",
    "conj_of",
    "prepc_up",
    "prep_unlike",
    "prepc_inside_of",
    "prepc_amid",
    "prepc_next_to",
    "prepc_followed_by",
    "prep_followed_by",
    "prepc_instead_of",
    "prepc_along",
    "conj_in",
    "prep_contrary_to",
    "prep_on_account_of",
    "prepc_behind",
    "prepc_away_from",
    "prepc_throughout",
    "prep_though",
    "prepc_following",
    "prepc_due_to",
    "prep_together_with",
    "prepc_except",
    "prep_far_from",
    "prep_in_spite_of",
    "prepc_within",
    "prepc_during",
    "prepc_among",
    "conj_if",
    "prep_concerning",
    "prep_on_top_of",
    "prepc_ahead_of",
    "prepc_against",
    "conj_versus",
    "prepc_beyond",
    "prep_vs.",
    "prepc_onto",
    "conj_yet",
    "prepc_regardless_of",
    "conj_vs.",
    "prep_beneath",
    "prepc_in_addition_to",
    "prepc_under",
    "prep_barring",
    "prep_based_on",
    "prep_notwithstanding",
    "prepc_past",
    "prepc_apart_from",
    "prep_although",
    "prep_as_for",
    "prepc_out",
    "prep_apart_from",
    "prep_except_for",
    "prepc_pending",
    "prep_aside_from",
    "prepc_as_for",
    "prepc_above",
    "prep_nearer",
    "prepc_except_for",
    "prep_considering",
    "prep_scouting",
    "prep_unless",
    "conj_than",
    "prepc_around",
    "conj_minus",
    "prep_in_case_of",
    "prepc_regarding",
    "prepc_near_to",
    "prep_save",
    "prepc_prior_to",
    "prep_thru",
    "conj_vs",
    "prepc_towards",
    "prep_close_by",
    "prep_amidst",
    "prepc_'cause",
    "prepc_in_front_of",
    "prepc_close_by",
    "prepc_alongside",
    "prepc_far_from",
    "prepc_in_spite_of",
    "prep_by_means_of",
    "conj_'n'",
    "conj_v.",
    "prepc_across",
    "prepc_outside_of",
    "prep_as_to",
    "prepc_because",
    "prepc_although",
    "prep_according",
    "prep_in_accordance_with",
    "prep_racing",
    "prep_beginning",
    "prepc_off",
    "prepc_till",
    "prep_en",
    "prep_with_respect_to",
    "conj_+",
    "prepc_worth",
    "prep_'til",
    "prep_irrespective_of",
    "prep_accross",
    "prepc_starting",
    "conj_'n",
    "prep_underneath",
    "prepc_with_regard_to",
    "conj_through",
    "prep_opposite",
    "prep_according_to",
    "prep_throught",
    "prep_howst",
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

        if ((*sentence)->lemmas != NULL) {
            for (int i=0; i < (*sentence)->t_len; i++) {
                free ((*sentence)->lemmas[i]);
                (*sentence)->lemmas[i] = NULL;
            }
            free ((*sentence)->lemmas);
            (*sentence)->lemmas = NULL;
        }

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
    db->lemmas = NULL;
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
    db->lemmas = g_ptr_array_new ();
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

        if (db->lemmas->len > 0) {
            sentence->lemmas = (char **) malloc (sizeof(char *) * db->lemmas->len);
            if (sentence->lemmas == NULL) {
                fprintf (stderr, "Could not allocate memory for lemmas.\n");
                exit(1);
            }
            if (db->lemmas->len != sentence->t_len) {
                fprintf (stderr, "Different numbers of tokens and lemmas found.\n");
                exit(1);
            }
            for (int i=0; i < db->lemmas->len; i++) {
                sentence->lemmas[i] = (char *) db->lemmas->pdata[i];
            }
        }

        if (db->pos->len > 0) {
            sentence->pos = (pos_t *) malloc (sizeof(pos_t) * db->pos->len);
            if (sentence->pos == NULL) {
                fprintf (stderr, "Could not allocate memory for pos tags.\n");
                exit(1);
            }
            if (db->pos->len != sentence->t_len) {
                fprintf (stderr, "Different numbers of tokens and POS tags found.\n");
                exit(1);
            }
            for (int i=0; i < db->pos->len; i++) {
                sentence->pos[i] = ((pos_t *) db->pos->data)[i];        
            }
        } else {
            sentence->pos = NULL;
        }
 
        g_ptr_array_add (db->sentences, sentence);

        g_ptr_array_free (db->tokens, TRUE);
        g_ptr_array_free (db->lemmas, TRUE);
        db->tokens = NULL;
        db->lemmas = NULL;
        db->deps = NULL;
        g_array_free(db->pos, TRUE);
        db->pos = NULL;
    
    }
    
}

void
cu_doc_builder_add_token(
    DBuilder_t *db,
    char *token
) {
    g_ptr_array_add (db->tokens, token);
    db->cur_t_len++;
    db->cur_c_len += strlen(token) + 1;
}

void
cu_doc_builder_add_lemma(
    DBuilder_t *db,
    char *lemma
) {
    g_ptr_array_add (db->lemmas, lemma);
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

    if ((*db)->lemmas != NULL) {
        if (free_seg == TRUE) {
            for (int i=0; i < (*db)->lemmas->len; i ++)
                free ((*db)->lemmas->pdata[i]);
        }
        g_ptr_array_free ((*db)->lemmas, free_seg);
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
    const char *pos_token
) {

    for (int i=0; i < 48 ; i++) {
        if (strcmp(PTB_POS_STRINGS[i], pos_token)==0) {
            if (i < 41)
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
    const char *dep_str
) {

    for (int i=0; i < 306; i++) {
        if (strcmp(SU_CC_TYPED_DEPS[i], dep_str)==0) {
                return (dep_t) i;
        }
    }
    
    return (dep_t) 306;
    //printf ("%s not a SU dependency!\n", dep_str);
    //exit(1);


}
