## Copyright (C) 1997 Klaus Gebhardt
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## usage: [IN, OUT, PID] = popen2 (COMMAND, ARGS)
##
## Start a subprocess with two-way communication.  COMMAND specifies
## the name of the command to start.  ARGS is an array of strings
## containing options for COMMAND.  IN and OUT are the file ids of the
## input and output streams for the subprocess, and PID is the process id of
## the subprocess, or -1 if COMMAND could not be executed.
##
## Example:
##
##  [in, out, pid] = popen2 ("sort", "-nr");
##  fputs (in, "these\n");
##  fputs (in, "are\n");
##  fputs (in, "some\n");
##  fputs (in, "strings\n");
##  fclose (in);
##  while (isstr (s = fgets (out)))
##    fputs (stdout, s);
##  endwhile
##  fclose (out);

## Author: Klaus Gebhardt, 1997

function [in, out, pid] = popen2 (command, args)

  in = -1;
  out = -1;
  pid = -1;

  if (nargin == 1 || nargin == 2)

    if (isstr (command))

      [r_pipe,  r_status]  = _pipe ();
      [w_pipe,  w_status]  = _pipe ();

      if ((r_status != -1) && (w_status != -1))

	r_parent_end = r_pipe(1);
	r_child_end  = r_pipe(2);
	_fcntl (r_parent_end, F_SETFD, 1);
	r_org = _dup (1);

	w_parent_end = w_pipe(2);
	w_child_end  = w_pipe(1);
	_fcntl (w_parent_end, F_SETFD, 1);
	w_org = _dup (0);

	if ((r_org != -1) && (w_org != -1))

	  _dup2 (r_child_end, 1);  _close (r_child_end);  r_child_end = -1;
	  _dup2 (w_child_end, 0);  _close (w_child_end);  w_child_end = -1;

	  if (nargin == 1)
	    pid = spawn ("nowait", command);
	  else
	    pid = spawn ("nowait", command, args);
	  endif

	  _dup2 (r_org, 1);  _close (r_org);
	  _dup2 (w_org, 0);  _close (w_org);
	endif

	if (r_child_end != -1)
	  _close (r_child_end);
	  endif

	if (w_child_end != -1)
	  _close (w_child_end);
	endif

	if (pid < 0)
	  _close (r_parent_end);
	  _close (w_parent_end);
	  error ("popen2: unable to start process `%s'", command);
	else
	  out = _fdopen (r_parent_end, "r");
	  in  = _fdopen (w_parent_end, "w");
	endif

	  else

	if (r_status != -1)
	  _close (r_pipe(1));
	  _close (r_pipe(2));
	  endif

	if (wr_status != -1)
	  _close (w_pipe(1));
	  _close (w_pipe(2));
	endif

	error ("popen2: pipe creation failed");
      endif
    else
      error ("popen2: file name must be a string");
    endif
  else
    usage ("[in, out, pid] = popen2 (command, args)");
  endif

endfunction
