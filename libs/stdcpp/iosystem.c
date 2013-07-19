/* 
Copyright (C) 1993 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/*  written by Klaus Gebhardt (gebhardt@crunch.ikp.physik.th-darmstadt.de),
    1996 - 1997 */

#ifdef __EMX__
#include <stdlib.h>
#include <process.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#endif

#ifdef __EMX__
static char **_IO_split_args (const char *command, char *cmd, int *count)
{
  char *c;
  char **cmd_list;

  strcpy (cmd, command);
  cmd_list = _splitargs (cmd, count);

  if ((count > 0) && (cmd_list != NULL))
    {
      c = cmd_list[0];

      while (*c && (*c != ' ') && (*c != '\t')
	     && (*c != '\r') && (*c != '\n') && (*c != ''))
	{
	  if (*c == '/')  *c = '\\';
	  c++;
	}
    }

  return cmd_list;
}

static char *_IO_os2_paths (const char *command, char *cmd)
{
  char *c;

  strcpy (cmd, command);

  c = cmd;
  while (*c && (*c != ' ') && (*c != '\t')
	 && (*c != '\r') && (*c != '\n') && (*c != ''))
    {
      if (*c == '/')  *c = '\\';
      c++;
    }

  return cmd;
}

static char *_IO_unix_paths (const char *command, char *cmd)
{
  char *c;

  strcpy (cmd, command);

  c = cmd;
  while (*c && (*c != ' ') && (*c != '\t')
	 && (*c != '\r') && (*c != '\n') && (*c != ''))
    {
      if (*c == '\\')  *c = '/';
      c++;
    }

  return cmd;
}

int _IO_system(const char *command, int p_mode)
{
  int pid = -1;
  int count;
  char **cmd_list;
  char cmd[512];

  cmd_list = _IO_split_args (command, cmd, &count);

  if ((p_mode == P_PM) || (p_mode == P_SESSION))
    if ((count > 0) && (cmd_list != NULL))
      {
	pid = _spawnvp (p_mode, cmd_list[0], cmd_list);
	free (cmd_list);
      }

  if (pid == -1)
    {
      const char *sh, *base, *opt;

      if ((sh = getenv ("EMXSHELL")) == NULL)  sh = getenv ("COMSPEC");

      if (sh != NULL)
	{
	  base = _getname (sh);

	  if (stricmp(base, "cmd.exe") == 0 || stricmp(base, "4os2.exe") == 0)
	    {
	      opt = "/c";
	      _IO_os2_paths (command, cmd);
	    }
	  else
	    {
	      opt = "-c";
	      _IO_unix_paths (command, cmd);
	    }

	  pid = _spawnlp (p_mode, sh, sh, opt, cmd, NULL);
	}
    }

  return (pid);
}
#endif

