/* c-auto.h for emx-gcc 0.9d */
/* Made by hand, Klaus Gebhardt, 1995-1999 */

/* Define the version of the compiler used */
#define EMX09D

/* Define if compiling for OS2. */
#define OS2

/* Define if compiling for DOS. */
#undef DOS

/* Define if the closedir function returns void instead of int.  */
#undef CLOSEDIR_VOID

/* Define to empty if the keyword does not work.  */
#undef const

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS

/* Define if your putenv doesn't waste space when the same environment
   variable is assigned more than once, with different (malloced)
   values.  This is true only on NetBSD/FreeBSD, as far as I know. See
   xputenv.c.  */
#undef SMART_PUTENV

/* Define if you are using GNU libc or otherwise have global variables
   `program_invocation_name' and `program_invocation_short_name'.  */
#undef HAVE_PROGRAM_INVOCATION_NAME

/* all: Define to enable running scripts when missing input files.  */
#undef MAKE_TEX_MF_BY_DEFAULT
#undef MAKE_TEX_PK_BY_DEFAULT
#undef MAKE_TEX_TEX_BY_DEFAULT
#undef MAKE_TEX_TFM_BY_DEFAULT
#undef MAKE_OMEGA_OFM_BY_DEFAULT
#undef MAKE_OMEGA_OCP_BY_DEFAULT

/* Define if you have the basename function.  */
#undef HAVE_BASENAME

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY

/* Define if you have the getwd function.  */
#define HAVE_GETWD

/* Define if you have the putenv function.  */
#undef HAVE_PUTENV

/* Define if you have the strcasecmp function.  */
#undef HAVE_STRCASECMP

/* Define if you have the strstr function.  */
#define HAVE_STRSTR

/* Define if you have the strtol function.  */
#undef HAVE_STRTOL

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H

/* Define if you have the <float.h> header file.  */
#define HAVE_FLOAT_H

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H

/* Define if you have the <ndir.h> header file.  */
#undef HAVE_NDIR_H

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H

/* Define if you have the <sys/dir.h> header file.  */
#undef HAVE_SYS_DIR_H

/* Define if you have the <sys/ndir.h> header file.  */
#undef HAVE_SYS_NDIR_H

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H
