/*

Copyright (C) 1996 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

/* Modified by Klaus Gebhardt, 1995 - 1996 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <csignal>

#include <new>

#include <iostream.h>

#ifdef HAVE_UNISTD_H
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <unistd.h>
#endif

#ifdef __EMX__
#include <float.h>
#include <sys/uflags.h>
#endif

#include "error.h"
#include "input.h"
#include "load-save.h"
#include "pager.h"
#include "sighandlers.h"
#include "syswait.h"
#include "toplev.h"
#include "utils.h"

// Nonzero means we have already printed a message for this series of
// SIGPIPES.  We assume that the writer will eventually give up.
int pipe_handler_error_count = 0;

// Nonzero means we can be interrupted.
int can_interrupt = 0;

// Allow us to save the signal mask and then restore it to the most
// recently saved value.  This is necessary when using the POSIX
// signal handling interface on some systems calling longjmp out of
// the signal handler to get to the top level on an interrupt doesn't
// restore the original signal mask.  Alternatively, we could use
// sigsetjmp/siglongjmp, but saving and restoring the signal mask
// ourselves works ok and seems simpler just now.

#if defined (HAVE_POSIX_SIGNALS)
static sigset_t octave_signal_mask;
#endif

#if RETSIGTYPE == void
#define SIGHANDLER_RETURN(status) return
#else
#define SIGHANDLER_RETURN(status) return status
#endif

#if defined (MUST_REINSTALL_SIGHANDLERS)
#define MAYBE_REINSTALL_SIGHANDLER(sig, handler) \
  octave_set_signal_handler (sig, handler)
#else
#define MAYBE_REINSTALL_SIGHANDLER(sig, handler) \
  do { } while (0)
#endif

#if defined (__EMX__)
#define MAYBE_ACK_SIGNAL(sig) \
  signal (sig, SIG_ACK)
#else
#define MAYBE_ACK_SIGNAL(sig) \
  do { } while (0)
#endif

#if defined (SIGABRT)
#define OCTAVE_MEMORY_EXHAUSTED_ERROR SIGABRT
#else
#define OCTAVE_MEMORY_EXHAUSTED_ERROR (-1)
#endif

void
octave_save_signal_mask (void)
{
#if defined (HAVE_POSIX_SIGNALS)
  sigprocmask (0, 0, &octave_signal_mask);
#endif
}

void
octave_restore_signal_mask (void)
{
#if defined (HAVE_POSIX_SIGNALS)
  sigprocmask (SIG_SETMASK, &octave_signal_mask, 0);
#endif
}

static void
my_friendly_exit (const char *sig_name, int sig_number)
{
  static bool been_there_done_that = false;

  if (been_there_done_that)
    {
#if defined (SIGABRT)
      octave_set_signal_handler (SIGABRT, SIG_DFL);
#endif

      cerr << "error: attempted clean up apparently failed -- aborting...\n";

      abort ();
    }
  else
    {
      been_there_done_that = true;

      cerr << "\n\nerror: " << sig_name << " -- stopping myself...\n";

      save_user_variables ();

      clean_up_for_exit ();

      if (sig_number < 0)
	exit (1);
      else
	{
	  octave_set_signal_handler (sig_number, SIG_DFL);

	  kill (getpid (), sig_number);
	}
    }
}

// I know, not really a signal handler.

static void
octave_new_handler (void)
{
  cerr << "error: memory exhausted -- trying to return to prompt\n";

  if (can_interrupt)
    {
#ifdef __EMX__
      _uflags (_UF_SBRK_MODEL, _UF_SBRK_ARBITRARY);
#endif
      jump_to_top_level ();
      panic_impossible ();
    }
  else
    my_friendly_exit ("operator new", OCTAVE_MEMORY_EXHAUSTED_ERROR);
}

sig_handler *
octave_set_signal_handler (int sig, sig_handler *handler)
{
#if defined (HAVE_POSIX_SIGNALS)
  struct sigaction act, oact;
  act.sa_handler = handler;
  act.sa_flags = 0;
  sigemptyset (&act.sa_mask);
  sigemptyset (&oact.sa_mask);
  sigaction (sig, &act, &oact);
  return oact.sa_handler;
#elif defined (__EMX__)
  sig_handler *signal_handler = signal (sig, handler);
  MAYBE_ACK_SIGNAL(sig);
  return signal_handler;
#else
  return signal (sig, handler);
#endif
}

static RETSIGTYPE
generic_sig_handler (int sig)
{
  my_friendly_exit (sys_siglist[sig], sig);

  SIGHANDLER_RETURN (0);
}

// Handle SIGCHLD.

static RETSIGTYPE
sigchld_handler (int sig)
{
  volatile octave_interrupt_handler saved_interrupt_handler
     = octave_ignore_interrupts ();

  // I wonder if this is really right, or if SIGCHLD should just be
  // blocked on OS/2 systems the same as for systems with POSIX signal
  // functions.

#if defined (__EMX__)
  volatile sig_handler *saved_sigchld_handler
    = octave_set_signal_handler (SIGCHLD, SIG_IGN);
#else
  sigset_t set, oset;

  BLOCK_CHILD (set, oset);
#endif

  int i = 0;
  int n = octave_child_list::length ();

  for (i = 0; i < n; i++)
	{
	  octave_child& elt = octave_child_list::elem (i);

	  pid_t pid = elt.pid;

	  if (pid > 0)
	    {
	      int status;

	      if (waitpid (pid, &status, WNOHANG) > 0)
		{
		  elt.pid = -1;

		  octave_child::dead_child_handler f = elt.handler;

	      if (f)  f (pid, status);
	      else
		{
		  string file = elt.filename;
		  unlink (file.c_str ());
		}

		  break;
		}
	    }
	}

  if (i == n)  waitpid (-1, NULL, WNOHANG);

  octave_set_interrupt_handler (saved_interrupt_handler);

#ifndef __EMX__
  UNBLOCK_CHILD (oset);
#else
  octave_set_signal_handler (SIGCHLD, saved_sigchld_handler);
#endif

  MAYBE_ACK_SIGNAL (SIGCHLD);

  MAYBE_REINSTALL_SIGHANDLER (SIGCHLD, sigchld_handler);

  SIGHANDLER_RETURN (0);
}

#if defined (__alpha__)
static RETSIGTYPE
sigfpe_handler (int sig)
{
  MAYBE_ACK_SIGNAL (SIGFPE);

  MAYBE_REINSTALL_SIGHANDLER (SIGFPE, sigfpe_handler);

  cerr << "error: floating point exception -- trying to return to prompt\n";

  if (can_interrupt)
    {
#if defined (__EMX__)
      _control87 (EM_INVALID | EM_DENORMAL | EM_ZERODIVIDE | EM_OVERFLOW |
		  EM_UNDERFLOW | EM_INEXACT, MCW_EM);
#endif
      jump_to_top_level ();
      panic_impossible ();
    }

  SIGHANDLER_RETURN (0);
}
#endif

#if 0
#if defined (SIGWINCH)

#if defined (USE_READLINE)
#include <readline/readline.h>
#include <readline/history.h>
#endif

static RETSIGTYPE
sigwinch_handler (int /* sig */)
{
  MAYBE_ACK_SIGNAL (SIGWINCH);

  MAYBE_REINSTALL_SIGHANDLER (SIGWINCH, sigwinch_handler);

  SIGHANDLER_RETURN (0);
}
#endif
#endif

