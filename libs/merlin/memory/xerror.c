/* xerror.c */
/* Klaus Gebhardt, 2001 */

#define MERLIN_MEMORY
#include "../merlin.h"

#include <process.h>

void merlin_error_and_abort (char *fname)
{
  fprintf (stderr, "%s: Virtual memory exhausted!\n", fname);
  abort ();
}
