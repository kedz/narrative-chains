#include <stdio.h>
#include <gio/gio.h>

GPtrArray *cu_slurp_dir_contents(GFile *);
void cu_walk_dir(GFile *, 
                 void *, 
                 void (*) (GFile *, void *),
                 void (*) (GFile *, void *));

#define CU_NEWSTRCPY(cpy, src) char *cpy = NULL; \
    asprintf (&cpy, "%s", src);

