#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef CU_UTIL_H
#define CU_UTIL_H
#include <stdio.h>
#include <gio/gio.h>

GPtrArray *cu_slurp_dir_contents(GFile *);
void cu_walk_dir(GFile *, 
                 void *, 
                 void (*) (GFile *, void *),
                 void (*) (GFile *, void *));

#define CU_NEWSTRCPY(cpy, src) char *cpy = NULL; \
    if (asprintf (&cpy, "%s", src)==-1) { \
        fprintf (stderr, "Could not allocate space for hash key!\n"); \
        exit (EXIT_FAILURE); \
    }

#endif
