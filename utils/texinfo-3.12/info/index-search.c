/*
** *************************************************************************** 
** index-search.c:
** (c) Klaus Gebhardt, March 1994
** ****************************************************************************
** Changelog:
**  -  Falls der Terminal Type "Emacs" ist wird eine Nachricht ausgegeben.
**     (Keine Fehlermeldung!)
**     Klaus Gebhardt, Apr 1996
**  -  Wird kein Index gefunden, dann wird Info mit einer Fehlermeldung
**     beendet. Dabei wird der Screen NICHT gel”scht.
**     Klaus Gebhardt, Jul 1994
**  -  Die Funktion external_info_session () wird von Octave ben”tigt.
**     Klaus Gebhardt, May 1995
** ****************************************************************************
*/

#include "info.h"

int external_info_search_P = 0;

char *getenv ();

int external_info_search (initial_node, force,
			  info_file, index_search_string)
     NODE *initial_node;
     int force;
     char *info_file;
     char *index_search_string;
{
  char *term_name;
  int i;
  external_info_search_P = 1;

  if (stricmp (getenv ("TERM"), "emacs"))
    {
      initialize_info_session (initial_node, 0);

      if (force || index_entry_exists (windows, index_search_string))
	{
	  terminal_clear_screen ();
	  terminal_prep_terminal ();
	  display_update_display (windows);
	  info_last_executed_command = 0;
	  if (! force)  do_info_index_search (windows, 0, index_search_string);
	  external_info_search_P = 0;
	  return 1;
	}
      else  return 0;
    }
  else
    {
      fprintf (stderr, "*** Starting Emacs info browser (%s): %s ***\n",
	       info_file, index_search_string);
      return -1;
    }
}
