#include "apps/corenlp-counter/corenlp-counter.h"

// Forward declarations of private functions.
GPtrArray *
__validate_files(
    GPtrArray *file_args, 
    gboolean recurse, 
    gboolean verbose);

gboolean
__validate_output_file(
    GFile *file);

void
__process_file(
    GFile *file,
    opt_s *options,
    gboolean verbose);

document_t *
__build_cnlp_docs_from_istream(
    GInputStream *istream,
    gboolean verbose);

gboolean
_is_gz(
    GFile *file);

void usage(){
    printf("Usage: corenlp-counter -vr -o <OUTFILE> <PATH1 PATH2...>\n");
}

gboolean _is_gz(file)
    GFile *file;
{
    char *path = g_file_get_path (file);
    char *gz = ".gz";
    int slen = strlen(path);

    if ((3 <= slen) && (strcmp(path+slen-3, gz)==0)) {
        free (path);
        return TRUE;
    } else {
        free (path);
        return FALSE;
    }
}

GPtrArray *
__validate_files(file_args, recurse, verbose)
    GPtrArray *file_args;
    gboolean recurse;
    gboolean verbose;
{
    GPtrArray *valid_files = g_ptr_array_new ();

    for (int f=0; f < file_args->len; f++) {
        GFile *file = (GFile *) file_args->pdata[f];
        GFileType type = g_file_query_file_type (
            file, G_FILE_QUERY_INFO_NONE, NULL);
        if (type==G_FILE_TYPE_DIRECTORY) {
            if (recurse==TRUE) {
                GPtrArray *dir_files = cu_slurp_dir_contents (file);
                while (dir_files->len > 0) {
                    g_ptr_array_add (
                        valid_files, g_ptr_array_remove_index (dir_files, 0));
                }
                g_ptr_array_free (dir_files, TRUE);
            } else if (verbose==TRUE) {
                char *path = g_file_get_path (file);
                printf ("Ignoring directory (%s) -- retry with -r\n",
                    path);
                free (path);    
            }
        } else if (type==G_FILE_TYPE_REGULAR) {
            g_ptr_array_add (valid_files, (gpointer) g_file_dup (file));
        } else if (verbose==TRUE) {
            char *bad_path = g_file_get_path (file);
            printf ("Ignoring (%s) -- not a file or directory.\n",
                bad_path);
            free (bad_path);
        }
    }
    return valid_files;
}

gboolean
__validate_output_file(file)
    GFile *file;
{

    GFile *parent = g_file_get_parent (file);
    if (g_file_query_exists (parent, NULL)==FALSE) {
        gboolean is_success = 
            g_file_make_directory_with_parents (parent, NULL, NULL);
        g_object_unref (parent);
        return is_success;
    } else {
        g_object_unref (parent);
        return TRUE;
    }
}


