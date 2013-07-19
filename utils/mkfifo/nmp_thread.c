/*
** ****************************************************************************
** Thread for exchanging the data. This routine is started by the server in
** new thread for every nmaed pipe.
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

#include "nmp.h"

/*
** This function creates a named pipe and connects to it. The function
** returns 0 on success, -1 on error.
*/
static int create_npipe (const char *name, HPIPE *hpipe)
{
  APIRET apiret;

  apiret = DosCreateNPipe (name, hpipe, NP_ACCESS_DUPLEX,
                           NP_NOWAIT | NP_TYPE_BYTE | NP_READMODE_BYTE |
                           0xFF, 1024L, 1024L, -1);

  if (apiret)  return -1;

  apiret = DosConnectNPipe (*hpipe);
  if (apiret && (apiret != 233))  return -1;

  apiret = DosSetNPHState (*hpipe, NP_WAIT);
  if (apiret && (apiret != 233))  return -1;

  return 0;
}


/*
** This routine disconnects from a named pipe and closes the named pipe.
** It returns 0 on success, -1 on error.
*/
static int disconnect_npipe (HPIPE hpipe)
{
  DosDisConnectNPipe (hpipe);
  DosClose (hpipe);
  return 0;
}


/*
** This is the thread for moving the data from one end to the other. When
** a client closes its connection to a named pipe, the named pipe will be
** closed and recreated.
*/
void nmp_thread (void *ptr)
{
  APIRET apiret;
  HPIPE  hpipe1, hpipe2;
  ULONG  count1r = 0, count2w = 0, count2r = 0, count1w = 0, tmp;
  AVAILDATA available1, available2;
  ULONG  state1, state2;

  _named_pipe_ *nmp;
  char data1[1024], data2[1024];
  int rc, t = 10;

  nmp = (_named_pipe_ *) ptr;
  nmp->result = 0;

  rc = create_npipe (nmp->name, &hpipe1);  if (rc)  goto end;
  rc = create_npipe (nmp->name, &hpipe2);  if (rc)  goto end;

  DosPostEventSem (_hev_);
  DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, 0, nmp->tid);

  while (nmp->tid != -1)
    {
      apiret = DosPeekNPipe (hpipe1,&rc,sizeof(rc),&tmp,&available1,&state1);
      if (apiret)  goto error;
      apiret = DosPeekNPipe (hpipe2,&rc,sizeof(rc),&tmp,&available2,&state2);
      if (apiret)  goto error;

      if (((available1.cbpipe <= 0) && (available2.cbpipe <= 0)) ||
	  ((state1 != NP_STATE_CONNECTED) && (state1 != NP_STATE_CLOSING)) ||
	  ((state2 != NP_STATE_CONNECTED) && (state1 != NP_STATE_CLOSING)))
	{
	  DosSetPriority (PRTYS_THREAD, PRTYC_IDLETIME, 0, nmp->tid);

	  if (t < 50) t++;
	  DosSleep (t);
	}
      else
	{
	  DosSetPriority (PRTYS_THREAD, PRTYC_REGULAR, 0, nmp->tid);

	  t = 0;
	}

      if ((count1r > count2w) || (available1.cbpipe > 0))
	{
	  if (count1r <= count2w)
	    {
	      apiret = DosRead (hpipe1, data1, sizeof(data1), &count1r);
	      if (apiret == 109)  goto restart1;
	      else if (apiret)    goto error;
	      count2w = 0;
	    }

	  if (count1r > count2w)
	    {
	      apiret = DosWrite (hpipe2, data1+count2w, count1r-count2w, &tmp);
	      if (apiret == 109)  goto restart2;
	      else if (apiret)    goto error;
	      count2w += tmp;
	    }
	}

      if (state1 == NP_STATE_CLOSING)  goto restart1;

      if ((count2r > count1w) || (available2.cbpipe > 0))
	{
	  if (count2r <= count1w)
	    {
	      apiret = DosRead (hpipe2, data2, sizeof(data2), &count2r);
	      if (apiret == 109)  goto restart2;
	      else if (apiret)    goto error;
	      count1w = 0;
	    }

	  if (count2r > count1w)
	    {
	      apiret = DosWrite (hpipe1, data2+count1w, count2r-count1w, &tmp);
	      if (apiret == 109)  goto restart1;
	      else if (apiret)    goto error;
	      count1w += tmp;
	    }
	}

      if (state2 == NP_STATE_CLOSING)  goto restart2;

      continue;

    restart1:
      disconnect_npipe (hpipe1);
      rc = create_npipe (nmp->name, &hpipe1);
      if (rc)  goto error;
      continue;

    restart2:
      disconnect_npipe (hpipe2);
      rc = create_npipe (nmp->name, &hpipe2);
      if (rc)  goto error;
      continue;
    }

end:
  nmp->tid    = -1;
  disconnect_npipe (hpipe1);
  disconnect_npipe (hpipe2);

  _endthread ();
  return;

error:
  nmp->result = -1;
  nmp->tid    = -1;

  disconnect_npipe (hpipe1);
  disconnect_npipe (hpipe2);

  _endthread ();
  return;
}