// Handle SIGINT by restarting the parser (see octave.cc).
//
// This also has to work for SIGBREAK (on systems that have it), so we
// use the value of sig, instead of just assuming that it is called
// for SIGINT only.

static RETSIGTYPE
sigint_handler (int sig)
{
  MAYBE_ACK_SIGNAL (sig);

  MAYBE_REINSTALL_SIGHANDLER (sig, sigint_handler);

  if (can_interrupt)
    {
      jump_to_top_level ();
      panic_impossible ();
    }

  SIGHANDLER_RETURN (0);
}

static RETSIGTYPE
sigpipe_handler (int sig)
{
  MAYBE_ACK_SIGNAL (SIGPIPE);

  MAYBE_REINSTALL_SIGHANDLER (SIGPIPE, sigpipe_handler);

  if (pipe_handler_error_count++ == 0)
    cerr << "warning: broken pipe\n";

  // Don't loop forever on account of this.

  if (pipe_handler_error_count  > 100)
    jump_to_top_level ();

  SIGHANDLER_RETURN (0);
}

octave_interrupt_handler
octave_catch_interrupts (void)
{
  octave_interrupt_handler retval;

#ifdef SIGINT
  octave_set_signal_handler (SIGINT, sigint_handler);
  retval.int_handler = sigint_handler;
  MAYBE_ACK_SIGNAL (SIGINT);
#endif

#ifdef SIGBREAK
  octave_set_signal_handler (SIGBREAK, sigint_handler);
  retval.brk_handler = sigint_handler;
  MAYBE_ACK_SIGNAL (SIGBREAK);
#endif

  return retval;
}

