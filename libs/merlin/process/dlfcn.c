/*
** ****************************************************************************
** Loading/Unloading of DLLs for OS/2
** (c) 1997, Klaus Gebhardt
** ****************************************************************************
*/

/*
** ****************************************************************************
** This was written for the OS/2 port of Octave, but it is not part of Octave!
** You can use the code UNMODIFIED. If you think changes are necessary,
** please send me a mail (gebhardt@world4.net).
** Thanks,
**   Klaus Gebhardt
** ****************************************************************************
*/


#define INCL_DOSMODULEMGR
#include <os2.h>

#define MERLIN_PROCESS
#include "../merlin.h"

#include <stdio.h>
#include <stdlib.h>


static UCHAR  errbuf[2*BUFSIZ];
static APIRET errvalid;


void *merlin_dlopen (const char *path, int mode)
{
  HMODULE hmod;
  UCHAR   dll[BUFSIZ];
  UCHAR   bad[BUFSIZ];

  const char *c;
  char *d;

  c = path;  d = dll;
  while (*c)
    {
      if ((*c) == '/')  (*d) = '\\';
      else              (*d) = (*c);
      c++;  d++;
    }
  (*d) = (*c);

  errvalid = DosLoadModule (bad, BUFSIZ - 1, dll, &hmod);

  if (errvalid)
    {
      sprintf (errbuf, "DosLoadModule failed (rc = %lu):\n%s\nbad: %s",
	       errvalid, dll, bad);
      return NULL;
    }

  return (void *) hmod;
}


void *merlin_dlsym (void *handle, const char *symbol)
{
  HMODULE hmod;
  void    *function;

  hmod = (HMODULE) handle;

  errvalid = DosQueryProcAddr (hmod, 0, symbol, (PFN *) &function);

  if (errvalid)
    {
      sprintf (errbuf, "DosQueryProcAddr failed (rc = %lu):\n"
	       "Could not find address for %s", errvalid, symbol);
      return NULL;
    }

  return function;
}


char *merlin_dlerror (void)
{
  if (errvalid)
    {
      errvalid = 0;
      return errbuf;
    }
  return NULL;
}


int merlin_dlclose (void *handle)
{
  HMODULE hmod;

  hmod = (HMODULE) handle;

  errvalid = DosFreeModule (hmod);

  if (errvalid)
    {
      sprintf (errbuf, "DosFreeModule failed (rc = %lu)", errvalid);
      return -1;
    }

  return 0;
}
