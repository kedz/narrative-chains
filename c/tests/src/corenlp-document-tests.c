#include <stdio.h>
#include "corenlp/document.h"
#include <assert.h>

void test_sentence_as_string ()
{

    char *tokens[6] = {"This", "is", "an", "example", "sentence", "."};
    sentence_t *sentence = NULL;
    sentence = (sentence_t *) malloc(sizeof(sentence_t));
    sentence->tokens = tokens;
    sentence->t_len = 6;
    sentence->c_len = 30;

    char *str = NULL;
    str = cu_sentence_as_string (sentence, FALSE);
    assert (strcmp(str, "This is an example sentence .")==0);  
      g_print ("%s\n", str);

    free (sentence);   
    free (str); 

}

void test_document_as_string ()
{

    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    sentence_t *sentence1 = NULL;
    sentence1 = (sentence_t *) malloc(sizeof(sentence_t));
    sentence1->tokens = tokens1;
    sentence1->t_len = 6;
    sentence1->c_len = 30;

    char *tokens2[6] = {"This", "is", "another", "example", "sentence", "."};
    sentence_t *sentence2 = NULL;
    sentence2 = (sentence_t *) malloc(sizeof(sentence_t));
    sentence2->tokens = tokens2;
    sentence2->t_len = 6;
    sentence2->c_len = 35;

    document_t *document = NULL;
    document = (document_t *) malloc (sizeof(document_t));   
    document->sentences = (sentence_t **) malloc (sizeof(sentence_t *) * 2);
    document->sentences[0] = sentence1;
    document->sentences[1] = sentence2;
    document->s_len = 2;

    char *str = cu_document_as_string (document);
    char *dstr = "This is an example sentence . "
                 "This is another example sentence .";
    assert(strcmp(str, dstr)==0);  

     g_print ("%s\n", str);
   
    free (sentence1);
    free (sentence2);
    free (document->sentences);
    free (document);
    free (str);
//    free (sentence);   
//    free (str); 

}

void test_sentence_free ()
{

    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    sentence_t *sentence1 = NULL;
    sentence1 = (sentence_t *) malloc(sizeof(sentence_t));
    sentence1->tokens = NULL;
    sentence1->pos = NULL;
    sentence1->deps = NULL;
    sentence1->tokens = tokens1;
    sentence1->t_len = 6;
    sentence1->c_len = 30;

    cu_sentence_free (&sentence1, FALSE);
    printf ("test cu_sentence_free\n");
    assert (sentence1==NULL);
    assert (tokens1!=NULL);


    sentence_t *sentence2 = NULL;
    sentence2 = (sentence_t *) malloc(sizeof(sentence_t));
    sentence2->tokens = NULL;
    sentence2->pos = NULL;
    sentence2->deps = NULL;
    sentence2->tokens = (char **) malloc (sizeof(char *) * 3);
    sentence2->t_len = 3;
    sentence2->c_len = 9;

    sentence2->tokens[0] = (char *) malloc (sizeof(char) * 2);
    strcpy(sentence2->tokens[0], "A");
    sentence2->tokens[1] = (char *) malloc (sizeof(char) * 5);
    strcpy(sentence2->tokens[1], "test");
    sentence2->tokens[2] = (char *) malloc (sizeof(char) * 2);
    strcpy(sentence2->tokens[2], ".");
    cu_sentence_free (&sentence2, TRUE);
    assert (sentence2==NULL);

}

void 
test_document_free ()
{
    
}

void
test_document_builder_init () {
    
    


}


void 
test_document_builder_add_token ()
{
    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    int tlens1[6] = {4, 2, 2, 7, 8, 1};
    DBuilder_t *db = NULL;
    db = cu_doc_builder_new ();
    cu_doc_builder_new_sentence (db);
    cu_doc_builder_add_token (db, tokens1[0], tlens1[0]);
    

    printf ("test cu_doc_builder_add_token\n");
    assert (strcmp(g_ptr_array_index(db->tokens, 0), tokens1[0])==0);

    cu_doc_builder_free (&db, TRUE);    

}

void
test_document_builder_new_sentence ()
{

    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    int tlens1[6] = {4, 2, 2, 7, 8, 1};
    sentence_t *sentence1 = NULL;
    sentence1 = (sentence_t *) malloc(sizeof(sentence_t));
    sentence1->tokens = tokens1;
    sentence1->t_len = 6;
    sentence1->c_len = 30;

    DBuilder_t *db = NULL;
    db = cu_doc_builder_new ();
    cu_doc_builder_new_sentence (db);
    for (int i=0; i < 6; i++)
        cu_doc_builder_add_token (db, tokens1[i], tlens1[i]);
    cu_doc_builder_new_sentence (db);

    sentence_t *s = db->sentences->pdata[0];

    printf ("test cu_doc_builder_new_sentence\n");
    assert (sentence1->t_len==s->t_len);
    assert (sentence1->c_len==s->c_len);

    for (int i=0; i < s->t_len; i++)
        assert (strcmp(s->tokens[i], sentence1->tokens[i])==0);


    cu_doc_builder_free (&db, TRUE); 
    cu_sentence_free (&sentence1, FALSE);

}

