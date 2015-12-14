#include "ctype.h"

int tolower(int c)
{
    return !isalpha(c) || islower(c) ? c : c + 0x20;
}