octave_interrupt_handler
octave_ignore_interrupts (void)
{
  octave_interrupt_handler retval;

#ifdef SIGINT
  retval.int_handler = octave_set_signal_handler (SIGINT, SIG_IGN);
#endif

#ifdef SIGBREAK
  retval.brk_handler = octave_set_signal_handler (SIGBREAK, SIG_IGN);
#endif

  return retval;
}

octave_interrupt_handler
octave_set_interrupt_handler (RETSIGTYPE (*f)(int))
{
  octave_interrupt_handler retval;

  if (f)
    {
#ifdef SIGINT
      retval.int_handler = octave_set_signal_handler (SIGINT, f);
      MAYBE_ACK_SIGNAL (SIGINT);
#endif

#ifdef SIGBREAK
      retval.brk_handler = octave_set_signal_handler (SIGBREAK, f);
      MAYBE_ACK_SIGNAL (SIGBREAK);
#endif
    }

  return retval;
}

octave_interrupt_handler
octave_set_interrupt_handler (const volatile octave_interrupt_handler& h)
{
  octave_interrupt_handler retval;

#ifdef SIGINT
  retval.int_handler = octave_set_signal_handler (SIGINT, h.int_handler);
  MAYBE_ACK_SIGNAL (SIGINT);
#endif

#ifdef SIGBREAK
  retval.brk_handler = octave_set_signal_handler (SIGBREAK, h.brk_handler);
  MAYBE_ACK_SIGNAL (SIGBREAK);
#endif

  return retval;
}

// Install all the handlers for the signals we might care about.

void
install_signal_handlers (void)
{
  set_new_handler (octave_new_handler);

  octave_catch_interrupts ();

#ifdef SIGABRT
  octave_set_signal_handler (SIGABRT, generic_sig_handler);
#endif

#ifdef SIGALRM
  octave_set_signal_handler (SIGALRM, generic_sig_handler);
#endif

#ifdef SIGBUS
  octave_set_signal_handler (SIGBUS, generic_sig_handler);
#endif

#ifdef SIGCHLD
  octave_set_signal_handler (SIGCHLD, sigchld_handler);
#endif

  // SIGCLD
  // SIGCONT

#ifdef SIGEMT
  octave_set_signal_handler (SIGEMT, generic_sig_handler);
#endif

#ifdef SIGFPE
#if defined (__alpha__)
  octave_set_signal_handler (SIGFPE, sigfpe_handler);
#else
  octave_set_signal_handler (SIGFPE, generic_sig_handler);
#endif
#endif

#ifdef SIGHUP
  octave_set_signal_handler (SIGHUP, generic_sig_handler);
#endif

#ifdef SIGILL
  octave_set_signal_handler (SIGILL, generic_sig_handler);
#endif

  // SIGINFO
  // SIGINT

#ifdef SIGIO
  octave_set_signal_handler (SIGIO, SIG_IGN);
#endif

#ifdef SIGIOT
  octave_set_signal_handler (SIGIOT, generic_sig_handler);
#endif

#ifdef SIGLOST
  octave_set_signal_handler (SIGLOST, generic_sig_handler);
#endif

#ifdef SIGPIPE
  octave_set_signal_handler (SIGPIPE, sigpipe_handler);
#endif

#ifdef SIGPOLL
  octave_set_signal_handler (SIGPOLL, SIG_IGN);
#endif

#ifdef SIGPROF
  octave_set_signal_handler (SIGPROF, generic_sig_handler);
#endif

  // SIGPWR

#ifdef SIGQUIT
  octave_set_signal_handler (SIGQUIT, generic_sig_handler);
#endif

#ifdef SIGSEGV
  octave_set_signal_handler (SIGSEGV, generic_sig_handler);
#endif

  // SIGSTOP

#ifdef SIGSYS
  octave_set_signal_handler (SIGSYS, generic_sig_handler);
#endif

#ifdef SIGTERM
#ifdef __EMX__
  signal (SIGTERM, SIG_DFL);
#else
  octave_set_signal_handler (SIGTERM, generic_sig_handler);
#endif
#endif

#ifdef SIGTRAP
  octave_set_signal_handler (SIGTRAP, generic_sig_handler);
#endif

  // SIGTSTP
  // SIGTTIN
  // SIGTTOU
  // SIGURG

#ifdef SIGUSR1
  octave_set_signal_handler (SIGUSR1, generic_sig_handler);
#endif

#ifdef SIGUSR2
  octave_set_signal_handler (SIGUSR2, generic_sig_handler);
#endif

#ifdef SIGVTALRM
  octave_set_signal_handler (SIGVTALRM, generic_sig_handler);
#endif

#if 0
#ifdef SIGWINCH
  octave_set_signal_handler (SIGWINCH, sigwinch_handler);
#endif
#endif

#ifdef SIGXCPU
  octave_set_signal_handler (SIGXCPU, generic_sig_handler);
#endif

#ifdef SIGXFSZ
  octave_set_signal_handler (SIGXFSZ, generic_sig_handler);
#endif
}

