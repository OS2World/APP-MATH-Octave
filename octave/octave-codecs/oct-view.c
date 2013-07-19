/*
** oct-view:
** Displays ppm files via PMView 0.93
** (c) 1995 - 1997, Klaus Gebhardt
*/

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <process.h>

int main (int argc, char *argv[])
{
  int rc;
  char cmd_buffer[1024];

  if (argc == 3)
    {
      sprintf(cmd_buffer, "start /PM /PGM oct-view %s 2>NUL", argv[1]);
      rc = spawnlp(P_NOWAIT, "cmd.exe", "cmd.exe", "/c", cmd_buffer, NULL);
    }
  else if (argc == 2)
    {
      if (getenv("OCTAVE_IMAGE_VIEWER"))
	{
	  if (getenv("OCTAVE_IMAGE_VIEWER_OPT"))
	    rc = spawnlp(P_PM,
			 getenv("OCTAVE_IMAGE_VIEWER"),
			 getenv("OCTAVE_IMAGE_VIEWER"),
			 getenv("OCTAVE_IMAGE_VIEWER_OPT"),
			 argv[1], NULL);
	  else
	    rc = spawnlp(P_PM,
			 getenv("OCTAVE_IMAGE_VIEWER"),
			 getenv("OCTAVE_IMAGE_VIEWER"),
			 argv[1], NULL);
	}
      else
	rc = spawnlp(P_PM,
		     "k:\\apps4\\graphics\\pmview\\pmview.exe",
		     "k:\\apps4\\graphics\\pmview\\pmview.exe",
		     "/WPos(,,,,ForAct)",
		     argv[1], NULL);

      while (wait (NULL) != -1);
      unlink(argv[1]);
    }
  else
    return -1;

  return (rc);
}
