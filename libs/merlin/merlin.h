/* Merlin extender */
/* (c) Klaus Gebhardt, 2001 */


#if defined (__cplusplus)
extern "C" {
#endif


#if defined(MERLIN_MATH) && !defined(_MERLIN_MATH)
#define _MERLIN_MATH

#include <sys/cdefs.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#define MERLIN_GAMMA 0.57721566490153286061

#define _MERLIN_MATH_EPS       1.e-20
#define _MERLIN_MATH_BIG       1.e20
#define _MERLIN_MATH_MAX_ITER  200

#define merlin_max(a,b) (((a) > (b)) ? (a) : (b))
#define merlin_min(a,b) (((a) < (b)) ? (a) : (b))

#define isinf(x)   (!isfinite(x) && !isnan(x))
#define finite(x)  isfinite(x)

void merlin_dorand48 (unsigned short[3]);
void merlin_srand48 (long);
double merlin_erand48 (unsigned short[3]);
double merlin_drand48 (void);

extern unsigned short merlin_rand48_seed[3];
extern unsigned short merlin_rand48_mult[3];
extern unsigned short merlin_rand48_add;

#define	_MERLIN_MATH_RAND48_SEED_0 (0x330e)
#define	_MERLIN_MATH_RAND48_SEED_1 (0xabcd)
#define	_MERLIN_MATH_RAND48_SEED_2 (0x1234)
#define	_MERLIN_MATH_RAND48_MULT_0 (0xe66d)
#define	_MERLIN_MATH_RAND48_MULT_1 (0xdeec)
#define	_MERLIN_MATH_RAND48_MULT_2 (0x0005)
#define	_MERLIN_MATH_RAND48_ADD    (0x000b)

#define __dorand48 merlin_dorand48
#define srand48 merlin_srand48
#define erand48 merlin_erand48
#define drand48 merlin_drand48

#define __rand48_seed merlin_rand48_seed
#define __rand48_mult merlin_rand48_mult
#define __rand48_add  merlin_rand48_add

double merlin_acosh (double);
double merlin_asinh (double);
double merlin_atanh (double);
double merlin_erf (double);
double merlin_erfc (double);
double merlin_j0 (double);
double merlin_y0 (double);
double merlin_j1 (double);
double merlin_y1 (double);
double merlin_jn (int, double);
double merlin_yn (int, double);
double merlin_gamma (double);
double merlin_lgamma (double);
double merlin_logb (double);

#define acosh  merlin_acosh
#define asinh  merlin_asinh
#define atanh  merlin_atanh
#define erf    merlin_erf
#define erfc   merlin_erfc
#define j0     merlin_j0
#define y0     merlin_y0
#define j1     merlin_j1
#define y1     merlin_y1
#define jn     merlin_jn
#define yn     merlin_yn
#define gamma  merlin_gamma
#define lgamma merlin_lgamma
#define logb   merlin_logb
#endif


#if defined(MERLIN_MEMORY) && !defined(_MERLIN_MEMORY)
#define _MERLIN_MEMORY

#include <stdio.h>
#include <malloc.h>

void *merlin_xmalloc (size_t);
void *merlin_xrealloc (void *, size_t);

void *merlin_flex_alloc (size_t);
void *merlin_flex_realloc (void *, size_t);
void merlin_flex_free (void *);

char *merlin_xstrdup (char *);

#define xmalloc  merlin_xmalloc
#define xreallox merlin_xrealloc

#define yy_flex_alloc   merlin_flex_alloc
#define yy_flex_realloc merlin_flex_realloc
#define yy_flex_free    merlin_flex_free

#define xstrdup merlin_xstrdup
#endif


#if defined(MERLIN_PROCESS) && !defined(_MERLIN_PROCESS)
#define _MERLIN_PROCESS

#include <sys/types.h>
#include <sys/stat.h>

/* Mode flags for the dlopen routine. */
#define MERLIN_RTLD_LAZY       1       /* lazy function call binding */
#define MERLIN_RTLD_NOW        2       /* immediate function call binding */
#define MERLIN_RTLD_GLOBAL     0x100   /* allow symbols to be global */

struct merlin_dl_info
{
  void (*init)(void);
  void (*fini)(void);
};

void *merlin_dlopen (const char *, int);
void *merlin_dlsym (void *, const char *);
char *merlin_dlerror (void);
int merlin_dlclose (void *);

#define RTLD_LAZY   MERLIN_RTLD_LAZY
#define RTLD_NOW    MERLIN_RTLD_NOW
#define RTLD_GLOBAL MERLIN_RTLD_GLOBAL

#define dl_info merlin_dl_info

#define dlopen  merlin_dlopen
#define dlsym   merlin_dlsym
#define dlerror merlin_dlerror
#define dlclose merlin_dlclose

#define MERLIN_MKFIFO_NAMED_PIPE "\\PIPE\\MKFIFO\\SERVER\\CON"

#define MERLIN_MKFIFO_CREATE_NMP   0
#define MERLIN_MKFIFO_REMOVE_NMP   1
#define MERLIN_MKFIFO_LIST_NMPS    2
#define MERLIN_MKFIFO_EXIT_SERVER 11

int merlin_mkfifo (const char *, mode_t);
int merlin_mkfifo_remove (const char *);
int merlin_mkfifo_list ();
int merlin_mkfifo_exit ();

#define mkfifo        merlin_mkfifo
#define remove_mkfifo merlin_mkfifo_remove

#endif


#if defined(MERLIN_STD) && !defined(_MERLIN_STD)
#define _MERLIN_STD

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  void *next;
  long data_len;
} MERLIN_LIST_ENTRY_HEADER;

typedef struct
{
  void *first;   /* This must be the first member in the structure. */
  long entries;  /* This must be the second member in the structure. */

  void **array;

  void *filter;
  void *comparator;
} MERLIN_LIST;

MERLIN_LIST *merlin_list (int (*)(MERLIN_LIST *, void *, long),
		   int (*)(void *, long, void *, long));

long merlin_list_add_entry (MERLIN_LIST *, void *, long);
long merlin_list_iterate (MERLIN_LIST *, int (*)(void *, long));
long merlin_list_array (MERLIN_LIST *);
long merlin_list_index_of (MERLIN_LIST *, void *, long);

void *merlin_list_find (MERLIN_LIST *, void *, long);
void *merlin_list_element_at (MERLIN_LIST *, long);


MERLIN_LIST *merlin_properties ();
long merlin_properties_save (MERLIN_LIST *, FILE *);
long merlin_properties_load (MERLIN_LIST *, FILE *);

void merlin_property_set (MERLIN_LIST *, char *, char *);
char *merlin_properties_get (MERLIN_LIST *, char *, char *);
int merlin_properties_scan (MERLIN_LIST *, char *, char *, char *, ...);

#endif


#if defined(MERLIN_VIO) && !defined(_MERLIN_VIO)
#define _MERLIN_VIO

#define MERLIN_NOCURSOR     0
#define MERLIN_SOLIDCURSOR  1
#define MERLIN_NORMALCURSOR 2

void merlin_setctype (int);
#endif


#if defined (__cplusplus)
}
#endif
