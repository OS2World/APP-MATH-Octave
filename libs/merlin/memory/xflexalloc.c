/* xflexalloc.c */
/* Klaus Gebhardt, 1996 */

#define MERLIN_MEMORY
#include "../merlin.h"


void *merlin_flex_alloc (size_t size)
{
  return (void *) malloc (size);
}

void *merlin_flex_realloc (void *ptr, size_t size)
{
  return (void *) realloc ((char *) ptr, size);
}

void merlin_flex_free (void *ptr)
{
  free (ptr);
}
