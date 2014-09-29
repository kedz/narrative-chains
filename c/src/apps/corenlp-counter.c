#include <stdio.h>
#include <glib.h>
#include <string.h>

void threaded_process_file(gpointer data, gpointer user_data)
{
    char *path = (char *) data;
    printf ("Threaded file process: %s\n", path);
}

void usage(){
    printf("Usage: corenlp-counter -vr <PATH>\n");
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
            if (strcmp(argv[i], "-v")==0) {
                verbose = TRUE;
            } else {
                g_ptr_array_add (pathlist, (gpointer) argv[i]);
            }
        }
    }


    const char *path = NULL;
    int num_errors = 0;
    for (int i=0; i < pathlist->len; i++) {
        path = (char *) g_ptr_array_index (pathlist, i);
        if (verbose)
            printf ("Processing: %s\n", path);
        //g_thread_pool_push (pool, path, NULL);
        //num_errors += process_path (options, path, recurse, pool);
    }


    }

    return 0;
}
