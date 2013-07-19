/*
** Testing the dlfcn library.
** (c) 1997, Klaus Gebhardt
*/

#include <stdlib.h>
#include <stdio.h>

#define MERLIN_PROCESS
#include "merlin.h"

int main (int argc, char *argv[])
{
  void *hmod;

  if (argc == 2)
    {
      printf ("\nLoading DLL: %s ...\n", argv[1]);
      hmod = dlopen (argv[1], RTLD_LAZY);
      if (hmod)
	{
	  printf ("%s loaded.\n", argv[1]);
	  dlclose (hmod);
	  return 0;
	}
      else
	{
	  printf ("%s not loaded.\n\n%s\n", argv[1], dlerror ());
	  return -1;
	}
    }
  else
    {
      printf ("\nusage: dlltest dll-filename\n");
      return -1;
    }
}
