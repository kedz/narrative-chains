#include <glib.h>
#include <stdlib.h>
#include <string.h>


#ifndef CORENLP_DOCUMENT_H
#define CORENLP_DOCUMENT_H

typedef enum dep_t { 
    TD_NONE=0, 
    TD_ROOT=1, 
    TD_NSUBJ=2, 
    TD_NN=3,
    TD_COP=4, 
    TD_DET=5,
    TD_POS=6,
    TD_AMOD=7,
    TD_DOBJ=8,
    TD_PREP_OF=9,
    TD_PREP_IN=10,
    TD_PREP_FOR=11,
    TD_PREP_OVER=12,
    TD_PREP_AFTER=13,
    TD_CONJ_AND=14,
    TD_CONJ_OR=15,
    TD_TMOD=16,
    TD_MARK=17,
    TD_VMOD=18,
    TD_ADVCL=19,
    TD_AGENT=20,
    TD_AUX=21,
    TD_CCOMP=22,
    TD_NEG=23,
    TD_ADVMOD=24,
    TD_APPOS=25,
    TD_NSUBJPASS=26,
    TD_AUXPASS=27,
    TD_NUM=28,
    TD_DEP=29,
    TD_POBJ=30,
    TD_PREP_FROM=31,
    TD_CONJ_BUT=32,
    TD_PREPC_INCLUDING=33,
    TD_PRT=34,
    TD_CC=35,
    TD_XCOMP=36,
    TD_PREP_WITH=37,
    TD_PREP_AT=38,
    TD_PREP_UNDER=39,
    TD_PREP_TO=40,
    TD_RCMOD=41,
    TD_PREPC_OUTSIDE=42,
    TD_POSSESSIVE=43,
    TD_PREP=44,
    TD_PCOMP=45,
    TD_PREP_ON=46,
    TD_PREP_INTO=47,
    TD_PREPC_FOR=48,
    TD_PREP_BY=49,
    TD_PREP_AS=50,
    TD_PREPC_TO=51,
    TD_PRECONJ=52,
    TD_CONJ_NOR=53,
    TD_PREP_ABOUT=54,
    TD_PREPC_AFTER=55,
    TD_PREP_AGAINST=56,
    TD_PREP_ALONG=57,
    TD_PREP_BEHIND=58
} dep_t;

typedef enum pos_t { PTB_NONE=0, 
                     PTB_CC=1,
                     PTB_CD=2,
			         PTB_DT=3,
                     PTB_EX=4,
                     PTB_FW=5,
                     PTB_IN=6,
                     PTB_JJ=7,
                     PTB_JJR=8,
                     PTB_LS=9,
                     PTB_MD=10,
                     PTB_NN=11,
                     PTB_NNS=12,
                     PTB_NNP=13,
                     PTB_NNPS=14,
                     PTB_PDT=15,
                     PTB_POS=16,
                     PTB_PRP=17,
                     PTB_PRP$=18,
                     PTB_RB=19,
                     PTB_RBR=20,
                     PTB_RBS=21,
                     PTB_RP=22,
                     PTB_SYM=23,
                     PTB_TO=24,
                     PTB_UH=25,
                     PTB_VB=26,
                     PTB_VBD=27,
                     PTB_VBG=28,
                     PTB_VBN=29,
                     PTB_VBP=30,
                     PTB_VBZ=31,
                     PTB_WDT=32,
                     PTB_WP=33,
                     PTB_WP$=34,
                     PTB_WRB=35,
                     PTB_PUNC=36} pos_t;

typedef struct sentence_t {
    char **tokens;
    pos_t *pos;
    unsigned int t_len;  
    unsigned int c_len;
    dep_t **deps;
} sentence_t;

typedef struct document_t {
    sentence_t **sentences;
    unsigned int s_len;
} document_t;

typedef struct DBuilder_t {
    GPtrArray *sentences;
    GPtrArray *tokens;
    GArray *pos;
    unsigned int cur_t_len;
    unsigned int cur_c_len;
    dep_t **deps; 
} DBuilder_t;


char *cu_sentence_as_string (sentence_t *, gboolean);
char *cu_document_as_string (document_t *);

void cu_sentence_free (sentence_t **sentence, gboolean free_seg);
void cu_document_free (document_t **document, gboolean free_seg);

DBuilder_t *cu_doc_builder_new ();
void cu_doc_builder_new_sentence (DBuilder_t *);
void cu_doc_builder_finish_sentence (DBuilder_t *);
void cu_doc_builder_add_token (DBuilder_t *, char *, unsigned int);
void cu_doc_builder_add_pos (DBuilder_t *, pos_t);
void cu_doc_builder_add_dep(DBuilder_t *, int, int, dep_t);

pos_t cu_char_to_pos_t(char *);
dep_t cu_char_to_dep_t(char *);

document_t *cu_doc_builder_finalize (DBuilder_t *);
void cu_doc_builder_free (DBuilder_t **, gboolean);

char *cu_pos_string(pos_t);
char *cu_dep_string(dep_t);
//d_builder_t *cnlp_doc_builder_new();
//void cnlp_add_token(d_builder_t *, char *);
//void cnlp_add_sentence(d_builder_t *);

#endif 
