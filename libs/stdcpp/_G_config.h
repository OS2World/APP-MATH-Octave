/* AUTOMATICALLY GENERATED; DO NOT EDIT! */ 
#ifndef _G_config_h
#define _G_config_h
#define _G_NAMES_HAVE_UNDERSCORE 1
#define _G_VTABLE_LABEL_HAS_LENGTH 1
#define _G_VTABLE_LABEL_PREFIX "__vt$"
#define _G_HAVE_ST_BLKSIZE 0
typedef long _G_clock_t;
typedef int _G_dev_t;
typedef long _G_fpos_t;
typedef int _G_gid_t;
typedef int _G_ino_t;
typedef int _G_mode_t;
typedef int _G_nlink_t;
typedef long _G_off_t;
typedef int _G_pid_t;
#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ long
#endif
typedef __PTRDIFF_TYPE__ _G_ptrdiff_t;
typedef unsigned long _G_sigset_t;
#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
typedef __SIZE_TYPE__ _G_size_t;
typedef unsigned long _G_time_t;
typedef int _G_uid_t;
typedef short unsigned int _G_wchar_t;
typedef int _G_ssize_t;
typedef int /* default */ _G_wint_t;
typedef char * _G_va_list;
#define _G_signal_return_type void
#define _G_sprintf_return_type int

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
typedef          int   _G_int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int  _G_uint8_t __attribute__((__mode__(__QI__)));
typedef          int  _G_int16_t __attribute__((__mode__(__HI__)));
typedef unsigned int _G_uint16_t __attribute__((__mode__(__HI__)));
typedef          int  _G_int32_t __attribute__((__mode__(__SI__)));
typedef unsigned int _G_uint32_t __attribute__((__mode__(__SI__)));
typedef          int  _G_int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int _G_uint64_t __attribute__((__mode__(__DI__)));
#else
typedef  short /* deduced */  _G_int16_t;
typedef unsigned short /* deduced */ _G_uint16_t;
typedef  long /* deduced */  _G_int32_t;
typedef unsigned long /* deduced */ _G_uint32_t;
#endif

#define _G_BUFSIZ 5120
#define _G_FOPEN_MAX 14
#define _G_FILENAME_MAX 260
#define _G_NULL 0 /* default */
#if defined (__cplusplus) || defined (__STDC__)
#define _G_ARGS(ARGLIST) ARGLIST
#else
#define _G_ARGS(ARGLIST) ()
#endif
#if !defined (__GNUG__) || defined (__STRICT_ANSI__)
#define _G_NO_NRV
#endif
#if !defined (__GNUG__)
#define _G_NO_EXTERN_TEMPLATES
#endif
#define _G_HAVE_ATEXIT 1
#define _G_HAVE_SYS_RESOURCE 0
#define _G_HAVE_SYS_TIMES 1
#define _G_HAVE_SYS_SOCKET 1
#define _G_HAVE_SYS_CDEFS 0     /* Don't require bsddev.zip */
#define _G_HAVE_SYS_WAIT 1
#define _G_HAVE_UNISTD 1
#define _G_HAVE_DIRENT 1
#define _G_HAVE_CURSES 1
#define _G_MATH_H_INLINES 0
#define _G_HAVE_BOOL 1
#endif /* !_G_config_h */