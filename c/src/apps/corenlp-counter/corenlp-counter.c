#include "apps/corenlp-counter/corenlp-counter.h"



int process_path(
    opt_s *options, 
    gchar *path, 
    gboolean recurse, 
    GThreadPool* pool,
    gboolean verbose);

int process_dir(
    opt_s *options,
    gchar *dir,
    gboolean recurse,
    GThreadPool *pool,
    gboolean verbose
);

int process_file(
    opt_s *options,
    gchar *path,
    gboolean verbose
);


void threaded_process_file(gpointer data, gpointer user_data)
{
    char *path = (char *) data;
    printf ("Threaded file process: %s\n", path);
}

void usage(){
    printf("Usage: corenlp-counter -vr -o <OUTFILE> <PATH1 PATH2...>\n");
}


int
process_path(
    opt_s *options,
    gchar *path,
    gboolean recurse,
    GThreadPool* pool,
    gboolean verbose
)
{
    int num_errors = 0;


    if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
        if (pool == NULL) {
            //printf ("Processing single-threaded: %s\n", path);
            CU_NEWSTRCPY(path_cpy, path);
            num_errors += process_file (options, path_cpy, verbose);
        } else {
            printf ("Processing multi-threaded: %s\n", path);
            //g_thread_pool_push (pool, path, NULL);
        }

    } else if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
        if (recurse) {
            if (verbose)
                printf ("Descending into dir: %s\n", path);
            CU_NEWSTRCPY(path_cpy, path)
            num_errors += process_dir (
                options, path_cpy, recurse, pool, verbose); 
        } else if (verbose) {
            printf ("Skipping dir: %s\n", path);
        }

    } else {
        g_print ("SKipping %s -- not a file/dir.\n", path);
    }

    free (path);
    return num_errors;
}

int
process_dir(
    opt_s *options,
    gchar *dir,
    gboolean recurse,
    GThreadPool *pool,
    gboolean verbose
)
{

    int num_errors = 0;
    GError* err = NULL;
    GDir *dir_p = NULL;
    dir_p = g_dir_open (dir, 0, &err);

    if (err != NULL) {
        fprintf (stderr, "Unable to read directory: %s\n", err->message);
        g_error_free(err);
        num_errors++;
    } else if (dir == NULL) {
        fprintf (stderr, "Unable to read directory: %s\n", dir);
        num_errors++;
    } else {
        const gchar *filename = NULL;
        gchar *path = NULL;
        while ((filename=g_dir_read_name (dir_p)) != NULL) {
            path = g_build_filename (dir, filename, NULL);
            num_errors += process_path (options, path, recurse, pool, verbose);
        }
        g_dir_close (dir_p);
    }
    
    free (dir);
    return num_errors;

}


int
process_file(
    opt_s *options,
    gchar *path,
    gboolean verbose
)
{
    if (verbose) {
        printf ("Processing file %s\n", path);
    }
    gboolean is_uncertain = FALSE;
    

    FILE *fp = NULL;
    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf (stderr, "Could not open file: %s\n", path);
        return 1;
    }
    gchar header[8];
    int read = fread(header, 1, 8, fp);
    if (read <= 0) {
        fprintf (stderr, "Could not open file: %s\n", path);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    char *content_type =
        g_content_type_guess (path, header, read, &is_uncertain);

    if (content_type != NULL) {

        char *mime_type = g_content_type_get_mime_type (content_type);
           printf ("Content type of %s\n", mime_type);

        if (strcmp(mime_type, "application/xml")==0) {
            if (options->_xml_file_handler != NULL) {
                CU_NEWSTRCPY(path_cpy, path)
                (options->_xml_file_handler) (path_cpy, options->data);
            }
        } else if (strcmp(mime_type, "application/x-gzip")==0) {
            if (options->_x_gzip_file_handler != NULL) {
 
                CU_NEWSTRCPY(path_cpy, path)
                (options->_x_gzip_file_handler) (path_cpy, options->data);
            }
        } else if (strcmp(mime_type, "application/x-compressed-tar")==0) {
            if (options->_x_gzip_file_handler != NULL) {
                CU_NEWSTRCPY(path_cpy, path)
                (options->_x_gzip_file_handler) (path_cpy, options->data);
            }
        } else {

            fprintf (stderr, "Could not figure out filetype\n");
        }

        
        g_free (mime_type);
        g_free (content_type);
        free (path);
        return 0;

    } else {
        fprintf (stderr, "File has no content type: %s\n", path);
        free (path);
        return 1;
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
                if (++i < argc) 
                    if (strcmp(argv[i],"chambers")==0) {
                        mode = CHAMBERS;
                    } else if (strcmp(argv[i], "chambers-dir")==0) {
                        mode = CHAMBERS_DIR;
                    } else if (strcmp(argv[i],"embedding")==0) {
                        mode = EMBEDDING;
                    } else if (strcmp(argv[i], "embedding-dir")==0) {
                        mode = EMBEDDING_DIR;
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
    if (output_file==NULL)
        output_file = g_file_new_for_path ("nc-counts.gz");
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

    } else if (mode==CHAMBERS_DIR) {
        if (verbose==TRUE)
            printf ("Generating counts for Chambers directional model.\n");

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
    }


    /* Clean up processed file lists */
    g_ptr_array_set_free_func (valid_files, (GDestroyNotify) g_object_unref);
    g_ptr_array_free (valid_files, TRUE);
    g_object_unref (output_file); 

//    /* Validate output file path */
//    if (output_file==NULL)
//        output_file = "counts.gz";
//
//    opt_s *options = NULL;
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
//    if (options->_finish_processing != NULL)
//        (options->_finish_processing) (options->data);
//    
//    if (options->_free != NULL)
//        (options->_free) (options->data);
//
//    //free (options->data);
//    free (options);
//
//    if (num_errors > 0) {
//        printf ("Finished with %d errors.\n", num_errors);
//    }

    return 0;
}
