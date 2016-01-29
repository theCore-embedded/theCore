#include "ctype.h"

/* Rely upon actual function */
extern int islower(int c);
extern int isalpha(int c);

int  LIBC_FUNCTION(tolower) (int c)
{
    return !isalpha(c) || islower(c) ? c : c + 0x20;
}
