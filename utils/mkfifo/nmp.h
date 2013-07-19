/*
** ****************************************************************************
** Here we define the structure for all the data needed to use a named pipe.
** (c) Klaus Gebhardt, 1997
** ****************************************************************************
*/

/*
** ****************************************************************************
** This is part of the server the mkfifo command connects to. This program
** creates a named pipe which is used for communication between the mkfifo
** routine and the server. The mkfifo sends the name of the named pipe,
** which should be created and then receives the return code.
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

#ifndef _NMP_H_
#define _NMP_H_

#include <sys/types.h>
#include <sys/stat.h>

#define INCL_DOSSEMAPHORES
#define INCL_DOSNMPIPES
#define INCL_DOSPROCESS
#include <os2.h>

HEV _hev_;

/* List of all named pipes */
typedef struct
{
  const char *name;  /* The name of the pipe. */
  mode_t mode;       /* Creation mode. */
  int tid;           /* The thread id. */
  int result;        /* Return code of the thread. */
  void *prev;        /* The previous named pipe in the list. */
  void *next;        /* The next named pipe in the list. */
} _named_pipe_;

void nmp_thread (void *);

#endif
