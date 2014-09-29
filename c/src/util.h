#include <stdio.h>

#define CU_NEWSTRCPY(cpy, src) char *cpy = NULL; \
    asprintf (&cpy, "%s", src);

