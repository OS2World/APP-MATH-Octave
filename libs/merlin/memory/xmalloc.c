/* xmalloc.c -- safe versions of malloc and realloc */
/* Klaus Gebhardt, 1996 */

#define MERLIN_MEMORY
#include "../merlin.h"


extern void merlin_error_and_abort (char *f);


void *merlin_xmalloc (size_t bytes)
{
  void *temp = (void *)malloc (bytes);
  if (!temp)  merlin_error_and_abort ("xmalloc");
  return (temp);
}

void *merlin_xrealloc (void *pointer, size_t bytes)
{
  void *temp;
  if (!pointer)  temp = (void *)malloc (bytes);
  else           temp = (void *)realloc (pointer, bytes);
  if (!temp)     merlin_error_and_abort ("xrealloc");
  return (temp);
}
