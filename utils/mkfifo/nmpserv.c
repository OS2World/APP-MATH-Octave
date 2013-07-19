/*
** ****************************************************************************
** This is the server the mkfifo command connects to. This program creates
** a named pipe which is used for communication between the mkfifo routine
** and the server. The mkfifo sends the name of the named pipe, which should
** be created and then receives the return code.
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

#include "nmp.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MERLIN_PROCESS
#include <merlin.h>


/* List of all named pipes */
int _named_pipes_ = 0;
_named_pipe_ *_nmp_list_ = NULL;


/* For accessing the pipe. */
static APIRET apiret;
static HPIPE  hpipe;
static ULONG  count, dummy = 0;


/* Name and access mode of the pipe */
static char name[1024];
static mode_t mode;
static int result;


/*
** Add a named pipe to the list of the already existing named pipes and
** a thread for it. Return 0 on success, -1 on error out of memory, -2 unable
** to start the thread.
*/
static int add_to_nmp_list ()
{
  void *next;
  void *prev;

  _named_pipe_ *nmp;

  nmp = (_named_pipe_ *) malloc (sizeof(_named_pipe_));
  if (!nmp)  return -1;

  nmp->name = strdup (name);
  if (!nmp->name)  return -1;

  nmp->mode = mode;

  /*
  ** Start a new thread to move the data from one end of the pipe
  ** to the other.
  */

  DosResetEventSem (_hev_, &dummy);
  nmp->tid = _beginthread (nmp_thread, NULL, 65536, (void *) nmp);
  DosWaitEventSem (_hev_, SEM_INDEFINITE_WAIT);

  if ((nmp->tid == -1) || (nmp->result == -1))
    {
      free ((char *) nmp->name);
      free (nmp);
      return -1;
    }

  prev = NULL;
  next = (void *) _nmp_list_;

  while (next != NULL)
    {
      prev = next;
      next = ((_named_pipe_ *) next)->next;
    }

  nmp->prev = prev;
  nmp->next = NULL;

  if (prev)  ((_named_pipe_ *) prev)->next = nmp;
  else       _nmp_list_ = nmp;

  _named_pipes_++;

  return 0;
}


/*
** Search for the named pipe, which name is given in the argument, and 
** return a pointer to its named pipe structure. Return NULL on error.
*/
static _named_pipe_ *lookup_nmp_list ()
{
  void *next;
  void *prev;

  prev = NULL;
  next = (void *) _nmp_list_;

  while (next != NULL)
    {
      if (!stricmp (((_named_pipe_ *) next)->name, name))
	return ((_named_pipe_ *) next);

      prev = next;
      next = ((_named_pipe_ *) next)->next;
    }

  return ((_named_pipe_ *) NULL);
}


/* Remove a named pipe from the list. */
static int remove_nmp_from_list ()
{
  void *next;
  void *prev;

  _named_pipe_ *nmp;

  nmp = lookup_nmp_list ();

  if (nmp)
    {
      prev = nmp->prev;
      next = nmp->next;

      if (prev)  ((_named_pipe_ *) prev)->next = next;
      else       _nmp_list_ = next;

      if (next)  ((_named_pipe_ *) next)->prev = prev;

      nmp->tid = -1;

      _named_pipes_--;

      free (nmp);

      return 0;
    }

  return -1;
}


/* Replace all '/' in name with '\\' */
static void os2_path ()
{
  char *c;
  c = name;

  while (*c)
    {
      if ((*c) == '/')  (*c) = '\\';
      c++;
    }
}


/* Create a new named pipe. */
static int create_nmp ()
{
  int rc;

  apiret = DosRead (hpipe, &mode, sizeof(mode), &count);
  if (apiret || (count != sizeof(mode)))  return -1;

  apiret = DosRead (hpipe, name, sizeof(name), &count);
  if (apiret)  return -1;

  os2_path ();

  if (lookup_nmp_list ())  rc = -1;
  else                     rc = add_to_nmp_list ();

  apiret = DosWrite (hpipe, &rc, sizeof(rc), &count);
  if (apiret || (count != sizeof(rc)))  return -1;

  return 0;
}


