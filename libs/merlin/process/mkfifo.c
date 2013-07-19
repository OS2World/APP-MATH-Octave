/*
** ****************************************************************************
** This implements the mkfifo command under OS/2.
** (c) Klaus Gebhardt, 1997
** ****************************************************************************
*/

/*
** ****************************************************************************
** This was written for the OS/2 port of Octave, but it is not part of Octave!
** You can use the code UNMODIFIED. If you think changes are necessary,
** please send me a mail (gebhardt@crunch.ikp.physik.th-darmstadt.de).
** Thanks,
**   Klaus Gebhardt
** ****************************************************************************
*/

#include <process.h>
#include <stdio.h>

#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#include <os2.h>

#define MERLIN_PROCESS
#include "../merlin.h"

#include <string.h>

int merlin_mkfifo (const char *pathname, mode_t mode)
{
  APIRET rc;
  HPIPE  hpipe;
  ULONG  action;
  ULONG  count;

  int mkfifo_cmd = MERLIN_MKFIFO_CREATE_NMP;
  int result = 0;
  int pid = -1;

  do
    {
      rc = DosWaitNPipe (MERLIN_MKFIFO_NAMED_PIPE, 0xFF);

      if ((rc == 3) && (pid == -1))
	{
	  pid = spawnlp (P_DETACH, "nmpserv", "nmpserv", NULL);
	  if (pid == -1)  return -1;
	}
      else if (rc == 0)
	{
	  rc = DosOpen (MERLIN_MKFIFO_NAMED_PIPE, &hpipe, &action, 0, 0,
			OPEN_ACTION_OPEN_IF_EXISTS,
			OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_FAIL_ON_ERROR |
			OPEN_FLAGS_RANDOM | OPEN_SHARE_DENYNONE |
			OPEN_ACCESS_READWRITE, 0);
	}
    }
  while (rc);

  rc = DosWrite (hpipe, &mkfifo_cmd, sizeof (mkfifo_cmd), &count);
  if (rc || (count != sizeof (mkfifo_cmd)))  goto exit;

  DosWrite (hpipe, &mode, sizeof (mode), &count);
  if (rc || (count != sizeof (mode)))  goto exit;

  DosWrite (hpipe, pathname, strlen (pathname) + 1, &count);
  if (rc || (count != strlen (pathname) + 1))  goto exit;

  rc = DosRead (hpipe, &result, sizeof (result), &count);
  if (rc || (count != sizeof (result)))  goto exit;

exit:
  DosClose (hpipe);
  return result;
}


int merlin_mkfifo_remove (const char *pathname)
{
  APIRET rc;
  HPIPE  hpipe;
  ULONG  action;
  ULONG  count;

  int mkfifo_cmd = MERLIN_MKFIFO_REMOVE_NMP;
  int result = 0;

  rc = DosOpen (MERLIN_MKFIFO_NAMED_PIPE, &hpipe, &action, 0, 0,
		OPEN_ACTION_OPEN_IF_EXISTS,
		OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_FAIL_ON_ERROR |
		OPEN_FLAGS_RANDOM | OPEN_SHARE_DENYNONE |
		OPEN_ACCESS_READWRITE, 0);

  if (rc)  return 0;

  rc = DosWrite (hpipe, &mkfifo_cmd, sizeof (mkfifo_cmd), &count);
  if (rc || (count != sizeof (mkfifo_cmd)))  goto exit;

  DosWrite (hpipe, pathname, strlen (pathname) + 1, &count);
  if (rc || (count != strlen (pathname) + 1))  goto exit;

  rc = DosRead (hpipe, &result, sizeof (result), &count);
  if (rc || (count != sizeof (result)))  goto exit;

exit:
  DosClose (hpipe);
  return result;
}


int merlin_mkfifo_list ()
{
  APIRET rc;
  HPIPE  hpipe;
  ULONG  action;
  ULONG  count;

  int mkfifo_cmd = MERLIN_MKFIFO_LIST_NMPS;
  int i, n;
  char name[1024];

  rc = DosOpen (MERLIN_MKFIFO_NAMED_PIPE, &hpipe, &action, 0, 0,
		OPEN_ACTION_OPEN_IF_EXISTS,
		OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_FAIL_ON_ERROR |
		OPEN_FLAGS_RANDOM | OPEN_SHARE_DENYNONE |
		OPEN_ACCESS_READWRITE, 0);

  if (rc)  return 0;

  rc = DosWrite (hpipe, &mkfifo_cmd, sizeof (mkfifo_cmd), &count);
  if (rc || (count != sizeof (mkfifo_cmd)))  goto exit;

  rc = DosRead (hpipe, &n, sizeof (n), &count);
  if (rc || (count != sizeof (n)))  goto exit;

  for (i=0; i<n; i++)
    {
      rc = DosWrite (hpipe, &i, sizeof (i), &count);
      if (rc || (count != sizeof (i)))  goto exit;

      rc = DosRead (hpipe, name, sizeof (name), &count);
      if (rc)  goto exit;

      printf ("%3i: %s\n", i, name);
    }

  DosClose (hpipe);
  return 0;

exit:
  DosClose (hpipe);
  return -1;
}


int merlin_mkfifo_exit ()
{
  APIRET rc;
  HPIPE  hpipe;
  ULONG  action;
  ULONG  count;

  int mkfifo_cmd = MERLIN_MKFIFO_EXIT_SERVER;
  int n;

  rc = DosOpen (MERLIN_MKFIFO_NAMED_PIPE, &hpipe, &action, 0, 0,
		OPEN_ACTION_OPEN_IF_EXISTS,
		OPEN_FLAGS_WRITE_THROUGH | OPEN_FLAGS_FAIL_ON_ERROR |
		OPEN_FLAGS_RANDOM | OPEN_SHARE_DENYNONE |
		OPEN_ACCESS_READWRITE, 0);

  if (rc)  return 0;

  rc = DosWrite (hpipe, &mkfifo_cmd, sizeof (mkfifo_cmd), &count);
  if (rc || (count != sizeof (mkfifo_cmd)))  goto exit;

  rc = DosRead (hpipe, &n, sizeof (n), &count);
  if (rc || (count != sizeof (n)))  goto exit;

  if (n > 1)
    fprintf (stderr, "\nwarning: %i named pipes destroyed.", n);
  else if (n == 1)
    fprintf (stderr, "\nwarning: %i named pipe destroyed.", n);

  DosClose (hpipe);
  return 0;

exit:
  DosClose (hpipe);
  return -1;
}
