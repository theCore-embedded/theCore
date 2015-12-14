#include "ctype.h"

int isupper(int c)
{
    return !islower(c);
}