/* Remove one named pipe. */
static int remove_nmp ()
{
  int rc;

  apiret = DosRead (hpipe, name, sizeof(name), &count);
  if (apiret)  return -1;

  os2_path ();

  rc = remove_nmp_from_list ();

  apiret = DosWrite (hpipe, &rc, sizeof(rc), &count);
  if (apiret || (count != sizeof(rc)))  return -1;

  return 0;
}


/* Send a list of all active named pipes to the client. */
static int list_nmps ()
{
  _named_pipe_ *nmp;
  int i, j;

  apiret = DosWrite (hpipe, &_named_pipes_, sizeof(_named_pipes_), &count);
  if (apiret || (count != sizeof(_named_pipes_)))  return -1;

  nmp = _nmp_list_;

  for (i=0; i<_named_pipes_; i++)
    {
      apiret = DosRead (hpipe, &j, sizeof(j), &count);
      if (apiret || (count != sizeof(j)) || (i != j))  return -1;

      apiret = DosWrite (hpipe, nmp->name, strlen (nmp->name) + 1, &count);
      if (apiret || (count != strlen (nmp->name) + 1))  return -1;

      nmp = (_named_pipe_ *) nmp->next;
    }

  return 0;
}


/* Tell how many named pipes are closed, before exiting. */
static int close_server ()
{
  apiret = DosWrite (hpipe, &_named_pipes_, sizeof(_named_pipes_), &count);
  if (apiret || (count != sizeof(_named_pipes_)))  return -1;

  return 0;
}


/* Unknown command is sent to the server. */
static int unknown_cmd ()
{
  int error = -1;

  apiret = DosWrite (hpipe, &error, sizeof(error), &count);
  if (apiret || (count != sizeof(error)))  return -1;

  return 0;
}


void APIENTRY cleanup_on_exit (ULONG ulTermCode)
{
  _named_pipe_ *nmp;

  nmp = _nmp_list_;

  while (_named_pipes_ > 0)
    {
      strcpy (name, nmp->name);

      os2_path ();
      remove_nmp_from_list ();

      nmp = (_named_pipe_ *) nmp->next;
    }

  DosExitList (EXLST_EXIT, 0);
}


/* The main program for the server. */
int main (int argc, char *argv[])
{
  int mkfifo_cmd;
  int i, noexit = 0;

  if (argc > 2)  exit (-1);

  if (argc == 2)
    {
      if (stricmp ("--noexit", argv[1]))  exit (-1);
      noexit = 1;
    }

  DosExitList (EXLST_ADD, cleanup_on_exit);

  DosCreateEventSem (NULL, &_hev_, 0L, (BOOL32) 0);

  /*
  ** Create the named pipe for communication between mkfifo and the server.
  ** This must be a two-way message pipe.
  */

  apiret = DosCreateNPipe (MERLIN_MKFIFO_NAMED_PIPE, &hpipe, NP_ACCESS_DUPLEX,
			   NP_WAIT | NP_TYPE_MESSAGE | NP_READMODE_MESSAGE |
			   0x01, 1024L, 1024L, -1);

  if (apiret == 231)  return 1;
  if (apiret != 0)    return -1;

  do
    {
      if (DosConnectNPipe (hpipe))  goto error;

      apiret = DosRead (hpipe, &mkfifo_cmd, sizeof(mkfifo_cmd), &count);
      if (apiret || (count != sizeof(mkfifo_cmd)))  goto error;

      switch (mkfifo_cmd)
	{
	case MERLIN_MKFIFO_CREATE_NMP:
	  result = create_nmp ();
	  break;

	case MERLIN_MKFIFO_REMOVE_NMP:
	  result = remove_nmp ();
	  break;

	case MERLIN_MKFIFO_LIST_NMPS:
	  result = list_nmps ();
	  break;

	case MERLIN_MKFIFO_EXIT_SERVER:
	  result = close_server ();
	  break;

	default:
	  result = unknown_cmd ();
	  break;
	}

      if (DosDisConnectNPipe (hpipe))  goto error;
    }
  while ((mkfifo_cmd != MERLIN_MKFIFO_EXIT_SERVER) &&
	 ((_named_pipes_ > 0) || noexit));

  DosClose (hpipe);
  return 0;

error:
  DosDisConnectNPipe (hpipe);
  DosClose (hpipe);
  return result;
}
