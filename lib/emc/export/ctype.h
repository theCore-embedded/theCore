#ifndef LIB_EMC_CTYPE_H_
#define LIB_EMC_CTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* To avoid collisions between hosted libc and existing implementation
 * names must be mangled */
#if EXTERNAL_LIBC
#define LIBC_FUNCTION(name) ecl_##name
#else
#define LIBC_FUNCTION(name) name
#endif

int LIBC_FUNCTION(isalnum) (int c);
int LIBC_FUNCTION(isblank) (int c);
int LIBC_FUNCTION(iscntrl) (int c);
int LIBC_FUNCTION(isdigit) (int c);
int LIBC_FUNCTION(isgraph) (int c);
int LIBC_FUNCTION(isprint) (int c);
int LIBC_FUNCTION(ispunct) (int c);
int LIBC_FUNCTION(isspace) (int c);
int LIBC_FUNCTION(isxdigit) (int c);
int LIBC_FUNCTION(tolower) (int c);
int LIBC_FUNCTION(toupper) (int c);
int LIBC_FUNCTION(isalpha) (int c);
int LIBC_FUNCTION(islower) (int c);
int LIBC_FUNCTION(isupper) (int c);

#ifdef __cplusplus
}
#endif


#endif // LIB_EMC_CTYPE_H_