#ifndef HAVE_SYS_SIGLIST
char *sys_siglist[NSIG + 1] =
{
#ifdef AIX
/* AIX has changed the signals a bit */
  "bogus signal",			/* 0 */
  "hangup",				/* 1  SIGHUP */
  "interrupt",				/* 2  SIGINT */
  "quit",				/* 3  SIGQUIT */
  "illegal instruction",		/* 4  SIGILL */
  "trace trap",				/* 5  SIGTRAP */
  "IOT instruction",			/* 6  SIGIOT */
  "crash likely",			/* 7  SIGDANGER */
  "floating point exception",		/* 8  SIGFPE */
  "kill",				/* 9  SIGKILL */
  "bus error",				/* 10 SIGBUS */
  "segmentation violation",		/* 11 SIGSEGV */
  "bad argument to system call",	/* 12 SIGSYS */
  "write on a pipe with no one to read it", /* 13 SIGPIPE */
  "alarm clock",			/* 14 SIGALRM */
  "software termination signum",	/* 15 SIGTERM */
  "user defined signal 1",		/* 16 SIGUSR1 */
  "user defined signal 2",		/* 17 SIGUSR2 */
  "death of a child",			/* 18 SIGCLD */
  "power-fail restart",			/* 19 SIGPWR */
  "bogus signal",			/* 20 */
  "bogus signal",			/* 21 */
  "bogus signal",			/* 22 */
  "bogus signal",			/* 23 */
  "bogus signal",			/* 24 */
  "LAN I/O interrupt",			/* 25 SIGAIO */
  "PTY I/O interrupt",			/* 26 SIGPTY */
  "I/O intervention required",		/* 27 SIGIOINT */
  "HFT grant",				/* 28 SIGGRANT */
  "HFT retract",			/* 29 SIGRETRACT */
  "HFT sound done",			/* 30 SIGSOUND */
  "HFT input ready",			/* 31 SIGMSG */
#else /* not AIX */
#ifdef __EMX__
  "bogus signal #0",                    /* 0 */
  "hangup",                             /* 1  SIGHUP */
  "interrupt (Ctrl-C)",                 /* 2  SIGINT (Ctrl-C) */
  "quit",                               /* 3  SIGQUIT */
  "illegal instruction",                /* 4  SIGILL */
  "single step",                        /* 5  SIGTRAP */
  "abort",                              /* 6  SIGABRT */
  "EMT instruction",                    /* 7  SIGEMT */
  "floating point exception",           /* 8  SIGFPE */
  "kill",                               /* 9  SIGKILL */
  "bus error",                          /* 10 SIGBUS */
  "segmentation violation",             /* 11 SIGSEGV */
  "bad argument to system call",        /* 12 SIGSYS */
  "broken pipe",                        /* 13 SIGPIPE */
  "alarm clock",                        /* 14 SIGALRM */
  "software termination signum",        /* 15 SIGTERM */
  "user defined signal 1",              /* 16 SIGUSR1 */
  "user defined signal 2",              /* 17 SIGUSR2 */
  "death of a child",                   /* 18 SIGCHLD */
  "bogus signal #20",                   /* 19 */
  "bogus signal #21",                   /* 20 */
  "break (Ctrl-Break)",                 /* 21 SIGBREAK */
#else /* not __EMX__ */
  "bogus signal",			/* 0 */
  "hangup",				/* 1  SIGHUP */
  "interrupt",				/* 2  SIGINT */
  "quit",				/* 3  SIGQUIT */
  "illegal instruction",		/* 4  SIGILL */
  "trace trap",				/* 5  SIGTRAP */
  "IOT instruction",			/* 6  SIGIOT */
  "EMT instruction",			/* 7  SIGEMT */
  "floating point exception",		/* 8  SIGFPE */
  "kill",				/* 9  SIGKILL */
  "bus error",				/* 10 SIGBUS */
  "segmentation violation",		/* 11 SIGSEGV */
  "bad argument to system call",	/* 12 SIGSYS */
  "write on a pipe with no one to read it", /* 13 SIGPIPE */
  "alarm clock",			/* 14 SIGALRM */
  "software termination signum",	/* 15 SIGTERM */
  "user defined signal 1",		/* 16 SIGUSR1 */
  "user defined signal 2",		/* 17 SIGUSR2 */
  "death of a child",			/* 18 SIGCLD */
  "power-fail restart",			/* 19 SIGPWR */
#ifdef sun
  "window size change",			    /* 20 SIGWINCH */
  "urgent socket condition",		    /* 21 SIGURG */
  "pollable event occured",		    /* 22 SIGPOLL */
  "stop (cannot be caught or ignored)", /*  23 SIGSTOP */
  "user stop requested from tty",	    /* 24 SIGTSTP */
  "stopped process has been continued",	/* 25 SIGCONT */
  "background tty read attempted",	    /* 26 SIGTTIN */
  "background tty write attempted",    /* 27 SIGTTOU */
  "virtual timer expired",		    /* 28 SIGVTALRM */
  "profiling timer expired",		    /* 29 SIGPROF */
  "exceeded cpu limit",			    /* 30 SIGXCPU */
  "exceeded file size limit",		    /* 31 SIGXFSZ */
  "process's lwps are blocked",	    /*  32 SIGWAITING */
  "special signal used by thread library", /* 33 SIGLWP */
#ifdef SIGFREEZE
  "Special Signal Used By CPR",	    /* 34 SIGFREEZE */
#endif
#ifdef SIGTHAW
  "Special Signal Used By CPR",	    /* 35 SIGTHAW */
#endif
#endif /* sun */
#endif /* __EMX__ */
#endif /* AIX */
  0
  };
