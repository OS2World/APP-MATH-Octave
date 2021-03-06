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

/* Modified by Klaus Gebhardt, 1996 - 1997 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cassert>
#include <csetjmp>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <string>

#include <fstream.h>
#include <iostream.h>
#include <strstream.h>

#ifdef HAVE_UNISTD_H
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <unistd.h>
#endif

#include "lo-error.h"
#include "lo-mappers.h"
#include "str-vec.h"

#include "builtins.h"
#include <defaults.h>
#include "defun.h"
#include "dynamic-ld.h"
#include "error.h"
#include "file-io.h"
#include "help.h"
#include "input.h"
#include "lex.h"
#include <oct-conf.h>
#include "oct-hist.h"
#include "oct-map.h"
#include "pager.h"
#include "parse.h"
#include "pathsearch.h"
#include "procstream.h"
#include "ov.h"
#include "pt-fvc.h"
#include "pt-misc.h"
#include "pt-plot.h"
#include "sighandlers.h"
#include "sysdep.h"
#include "syswait.h"
#include "toplev.h"
#include "unwind-prot.h"
#include "utils.h"
#include "variables.h"
#include <version.h>

#ifdef __EMX__
extern "C"
{
#include <process.h>
  int _IO_system (const char *, int);
}
#endif

// argv[0] for this program.
string Vprogram_invocation_name;

// Cleaned-up name of this program, not including path information.
string Vprogram_name;

// Login name for user running this program.
string Vuser_name;

// Name of the host we are running on.
string Vhost_name;

// User's home directory.
string Vhome_directory;

// Nonzero means we print 
static bool Vdefault_eval_print_flag = true;

// Nonzero means we're breaking out of a loop or function body.
extern int breaking;

// Nonzero means we're returning from a function.
extern int returning;

// Nonzero means we are using readline.
// (--no-line-editing)
#if defined (USE_READLINE)
int using_readline = 1;
#else
int using_readline = 0;
#endif

#if defined (USE_READLINE)
// This is from readline's rltty.c:
extern "C" void rl_deprep_terminal (void);
#endif

// Nonzero means we printed messages about reading startup files.
int reading_startup_message_printed = 0;

// Command number, counting from the beginning of this session.
int current_command_number = 1;

// Nonzero means we are exiting via the builtin exit or quit functions.
int quitting_gracefully = 0;

// Current command to execute.
tree_statement_list *global_command = 0;

// Pointer to function that is currently being evaluated.
tree_function *curr_function = 0;

// Nonzero means input is coming from startup file.
int input_from_startup_file = 0;

// Nonzero means that input is coming from a file that was named on
// the command line.
int input_from_command_line_file = 1;

// Top level context (?)
jmp_buf toplevel;

void
parse_and_execute (FILE *f, int print)
{
  volatile octave_interrupt_handler old_interrupt_handler
    = octave_ignore_interrupts ();

  volatile jmp_buf old_toplevel = toplevel;

  begin_unwind_frame ("parse_and_execute");
  
  YY_BUFFER_STATE old_buf = current_buffer ();
  YY_BUFFER_STATE new_buf = create_buffer (f);

  add_unwind_protect (restore_input_buffer, (void *) old_buf);
  add_unwind_protect (delete_input_buffer, (void *) new_buf);

  switch_to_buffer (new_buf);

  unwind_protect_int (using_readline);
  unwind_protect_int (input_from_command_line_file);

  using_readline = 0;
  input_from_command_line_file = 0;

  unwind_protect_ptr (curr_sym_tab);

  if (setjmp(toplevel) == 0)
    {
      octave_set_interrupt_handler (old_interrupt_handler);

      int retval;
      do
	{
	  reset_parser ();

	  retval = yyparse ();

	  if (retval == 0 && global_command)
	    {
	      global_command->eval (print);

	      delete global_command;

	      global_command = 0;

	      bool quit = (returning || breaking);

	      if (returning)
		returning = 0;

	      if (breaking)
		breaking--;

	      if (error_state)
		{
		  error ("near line %d of file `%s'", input_line_number,
			 curr_fcn_file_full_name.c_str ());

		  break;
		}

	      if (quit)
		break;
	    }
	}
      while (retval == 0);
    }
  else  error ("script executing interrupted ...");

  run_unwind_frame ("parse_and_execute");

  octave_ignore_interrupts ();
  toplevel = old_toplevel;
  octave_set_interrupt_handler (old_interrupt_handler);
}

static void
safe_fclose (void *f)
{
  if (f)
    fclose ((FILE *) f);
}

void
parse_and_execute (const string& s, int print, int verbose,
		   const char *warn_for)
{
  volatile octave_interrupt_handler old_interrupt_handler
    = octave_ignore_interrupts ();

  begin_unwind_frame ("parse_and_execute_2");

  unwind_protect_int (reading_script_file);
  unwind_protect_str (curr_fcn_file_full_name);

  reading_script_file = 1;
  curr_fcn_file_full_name = s;

  FILE *f = get_input_from_file (s, 0);

  if (f)
    {
      add_unwind_protect (safe_fclose, (void *) f);

      unwind_protect_int (input_line_number);
      unwind_protect_int (current_input_column);

      input_line_number = 0;
      current_input_column = 1;

      if (verbose)
	{
	  cout << "reading commands from " << s << " ... ";
	  reading_startup_message_printed = 1;
	  cout.flush ();
	}

      parse_and_execute (f, print);

      if (verbose)
	cout << "done." << endl;
    }
  else if (warn_for)
    error ("%s: unable to open file `%s'", warn_for, s.c_str ());

  run_unwind_frame ("parse_and_execute_2");

  octave_set_interrupt_handler (old_interrupt_handler);
}

int
main_loop (void)
{
  // Allow the user to interrupt us without exiting.

  octave_save_signal_mask ();

  if (setjmp (toplevel) != 0)
    {
#if defined (__EMX__) && defined (OS2)
      run_all_unwind_protects ();
#endif

      raw_mode (0);

      cout << "\n";

      octave_restore_signal_mask ();
    }

  can_interrupt = 1;

  octave_catch_interrupts ();

  // The big loop.

  int retval;
  do
    {
#ifdef __EMX__
      can_interrupt = 1;
      octave_catch_interrupts ();
#endif

      curr_sym_tab = top_level_sym_tab;

      reset_parser ();

      retval = yyparse ();

      if (retval == 0 && global_command)
	{
	  global_command->eval (1);

	  delete global_command;

	  global_command = 0;

	  if (! (interactive || forced_interactive
		 || really_forced_interactive))
	    {
	      bool quit = (returning || breaking);

	      if (returning)
		returning = 0;

	      if (breaking)
		breaking--;

	      if (quit)
		break;
	    }

	  if (error_state)
	    {
	      if (! (interactive || forced_interactive
		     || really_forced_interactive))
		break;
	    }
	  else
	    {
	      if (octave_completion_matches_called)
		octave_completion_matches_called = false;	    
	      else
		current_command_number++;
	    }
	}
    }
  while (retval == 0);

  return retval;
}

DEFUN (source, args, ,
  "source (FILE)\n\
\n\
Parse and execute the contents of FILE.  Like executing commands in a\n\
script file but without requiring the file to be named `FILE.m'.")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
      string file = args(0).string_value ();

      if (! error_state)
	{
	  file = oct_tilde_expand (file);

	  int script_file_executed = parse_fcn_file (file, true, true);

	  if (error_state || ! script_file_executed)
	    error ("source: error sourcing file `%s'", file.c_str ());
	}
      else
	error ("source: expecting file name as argument");
    }
  else
    print_usage ("source");

  return retval;
}

// Fix up things before exiting.

void
clean_up_for_exit (void)
{
#if defined (USE_READLINE)
  rl_deprep_terminal ();
#else
  raw_mode (0);
#endif

  octave_command_history.clean_up_and_save ();

  close_plot_stream ();

  close_files ();

  cleanup_tmp_files ();

  flush_octave_stdout ();

  if (!quitting_gracefully &&
      (interactive || forced_interactive || really_forced_interactive))
    cout << "\n";
}

void
clean_up_and_exit (int retval)
{
  clean_up_for_exit ();

#if defined (__EMX__)
  _sleep2 (500);
#endif

  exit (retval == EOF ? 0 : retval);
}

DEFUN_TEXT (casesen, args, ,
  "casesen [on|off]")
{
  octave_value_list retval;

  int argc = args.length () + 1;

  string_vector argv = args.make_argv ("casesen");

  if (error_state)
    return retval;

  if (argc == 1 || (argc > 1 && argv[1] == "off"))
    warning ("casesen: sorry, Octave is always case sensitive");
  else if (argc > 1 && argv[1] == "on")
    ; // ok.
  else
    print_usage ("casesen");

  return retval;
}

DEFUN (computer, args, nargout,
  "computer ():\n\
\n\
Have Octave ask the system, \"What kind of computer are you?\"")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin != 0)
    warning ("computer: ignoring extra arguments");

  string msg;

  if (strcmp (CANONICAL_HOST_TYPE, "unknown") == 0)
    msg = "Hi Dave, I'm a HAL-9000";
  else
    msg = CANONICAL_HOST_TYPE;

  if (nargout == 0)
    octave_stdout << msg << "\n";
  else
    retval = msg;

  return retval;
}

DEFUN (quit, args, nargout,
  "quit (STATUS): exit Octave gracefully, returning STATUS to the system.\n\
\n\
STATUS should be an integer value.  If STATUS is missing, 0 is assumed.")
{
  octave_value_list retval;

  if (nargout == 0)
    {
      int exit_status = 0;

      quitting_gracefully = 1;

      if (args.length () > 0)
	{
	  // XXX FIXME XXX -- need a safe uniform way to do this.

	  double tmp = args(0).double_value ();

	  if (! error_state && ! xisnan (tmp))
	    exit_status = NINT (tmp);
	}

      clean_up_and_exit (exit_status);
    }
  else
    error ("quit: invalid number of output arguments");

  return retval;
}

DEFALIAS (exit, quit);

DEFUN (warranty, , ,
  "warranty (): describe copying conditions")
{
  octave_value_list retval;

  octave_stdout << "\n" OCTAVE_NAME_VERSION_AND_COPYRIGHT "\n\n\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program. If not, write to the Free Software\n\
Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n\
\n";

  return retval;
}

// XXX FIXME XXX -- this may not be the best place for these...

octave_value_list
feval (const string& name, const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  tree_fvc *fcn = is_valid_function (name, "feval", 1);

  if (fcn)
    retval = fcn->eval (0, nargout, args);

  return retval;
}

octave_value_list
feval (const octave_value_list& args, int nargout)
{
  octave_value_list retval;

  if (args.length () > 0)
    {
      string name = args(0).string_value ();

      if (! error_state)
	{
	  int tmp_nargin = args.length () - 1;
	  octave_value_list tmp_args;
	  tmp_args.resize (tmp_nargin);
	  for (int i = 0; i < tmp_nargin; i++)
	    tmp_args(i) = args(i+1);

	  retval = feval (name, tmp_args, nargout);
	}
    }

  return retval;
}

DEFUN (feval, args, nargout,
  "feval (NAME, ARGS, ...)\n\
\n\
evaluate NAME as a function, passing ARGS as its arguments")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    retval = feval (args, nargout);
  else
    print_usage ("feval");

  return retval;
}

static octave_value_list
eval_string (const string& s, int print, int& parse_status,
	     int nargout) 
{
  begin_unwind_frame ("eval_string");

  unwind_protect_int (get_input_from_eval_string);
  unwind_protect_int (input_from_command_line_file);
  unwind_protect_ptr (global_command);
  unwind_protect_str (current_eval_string);

  get_input_from_eval_string = 1;
  input_from_command_line_file = 0;
  current_eval_string = s;

  YY_BUFFER_STATE old_buf = current_buffer ();
  YY_BUFFER_STATE new_buf = create_buffer (0);

  add_unwind_protect (restore_input_buffer, (void *) old_buf);
  add_unwind_protect (delete_input_buffer, (void *) new_buf);

  switch_to_buffer (new_buf);

  unwind_protect_ptr (curr_sym_tab);

  reset_parser ();

  parse_status = yyparse ();

  // Important to reset the idea of where input is coming from before
  // trying to eval the command we just parsed -- it might contain the
  // name of an function file that still needs to be parsed!

  tree_statement_list *command = global_command;

  run_unwind_frame ("eval_string");

  octave_value_list retval;

  if (parse_status == 0 && command)
    {
      retval = command->eval (print, nargout);
      delete command;
    }

  return retval;
}

octave_value
eval_string (const string& s, int print, int& parse_status)
{
  octave_value retval;

  octave_value_list tmp = eval_string (s, print, parse_status, 1);

    retval = tmp(0);

  return retval;
}

static octave_value_list
eval_string (const octave_value& arg, int print, int& parse_status,
	     int nargout)
{
  string s = arg.string_value ();

  if (error_state)
    {
      error ("eval: expecting string argument");
      return -1.0;
    }

  return eval_string (s, print, parse_status, nargout);
}

DEFUN (eval, args, nargout,
  "eval (TRY, CATCH)\n\
\n\
Evaluate the string TRY as octave code.  If that fails, evaluate the\n\
string CATCH.")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0)
    {
      begin_unwind_frame ("Feval");

      if (nargin > 1)
	{
	  unwind_protect_int (buffer_error_messages);
	  buffer_error_messages = 1;
	}

      int parse_status = 0;

      retval = eval_string (args(0), Vdefault_eval_print_flag,
			    parse_status, nargout);

      if (nargin > 1 && (parse_status != 0 || error_state))
	{
	  error_state = 0;

	  // Set up for letting the user print any messages from
	  // errors that occurred in the first part of this eval().

	  buffer_error_messages = 0;
	  bind_global_error_variable ();
	  add_unwind_protect (clear_global_error_variable, 0);

	  eval_string (args(1), 0, parse_status, nargout);

	  retval = octave_value_list ();
	}

      run_unwind_frame ("Feval");
    }
  else
    print_usage ("eval");

  return retval;
}

// Execute a shell command.

static int cmd_status = -1;
static int cmd_pid    = -1;

static void
cmd_death_handler (pid_t, int status)
{
  cmd_status = status;
}

static void
cleanup_iprocstream (void *p)
{
  iprocstream *cmd = (iprocstream *) p;

  octave_child_list::remove (cmd_pid);

  delete cmd;
}

static octave_value_list
run_command_and_return_output (const string& cmd_str)
{
  octave_value_list retval;

  iprocstream *cmd = new iprocstream (cmd_str.c_str ());

  cmd_status = -1;
  cmd_pid    = cmd->pid ();

  if (cmd)
    {
      octave_child_list::insert (cmd_pid, cmd_death_handler);

      add_unwind_protect (cleanup_iprocstream, cmd);

      if (*cmd)
	{
	  ostrstream output_buf;

	  // XXX FIXME XXX -- sometimes, the subprocess hasn't written
	  // anything before we try to read from the procstream.  The
	  // kluge below (simply waiting and trying again) is ugly,
	  // but it seems to work, at least most of the time.  It
	  // could probably still fail if the subprocess hasn't
	  // started writing after the snooze.  Isn't there a better
	  // way?  If there is, you should also fix the code for the
	  // ls function in dirfns.cc.

	  char ch;

	  if (cmd->get (ch))
	    output_buf.put (ch);
	  else
	    {
	      cmd->clear ();

	      octave_usleep (100);
	    }

	  while (cmd->get (ch))
	    output_buf.put (ch);

	  cmd_status = cmd->close ();

	  if (WIFEXITED (cmd_status))
	    cmd_status = WEXITSTATUS (cmd_status);
	  else
	    cmd_status = 127;

	  output_buf << ends;

	  char *msg = output_buf.str ();

	  retval(1) = (double) cmd_status;
	  retval(0) = msg;

	  delete [] msg;
	}

      run_unwind_protect ();
    }
  else
    error ("unable to start subprocess for `%s'", cmd_str.c_str ());

  return retval;
}

DEFUN (system, args, nargout,
  "system (STRING [, RETURN_OUTPUT] [, TYPE])\n\
\n\
Execute the shell command specified by STRING.\n\
\n\
If TYPE is \"async\", the process is started in the background and the\n\
pid of the child proces is returned immediately.  Otherwise, the\n\
process is started, and Octave waits until it exits.  If TYPE argument\n\
is omitted, a value of \"sync\" is assumed.\n\
\n\
If NARGIN == 2 (the actual value of RETURN_OUTPUT is irrelevant) and\n\
the subprocess is started synchronously, or if system() is called with\n\
NARGIN == 1 and NARGOUT > 0, the output from the command is returned.\n\
Otherwise, if the subprocess is executed synchronously, it's output is\n\
sent to the standard output.  To send the output of a command executed\n\
with system() through the pager, use a command like\n\
\n\
   disp (system (CMD, 1));\n\
\n\
or\n\
\n\
   printf (\"%s\\n\", system (CMD, 1));")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin > 0 && nargin < 4)
    {
      bool return_output = (nargout > 0 || nargin > 1);

      string cmd_str = args(0).string_value ();

#if defined (__EMX__) && defined (OS2)
      enum exec_type { sync, async, pm };
#else
      enum exec_type { sync, async };
#endif

      exec_type type = sync;

      if (! error_state)
	{
	  if (nargin > 2)
	    {
	      string type_str = args(2).string_value ();

	      if (! error_state)
		{
		  if (type_str == "sync")
		    type = sync;
		  else if (type_str == "async")
		    type = async;
#if defined (__EMX__) && defined (OS2)
		  else if (type_str == "pm")
		    type = pm;
		  else
		    error ("system: third arg must be \"sync\" , \"async\""
			   "or \"pm\"");
#else
		  else
		    error ("system: third arg must be \"sync\" or \"async\"");
#endif
		}
	      else
		error ("system: third argument must be a string");
	    }
	}
      else
	error ("system: expecting string as first argument");

      if (! error_state)
	{
#if defined (__EMX__) && defined (OS2)
	  if (type == async || type == pm)
#else
	  if (type == async)
#endif
	    {
#ifndef __EMX__
	      pid_t pid = fork ();

	      if (pid < 0) 
		error ("system: fork failed -- can't create child process");
	      else if (pid == 0)
		{
		  system (cmd_str.c_str ());
		  exit (0);
		  retval(0) = 0.0;
		}
	      else
		retval(0) = (double) pid;
#else
	      pid_t pid;
	      int p_mode;
#ifdef OS2
	      if (type == pm)  p_mode = P_PM;
	      else             p_mode = P_NOWAIT;
#else
	      p_mode = P_NOWAIT;
#endif
	      pid = _IO_system (cmd_str.c_str (), p_mode);

	      if (pid < 0)
		error ("system: spawn failed -- can't create child process");
	      else
		retval(0) = (double) pid;
#endif
	    }
	  else if (return_output)
	    retval = run_command_and_return_output (cmd_str);
	  else
	    {
#ifdef __EMX__
 	      int status = _IO_system (cmd_str.c_str (), P_WAIT);
#else
	      int status = system (cmd_str.c_str ());
#endif

#ifndef __EMX__
	      // The value in status is as returned by waitpid.  If
	      // the process exited normally, extract the actual exit
	      // status of the command.  Otherwise, return 127 as a
	      // failure code.

	      if (WIFEXITED (status))
		status = WEXITSTATUS (status);
	      else
		status = 127;
#endif

	      retval = (double) status;
	    }
	}
    }
  else
    print_usage ("system");

  return retval;
}

DEFALIAS (shell_cmd, system);

static SLStack<string> octave_atexit_functions;

void
do_octave_atexit (void)
{
  while (! octave_atexit_functions.empty ())
    {
      octave_value_list fcn = octave_atexit_functions.pop ();

      feval (fcn, 0);

      flush_octave_stdout ();
    }
}

DEFUN (atexit, args, ,
  "atexit (NAME): register NAME as a function to call when Octave exits\n\
\n\
Functions are called with no arguments in the reverse of the order in\n\
which they were registered with atexit()")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin == 1)
    {
#if defined (HAVE_ATEXIT) || defined (HAVE_ON_EXIT)
      string arg = args(0).string_value ();

      if (! error_state)
	octave_atexit_functions.push (arg);
      else
	error ("atexit: argument must be a string");
#else
      gripe_not_supported ("atexit");
#endif
    }
  else
    print_usage ("atexit");

  return retval;
}

DEFUN (octave_config_info, args, ,
  "octave_config_info (OPTION)\n\
\n\
If OPTION is a string, return the configuration information for the\n\
specified option.\n\
\n\
With no arguments, return a structure containing configuration\n\
information.")
{
  octave_value retval;

#if defined (WITH_DYNAMIC_LINKING) && (defined (WITH_DL) || defined (WITH_SHL))
  bool octave_supports_dynamic_linking = true;
#else
  bool octave_supports_dynamic_linking = false;
#endif

  Octave_map m;

  // XXX FIXME XXX -- should we perform OCTAVE_HOME substitution on these?

  m ["default_pager"] = DEFAULT_PAGER;
  m ["prefix"] = OCTAVE_PREFIX;
  m ["exec_prefix"] = OCTAVE_EXEC_PREFIX;
  m ["datadir"] = OCTAVE_DATADIR;
  m ["dld"] = (double) octave_supports_dynamic_linking;
  m ["libdir"] = OCTAVE_LIBDIR;
  m ["bindir"] = OCTAVE_BINDIR;
  m ["infodir"] = OCTAVE_INFODIR;
  m ["fcnfiledir"] = OCTAVE_FCNFILEDIR;
  m ["localfcnfiledir"] = OCTAVE_LOCALFCNFILEDIR;
  m ["localstartupfiledir"] = OCTAVE_LOCALSTARTUPFILEDIR;
  m ["startupfiledir"] = OCTAVE_STARTUPFILEDIR;
  m ["localfcnfilepath"] = OCTAVE_LOCALFCNFILEPATH;
  m ["archlibdir"] = OCTAVE_ARCHLIBDIR;
  m ["localarchlibdir"] = OCTAVE_LOCALARCHLIBDIR;
  m ["octfiledir"] = OCTAVE_OCTFILEDIR;
  m ["localoctfilepath"] = OCTAVE_LOCALOCTFILEPATH;
  m ["fcnfilepath"] = OCTAVE_FCNFILEPATH;
  m ["imagepath"] = OCTAVE_IMAGEPATH;
  m ["canonical_host_type"] = CANONICAL_HOST_TYPE;
  m ["configure_options"] = config_opts;
  m ["F77"] = F77;
  m ["FFLAGS"] = FFLAGS;
  m ["FPICFLAG"] = FPICFLAG;
  m ["F2C"] = F2C;
  m ["F2CFLAGS"] = F2CFLAGS;
  m ["FLIBS"] = FLIBS;
  m ["CPPFLAGS"] = CPPFLAGS;
  m ["INCFLAGS"] = INCFLAGS;
  m ["CC"] = CC " " CC_VERSION;
  m ["CFLAGS"] = CFLAGS;
  m ["CPICFLAG"] = CPICFLAG;
  m ["CXX"] = CXX " " CXX_VERSION;
  m ["CXXFLAGS"] = CXXFLAGS;
  m ["CXXPICFLAG"] = CXXPICFLAG;
  m ["LDFLAGS"] = LDFLAGS;
  m ["LIBFLAGS"] = LIBFLAGS;
  m ["RLD_FLAG"] = RLD_FLAG;
  m ["TERMLIBS"] = TERMLIBS;
  m ["LIBS"] = LIBS;
  m ["LEXLIB"] = LEXLIB;
  m ["LIBPLPLOT"] = LIBPLPLOT;
  m ["LIBDLFCN"] = LIBDLFCN;
  m ["DEFS"] = DEFS;

  int nargin = args.length ();

  if (nargin == 1)
    {
      string arg = args(0).string_value ();

      if (! error_state)
	retval = octave_value (m [arg.c_str ()]);
    }
  else if (nargin == 0)
    retval = octave_value (m);
  else
    print_usage ("octave_config_info");

  return retval;
}

#if defined (__GNUG__) && defined (DEBUG_NEW_DELETE)

int debug_new_delete = 0;

typedef void (*vfp)(void);
extern vfp __new_handler;

void *
__builtin_new (size_t sz)
{
  void *p;

  /* malloc (0) is unpredictable; avoid it.  */
  if (sz == 0)
    sz = 1;
  p = (void *) malloc (sz);
  while (p == 0)
    {
      (*__new_handler) ();
      p = (void *) malloc (sz);
    }

  if (debug_new_delete)
    cout << "__builtin_new: " << p << endl;

  return p;
}

void
__builtin_delete (void *ptr)
{
  if (debug_new_delete)
    cout << "__builtin_delete: " << ptr << endl;

  if (ptr)
    free (ptr);
}

#endif

static int
default_eval_print_flag (void)
{
  Vdefault_eval_print_flag = check_preference ("default_eval_print_flag");

  return 0;
}

void
symbols_of_toplev (void)
{
  DEFVAR (default_eval_print_flag, 1.0, 0, default_eval_print_flag,
    "If the value of this variable is nonzero, Octave will print the\n\
results of commands executed by eval() that do not end with semicolons.");
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
