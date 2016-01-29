/* Avoid collisions with libc by using relative paths */
#include "export/ctype.h"

/* Rely upon actual function */
extern int islower(int c);

int LIBC_FUNCTION(isupper) (int c)
{
    return !islower(c);
}