void
__process_file(file, options, verbose)
    GFile *file;
    opt_s *options;
    gboolean verbose;
{

    char *path = g_file_get_path (file);
    
    GFileInfo *file_info = g_file_query_info (
        file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
   
    const char *content_type = g_file_info_get_content_type (file_info);
    char *desc = g_content_type_get_description (content_type); 
    printf (
        "%s : content type: %s : content desc %s\n", path, content_type, desc);
    if (strcmp(desc, "XML document")==0) {
        GInputStream *istream = (GInputStream *) g_file_read (
            file, NULL, NULL);
 
        document_t *doc = __build_cnlp_docs_from_istream (
            istream, verbose);
        if (doc!=NULL) {
            if (options->_process_document!=NULL) {
                (*(options->_process_document)) (options->data, doc);       
            }
            cu_document_free (&doc, TRUE);
        }

        g_input_stream_close (istream, NULL, NULL);
        g_object_unref (istream);
    } else if (strcmp(desc, "Gzip archive")==0) {
        GConverter *decompressor = NULL;
        decompressor = (GConverter *) g_zlib_decompressor_new (
            G_ZLIB_COMPRESSOR_FORMAT_GZIP);            
        GInputStream *istream = (GInputStream *) g_file_read (
            file, NULL, NULL);
        
        GInputStream *fistream = g_converter_input_stream_new (
            istream, decompressor);

        document_t *doc = __build_cnlp_docs_from_istream (fistream, verbose);
        if (doc!=NULL) {
            if (options->_process_document!=NULL) {
                (*(options->_process_document)) (options->data, doc);       
            }
            cu_document_free (&doc, TRUE);
        }
        
        g_input_stream_close (istream, NULL, NULL);
        g_object_unref (istream);   
        g_object_unref (fistream);
        g_object_unref (decompressor);
    
   }

    g_object_unref (file_info);
    free (path);
    //free ((char *) content_type);
    free (desc);

}

document_t *
__build_cnlp_docs_from_istream(istream, verbose)
    GInputStream *istream;
    gboolean verbose;
{

    xmlSAXHandler *sax_handler = get_corenlp_xml_sax_handler_ptr ();
    cnlp_xml_dbuilder_t xdb = {NULL, NULL, 0, 0, 0};
    cnlp_xmlp_state_s *state = cu_cnlp_xmlp_state_new ();
    state->data = (void *) &xdb;

    gsize buffer_size = 16384;
    char *buffer = (char *) malloc (sizeof(char) * buffer_size);
    gssize read = g_input_stream_read (
        istream, (void *) buffer, 4, NULL, NULL);
    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt (
        sax_handler, (void *) state, buffer, 4, NULL);
    
    for (;;) {

        read = g_input_stream_read (
            istream, (void *) buffer, buffer_size, NULL, NULL);
        if (read==0)
            break;
        int errno = xmlParseChunk (
            ctxt, buffer, read, 0);
        if (errno!=0) {
            fprintf (stderr, "SAX Parser returned error num: %d\n", errno);
            return NULL;
        }       
    } 
    xmlParseChunk (
        ctxt, buffer, 0, 1);

    xmlFreeParserCtxt (ctxt);
    free (state);
    free (buffer);
    return xdb.doc;

}

int main(int argc, char **argv) 
{
    GPtrArray *file_args;
    gboolean recurse = FALSE;
    gboolean verbose = FALSE;
    GFile *output_file = NULL;
    cc_mode_t mode = CHAMBERS;

    if (argc == 1) {

        usage();
        exit(0);

    } else {
        file_args = g_ptr_array_new();
        for (int i=1; i < argc; i++) {
            if (strcmp(argv[i], "-r")==0) {
                recurse = TRUE;
            } else if (strcmp(argv[i], "-v")==0) {
                verbose = TRUE;
            } else if (strcmp(argv[i], "-m")==0) {
                if (++i < argc) {
                    if (strcmp(argv[i],"chambers")==0) {
                        mode = CHAMBERS;
                    } else if (strcmp(argv[i], "chambers-dir")==0) {
                        mode = CHAMBERS_DIR;
                    } else if (strcmp(argv[i],"embedding")==0) {
                        mode = EMBEDDING;
                    } else if (strcmp(argv[i], "embedding-dir")==0) {
                        mode = EMBEDDING_DIR;
                    }
                }
            } else if (strcmp(argv[i], "-o")==0) {
                if (++i < argc)
                    output_file = g_file_new_for_commandline_arg (argv[i]);
            } else {
                GFile *file_arg= g_file_new_for_commandline_arg (argv[i]);
                g_ptr_array_add (file_args, (gpointer) file_arg);
            }
        }
    }

    /* Validate output file, and create necessary directories. */
    if (output_file==NULL) {
        output_file = g_file_new_for_path ("nc-counts.gz");
    }
    if (__validate_output_file (output_file)!=TRUE) {
        char *path = g_file_get_path (output_file);
        printf ("Could not create output file %s\n", path);
        free (path);    
        exit(EXIT_FAILURE);
    } 
    
    /* Validate file paths and get files in directories if in 
     * recursive mode.
     **/
    GPtrArray *valid_files = __validate_files (file_args, recurse, verbose);

    /* Free file arguments -- we don't need them anymore since valid_files
     * contains all the files that need processing.
     **/
    g_ptr_array_set_free_func (file_args, (GDestroyNotify) g_object_unref);
    g_ptr_array_free (file_args, TRUE);

    opt_s *options = NULL;

    if (mode==CHAMBERS) {
        if (verbose==TRUE)
            printf ("Generating counts for Chambers model.\n");
        options = cu_corenlp_counter_chambers_options_new (FALSE);
    } else if (mode==CHAMBERS_DIR) {
        if (verbose==TRUE)
            printf ("Generating counts for Chambers directional model.\n");
        options = cu_corenlp_counter_chambers_options_new (TRUE);

    } else if (mode==EMBEDDING) {
        if (verbose==TRUE)
            printf ("Generating counts for embedding model.\n");

    } else if (mode==EMBEDDING_DIR) {
        if (verbose==TRUE)
            printf ("Generating counts for embedding directional model.\n");

    }

//        usage();
//        exit(1);
//    } else if (strcmp(mode, "chambers")==0) {        
//        options = cu_corenlp_counter_chambers_options_new();
//        if (verbose)
//            printf ("Producing Chambers-style narrative chain counts...\n");
//    //} else if (strcmp(mode, "?")==0) {
//    //opt_s * options = nc_options_new();
//        
//    } else {
//        printf ("%s is not a valid mode.\n");
//        exit(1);
//    }
 

    for (int f=0; f < valid_files->len; f++) {
        GFile *file = (GFile *) valid_files->pdata[f];
        if (verbose==TRUE) {
            char *path = g_file_get_path (file);
            printf ("%s\n", path);     
            free (path);
        }
        __process_file (file, options, verbose);
        
    }


    /* Clean up processed file lists */
    g_ptr_array_set_free_func (valid_files, (GDestroyNotify) g_object_unref);
    g_ptr_array_free (valid_files, TRUE);

//
//    if (mode==NULL) {
//        usage();
//        exit(1);
//    } else if (strcmp(mode, "chambers")==0) {        
//        options = cu_corenlp_counter_chambers_options_new();
//        if (verbose)
//            printf ("Producing Chambers-style narrative chain counts...\n");
//    //} else if (strcmp(mode, "?")==0) {
//    //opt_s * options = nc_options_new();
//        
//    } else {
//        printf ("%s is not a valid mode.\n");
//        exit(1);
//    }
//    printf ("Mode is %s\n", mode); 
////    if (mode == STATS) {
////        g_print("Making struct\n");
////        options->data = new_stats_data();
////        options->_xml_file_handler = stats_xml_file_handler;
////        options->_x_gzip_file_handler = stats_x_gzip_file_handler;
////        options->_finish_processing = stats_finish_processing;
////        options->_free = stats_free;
////    }
//
//    const char *path = NULL;
//    int num_errors = 0;
//    for (int i=0; i < pathlist->len; i++) {
//        path = (char *) g_ptr_array_index (pathlist, i);
//        CU_NEWSTRCPY(path_cpy, path)
//        num_errors += 
//            process_path (options, (gchar *) path_cpy, recurse, NULL, verbose);        
//
//        //g_thread_pool_push (pool, path, NULL);
//        //num_errors += process_path (options, path, recurse, pool);
//    }
//    g_ptr_array_free (pathlist, TRUE);
//
////    char *outpath = NULL;

    if (options->_finish_processing != NULL) {
        if (_is_gz (output_file)) {
            GOutputStream *ostream = (GOutputStream *) g_file_replace (
                output_file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL);

            GConverter *compressor = NULL;
            compressor = (GConverter *) g_zlib_compressor_new (
                G_ZLIB_COMPRESSOR_FORMAT_GZIP, 0);            
            GOutputStream *fostream = 
                (GOutputStream *) g_converter_output_stream_new (
                    ostream, compressor);
                    
            (options->_finish_processing) (options->data, fostream);
            if (g_output_stream_close (fostream, NULL, NULL)!=TRUE) {
                fprintf (stderr, "Could not close output file stream!\n");
            }    

            if (compressor!=NULL)
                g_object_unref (compressor);
            if (ostream!=NULL)
                g_object_unref (ostream);
            if (fostream!=NULL)
                g_object_unref (fostream);
        } else {

            GOutputStream *ostream = (GOutputStream *) g_file_replace (
                output_file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL);

            (options->_finish_processing) (options->data, ostream);
            if (g_output_stream_close (ostream, NULL, NULL)!=TRUE) {
                fprintf (stderr, "Could not close output file stream!\n");
            }    

            if (ostream!=NULL)
                g_object_unref (ostream);
        }

    }

    g_object_unref (output_file); 
    /* Clean up program options/counts data structures. */
    if (options->_free != NULL)
        (options->_free) (options->data);

    free (options);
//
//    if (num_errors > 0) {
//        printf ("Finished with %d errors.\n", num_errors);
//    }

    return 0;
}