void
test_document_builder_add_deps ()
{

    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    int tlens1[6] = {4, 2, 2, 7, 8, 1};
//    sentence_t *sentence1 = NULL;
//    sentence1 = (sentence_t *) malloc(sizeof(sentence_t));
//    sentence1->tokens = tokens1;
//    sentence1->t_len = 6;
//    sentence1->c_len = 30;

    DBuilder_t *db = NULL;
    db = cu_doc_builder_new ();
    for (int i=0; i < 6; i++)
        cu_doc_builder_add_token (db, tokens1[i], tlens1[i]);

    int gov_idx1 = 0;
    int dep_idx1 = 5;
    dep_t dtype1 = TD_ROOT;
    cu_doc_builder_add_dep(db, gov_idx1, dep_idx1, dtype1);
    int gov_idx2 = 5;
    int dep_idx2 = 1;
    dep_t dtype2 = TD_NSUBJ;
    cu_doc_builder_add_dep(db, gov_idx2, dep_idx2, dtype2);
    int gov_idx3 = 5;
    int dep_idx3 = 2;
    dep_t dtype3 = TD_COP;
    cu_doc_builder_add_dep(db, gov_idx3, dep_idx3, dtype3);
    int gov_idx4 = 5;
    int dep_idx4 = 3;
    dep_t dtype4 = TD_DET;
    cu_doc_builder_add_dep(db, gov_idx4, dep_idx4, dtype4);
    int gov_idx5 = 5;
    int dep_idx5 = 4;
    dep_t dtype5 = TD_NN;
    cu_doc_builder_add_dep(db, gov_idx5, dep_idx5, dtype5);


    cu_doc_builder_finish_sentence (db);
    sentence_t *s = db->sentences->pdata[0];
    assert (s->deps[gov_idx1][dep_idx1]==TD_ROOT);
    assert (s->deps[gov_idx2][dep_idx2]==TD_NSUBJ);
    assert (s->deps[gov_idx3][dep_idx3]==TD_COP);
    assert (s->deps[gov_idx4][dep_idx4]==TD_DET);
    assert (s->deps[gov_idx5][dep_idx5]==TD_NN);


    g_print ("FREEING BUILDER AFTER DEP ADDED\n");
    cu_doc_builder_free (&db, TRUE); 
//    cu_doc_builder_new_sentence (db);

//    sentence_t *s = db->sentences->pdata[0];

}

//      <dependencies type="collapsed-ccprocessed-dependencies">
//          <dep type="root">
//            <governor idx="0">ROOT</governor>
//            <dependent idx="5">sentence</dependent>
//          </dep>
//          <dep type="nsubj">
//            <governor idx="5">sentence</governor>
//            <dependent idx="1">This</dependent>
//          </dep>
//          <dep type="cop">
//            <governor idx="5">sentence</governor>
//            <dependent idx="2">is</dependent>
//          </dep>
//          <dep type="det">
//            <governor idx="5">sentence</governor>
//            <dependent idx="3">an</dependent>
//          </dep>
//          <dep type="nn">
//            <governor idx="5">sentence</governor>
//            <dependent idx="4">example</dependent>
//          </dep>
//        </dependencies> 

void 
test_document_builder_complete ()
{

    printf ("Testing document builder.\n");

    char *tokens1[6] = {"This", "is", "an", "example", "sentence", "."};
    pos_t pos1[6] = {PTB_DT, PTB_VBZ, PTB_DT, PTB_NN, PTB_NN, PTB_PUNC};
    int tlens1[6] = {4, 2, 2, 7, 8, 1};

    char *tokens2[6] = {"This", "is", "another", "example", "sentence", "."};
    pos_t pos2[6] = {PTB_DT, PTB_VBZ, PTB_DT, PTB_NN, PTB_NN, PTB_PUNC};
    int tlens2[6] = {4, 2, 7, 7, 8, 1};

    DBuilder_t *db = NULL;
    db = cu_doc_builder_new ();
    cu_doc_builder_new_sentence (db);

    for (int i=0; i < 6; i++) {
        cu_doc_builder_add_token(db, tokens1[i], tlens1[i]);
        cu_doc_builder_add_pos(db, pos1[i]);
    }    

    cu_doc_builder_new_sentence(db);
    
    for (int i=0; i < 6; i++) {
        cu_doc_builder_add_token(db, tokens2[i], tlens2[i]);
        cu_doc_builder_add_pos(db, pos2[i]);
    }    

    printf ("Finalizing doc...\n");
    document_t *doc = NULL;
    doc = cu_doc_builder_finalize(db);    

    


    cu_doc_builder_free (&db, TRUE); 

    char *s1 = NULL;
    s1 = cu_sentence_as_string (doc->sentences[0], TRUE);
    g_print ("Sentence 1\n==========\n%s\n\n", s1);
    free (s1);

    char *str = NULL;
    str = cu_document_as_string (doc);
    if (str != NULL) {
        g_print ("%s\n", str); 
        free (str);
    }
    
//    g_print ("Freeing doc\n");
    cu_document_free(&doc, TRUE);        


}



int main()
{

    test_sentence_free ();
    test_sentence_as_string ();
    test_document_as_string ();
    test_document_builder_add_token ();
    test_document_builder_new_sentence ();
    test_document_builder_complete ();


}