#endif

octave_child_list *octave_child_list::instance = 0;

void
octave_child_list::do_insert (pid_t pid, octave_child::dead_child_handler f,
			      string name)
{
  // Insert item in first open slot, increasing size of list if
  // necessary.

  bool enlarge = true;

  for (int i = 0; i < curr_len; i++)
    {
      octave_child& tmp = list (i);

      if (tmp.pid < 0)
	{
	  list (i) = octave_child (pid, f, name);
	  enlarge = false;
	  break;
	}
    }

  if (enlarge)
    {
      int total_len = list.length ();

      if (curr_len == total_len)
	{
	  if (total_len == 0)
	    list.resize (16);
	  else
	    list.resize (total_len * 2);
	}

      list (curr_len) = octave_child (pid, f, name);
      curr_len++;
    }
}

void
octave_child_list::insert (pid_t pid, octave_child::dead_child_handler f,
			   string name)
{
  if (! instance)
    instance = new octave_child_list ();

  if (instance)
    instance->do_insert (pid, f, name);
  else
    panic_impossible ();
}

void
octave_child_list::do_remove (pid_t pid)
{
  // Mark the record for PID invalid.

  for (int i = 0; i < curr_len; i++)
    {
      octave_child& tmp = list (i);

      if (tmp.pid == pid)
	{
	  tmp.pid = -1;

	  octave_child::dead_child_handler f = tmp.handler;

	  if (f)
	    {
	      int status;

	      if (waitpid (pid, &status, WNOHANG) <= 0)  status = 127;

	      f (pid, status);
	    }
	  else
	    {
	      string file = tmp.filename;
	      unlink (file.c_str ());
	    }

	  break;
	}
    }
}

void
octave_child_list::remove (pid_t pid)
{
  if (! instance)
    instance = new octave_child_list ();

  if (instance)
    instance->do_remove (pid);
  else
    panic_impossible ();
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
