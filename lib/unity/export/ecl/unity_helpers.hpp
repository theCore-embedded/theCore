//! \file
//! \brief Set of unity helpers

#ifndef ECL_LIB_UNITY_HELPERS_
#define ECL_LIB_UNITY_HELPERS_

#include <unity.h>

//! Prints string using Unity's facilities and put end of line at the end.
//! \param[in] str NULL-terminated string to print.
static inline void UnityPrintWithEOL(const char *str)
{
    UnityPrint(str);
    UNITY_PRINT_EOL();
}


#endif // ECL_LIB_UNITY_HELPERS_
