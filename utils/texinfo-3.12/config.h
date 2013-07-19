#if defined (EMX09C)

#define HANDLE_MAN_PAGES
#define DEFAULT_INFOPATH ".;h:/data/info"
#define C_ALLOCA
#define DIRENT
#define HAVE_ALLOCA_H

/* Define if the setvbuf function takes the buffering type as its second
   argument and the buffer pointer as the third, as on System V
   before release 3.  */
#undef SETVBUF_REVERSED

/* Define if your <sys/time.h> declares struct tm.  */
#define TM_IN_SYS_TIME

/* Define if the closedir function returns void instead of int.  */
#undef VOID_CLOSEDIR

#define HAVE_SGTTY_H
#undef HAVE_TERMIOS_H
#define HAVE_TERMIO_H
#define HAVE_ATEXIT
#define HAVE_GETCWD
#undef HAVE_GETHOSTNAME
#undef HAVE_ISINF
#undef HAVE_ISNAN
#undef HAVE_ON_EXIT
#undef HAVE_RINDEX
#define HAVE_SETVBUF
#define HAVE_STRING_H
#undef HAVE_STRCASECMP
#undef HAVE_STRERROR
#define HAVE_STRICMP
#undef HAVE_STRNCASECMP
#define HAVE_STRNICMP
#define HAVE_STRCHR
#define HAVE_STRDUP
#define HAVE_VFPRINTF
#define HAVE_VSPRINTF
#define HAVE_VPRINTF
#define HAVE_SYS_FCNTL_H
#undef HAVE_SYS_PTEM_H
#undef HAVE_SYS_SELECT_H
#define HAVE_SYS_TIME_H
#undef HAVE_SYS_TTOLD_H
#define HAVE_SYS_UTSNAME_H
#define HAVE_UNISTD_H
#define HAVE_VARARGS_H

#undef HAVE_POSIX_SIGNALS
#undef HAVE_BSD_SIGNALS

#define PACKAGE       "texinfo"
#define VERSION       "3.12"

#define RETSIGTYPE void

#define _(String)  (String)
#define N_(String) (String)
#endif
