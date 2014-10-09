#include <stdio.h>
#include <gio/gio.h>


#define CU_NEWSTRCPY(cpy, src) char *cpy = NULL; \
    asprintf (&cpy, "%s", src);

