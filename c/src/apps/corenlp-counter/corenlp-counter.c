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
    printf("Usage: corenlp-counter -vr <PATH>\n");
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
        g_content_type_guess (path, NULL, 0, &is_uncertain);

    if (content_type != NULL) {

        char *mime_type = g_content_type_get_mime_type (content_type);

        if (strcmp(mime_type, "application/xml")==0) {
            if (options->_xml_file_handler != NULL) {
                CU_NEWSTRCPY(path_cpy, path)
                (options->_xml_file_handler) (path_cpy, options->data);
            }
//        } else if (strcmp(mime_type, "application/x-gzip")==0) {
//            //if (options->_x_gzip_file_handler != NULL) {
//            //    (options->_x_gzip_file_handler) (path, options->data);
//            //}
//        } else if (strcmp(mime_type, "application/x-compressed-tar")==0) {
//            //if (options->_x_gzip_file_handler != NULL) {
//            //    (options->_x_gzip_file_handler) (path, options->data);
//            //}
        }
//
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


int main(int argc, char **argv) {

    GPtrArray *pathlist;
    gboolean recurse = FALSE;
    gboolean verbose = FALSE;

    //cnt_mode_t mode = STATS;

    if (argc == 1) {

        usage();
        exit(0);

    } else {
        pathlist = g_ptr_array_new();
        for (int i=1; i < argc; i++) {
            if (strcmp(argv[i], "-r")==0) {
                recurse = TRUE;
            } else if (strcmp(argv[i], "-v")==0) {
                verbose = TRUE;
            } else {
                g_ptr_array_add (pathlist, (gpointer) argv[i]);
            }
        }
    }

    
    opt_s * options = nc_options_new();
//    if (mode == STATS) {
//        g_print("Making struct\n");
//        options->data = new_stats_data();
//        options->_xml_file_handler = stats_xml_file_handler;
//        options->_x_gzip_file_handler = stats_x_gzip_file_handler;
//        options->_finish_processing = stats_finish_processing;
//        options->_free = stats_free;
//    }

    const char *path = NULL;
    int num_errors = 0;
    for (int i=0; i < pathlist->len; i++) {
        path = (char *) g_ptr_array_index (pathlist, i);
        CU_NEWSTRCPY(path_cpy, path)
        num_errors += 
            process_path (options, (gchar *) path_cpy, recurse, NULL, verbose);        

        //g_thread_pool_push (pool, path, NULL);
        //num_errors += process_path (options, path, recurse, pool);
    }
    g_ptr_array_free (pathlist, TRUE);

//    char *outpath = NULL;
    if (options->_finish_processing != NULL)
        (options->_finish_processing) (options->data);
    
    if (options->_free != NULL)
        (options->_free) (options->data);

    free (options->data);
    free (options);

    if (num_errors > 0) {
        printf ("Finished with %d errors.\n", num_errors);
    }

    return 0;
}
