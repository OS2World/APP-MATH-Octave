/* xstrdup.c -- safe version of strdup */
/* Klaus Gebhardt, 1996 */

#define MERLIN_MEMORY
#include "../merlin.h"

#include <string.h>

extern void merlin_error_and_abort (char *f);


char *merlin_xstrdup (char *s)
{
  char *temp = strdup (s);
  if (!temp)  merlin_error_and_abort ("xstrdup");
  return (temp);
}
