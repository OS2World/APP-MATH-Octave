/* gnuview.c -- This program interprets the commandline arguments like
   info does and starts OS/2 view.exe. */

/* Copyright (C) 1993 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Written by Klaus Gebhardt. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "getopt.h"

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <process.h>
#include <ctype.h>

/* Defines that are passed along with the pathname to gnuview_add_path (). */
#define INFOPATH_PREPEND               0
#define INFOPATH_APPEND                1
#define INFOPATH_APPEND_WITHOUT_COLON  2
#define INFOPATH_PREPEND_WITHOUT_COLON 3

#define COLONCHR ';'
#define COLONSTR ";"
#define PATHSEPCHR '\\'
#define PATHSEPSTR "\\"

/* Add POINTER to the list of pointers found in ARRAY.  SLOTS is the number
   of slots that have already been allocated.  INDEX is the index into the
   array where POINTER should be added.  GROW is the number of slots to grow
   ARRAY by, in the case that it needs growing.  TYPE is a cast of the type
   of object stored in ARRAY (e.g., NODE_ENTRY *. */
#define add_pointer_to_array(pointer, idx, array, slots, grow, type) \
  do { \
    if (idx + 2 >= slots) \
      array = (type *)(xrealloc (array, (slots += grow) * sizeof (type))); \
    array[idx++] = (type)pointer; \
    array[idx] = (type)NULL; \
  } while (0)

#define maybe_free(x) do { if (x) free (x); } while (0)

/* The version numbers of this version of GnuView. */
int gnuview_major_version = 1;
int gnuview_minor_version = 0;
int gnuview_patch_level = 0;

/* Non-zero means search all indices for APROPOS_SEARCH_STRING. */
static int apropos_p = 0;

/* Variable containing the string to search for when apropos_p is non-zero. */
static char *apropos_search_string = (char *)NULL;

/* Non-zero means search all indices for INDEX_SEARCH_STRING.  Unlike
   apropos, this puts the user at the node, running GnuView. */
static int index_search_p = 0;

/* Variable containing the string to search for when index_search_p is
   non-zero. */
static char *index_search_string = (char *)NULL;

/* Non-zero means print version GnuView only. */
static int print_version_p = 0;

/* Non-zero means print a short description of the options. */
static int print_help_p = 0;

/* Array of the names of nodes that the user specified with "--node" on the
   command line. */
static char **user_nodenames = (char **)NULL;
static int user_nodenames_index = 0;
static int user_nodenames_slots = 0;

/* String specifying the first file to load.  This string can only be set
   by the user specifying "--file" on the command line. */
static char *user_filename = (char *)NULL;

/* String specifying the name of the file to dump nodes to.  This value is
   filled if the user speficies "--output" on the command line. */
static char *user_output_filename = (char *)NULL;

/* Non-zero indicates that when "--output" is specified, all of the menu
   items of the specified nodes (and their subnodes as well) should be
   dumped in the order encountered.  This basically can print a book. */
int dump_subnodes = 0;

/* Structure describing the options that GnuView accepts.  We pass this
   structure to getopt_long ().  If you add or otherwise change this
   structure, you must also change the string which follows it. */
#define APROPOS_OPTION 1
#define DRIBBLE_OPTION 2
#define RESTORE_OPTION 3
#define IDXSRCH_OPTION 4
static struct option long_options[] = {
  { "apropos", 1, 0, APROPOS_OPTION },
  { "directory", 1, 0, 'd' },
  { "node", 1, 0, 'n' },
  { "file", 1, 0, 'f' },
  { "subnodes", 0, &dump_subnodes, 1 },
  { "output", 1, 0, 'o' },
  { "help", 0, &print_help_p, 1 },
  { "version", 0, &print_version_p, 1 },
  { "dribble", 1, 0, DRIBBLE_OPTION },
  { "restore", 1, 0, RESTORE_OPTION },
  { "index-search", 1, 0, IDXSRCH_OPTION },
  {NULL, 0, NULL, 0}
};

/* String describing the shorthand versions of the long options found above. */
static char *short_options = "d:n:f:o:sh?";

/* Some "forward" declarations. */
static void usage ();
static void gnuview_short_help ();
static void remember_gnuview_program_name ();
static char *gnuview_version_string ();

/* The path on which we look for info files.  You can initialize this
   from the environment variable BOOKSHELF if there is one, or you can
   call gnuview_add_path () to add paths to the beginning or end of it. */
char *gnuviewpath = (char *)NULL;
static int gnuviewpath_size = 0;

static void gnuview_add_path (char *, int);

static void os2_path_sep (char *);
static char *gnuview_filename_non_directory (char *);

char *program_name = (char *)NULL;


/* **************************************************************** */
/*								    */
/*		  Main Entry Point to the GnuView Program	    */
/*								    */
/* **************************************************************** */

int main (int argc, char **argv)
{
  int getopt_long_index;	/* Index returned by getopt_long (). */

  remember_gnuview_program_name (argv[0]);

  setvbuf(stdout, NULL, _IOFBF, BUFSIZ);

  while (1)
    {
      int option_character;

      option_character = getopt_long
	(argc, argv, short_options, long_options, &getopt_long_index);

      /* getopt_long () returns EOF when there are no more long options. */
      if (option_character == EOF) break;

      /* If this is a long option, then get the short version of it. */
      if (option_character == 0 && long_options[getopt_long_index].flag == 0)
	option_character = long_options[getopt_long_index].val;

      /* Case on the option that we have received. */
      switch (option_character)
	{
	case 0:
	  break;

	  /* User wants to add a directory. */
	case 'd':
	  gnuview_add_path (optarg, INFOPATH_PREPEND);
	  break;

	  /* User is specifying a particular node. */
	case 'n':
	  add_pointer_to_array (optarg, user_nodenames_index, user_nodenames,
				user_nodenames_slots, 10, char *);
	  break;

	  /* User is specifying a particular INF file. */
	case 'f':
	  if (user_filename) free (user_filename);
	  os2_path_sep (user_filename = strdup (optarg));
	  break;

	  /* User is requesting help. */
	case 'h':
	  print_help_p = 1;
	  break;

	  /* User is specifying the name of a file to output to. */
	case 'o':
	  if (user_output_filename) free (user_output_filename);
	  os2_path_sep (user_output_filename = strdup (optarg));
	  break;

	  /* User is specifying that she wishes to dump the subnodes of
	     the node that she is dumping. */
	case 's':
	  dump_subnodes = 1;
	  break;

	  /* User has specified a string to search all indices for. */
	case APROPOS_OPTION:
	  apropos_p = 1;
	  maybe_free (apropos_search_string);
	  apropos_search_string = strdup (optarg);
	  break;

	  /* User has specified a dribble file to receive keystrokes. */
	case DRIBBLE_OPTION:
	  break;

	  /* User has specified an alternate input stream. */
	case RESTORE_OPTION:
	  break;

	  /* User has specified a string to search all indices for. */
	case IDXSRCH_OPTION:
	  index_search_p = 1;
	  maybe_free (index_search_string);
	  index_search_string = strdup (optarg);
	  break;

	default:
	  usage ();
	}
    }

  /* there is no INF file given in the command line -> exit */
  if (!user_filename)
    {
      fprintf (stderr,"\nGnuView, Version %s.\n", gnuview_version_string ());
      fprintf (stderr,"Error: No file to visit.\n");
    }

  /* If the user specified --version, then show the version and exit. */
  if (print_version_p)
    {
      printf ("GnuView, Version %s.\n", gnuview_version_string ());
      exit (0);
    }

  /* If the `--help' option was present, show the help and exit. */
  if (print_help_p)
    {
      gnuview_short_help ();
      exit (0);
    }
  
  /* If the user hasn't specified a path for INF files, default that path
     now. */
  if (!gnuviewpath)
    {
      char *path_from_env, *getenv ();

      path_from_env = getenv ("BOOKSHELF");

      if (path_from_env) gnuview_add_path (path_from_env, INFOPATH_PREPEND);
      else               gnuview_add_path (DEFAULT_INFOPATH, INFOPATH_PREPEND);
    }

  /* If the user specified a particular filename, add the path of that
     file to the contents of BOOKSHELF. */
  if (user_filename)
    {
      char *directory_name, *temp;

      directory_name = strdup (user_filename);
      temp = gnuview_filename_non_directory (directory_name);

      if (temp != directory_name)
	{
	  *temp = 0;
	  gnuview_add_path (directory_name, INFOPATH_PREPEND);
	}

      free (directory_name);
    }

  gnuview_add_path ("SET BOOKSHELF=", INFOPATH_PREPEND_WITHOUT_COLON);
  gnuview_add_path (" & view ", INFOPATH_APPEND_WITHOUT_COLON);
  gnuview_add_path (user_filename, INFOPATH_APPEND_WITHOUT_COLON);
  gnuview_add_path (" ", INFOPATH_APPEND_WITHOUT_COLON);

  if (index_search_string)
    gnuview_add_path (index_search_string, INFOPATH_APPEND_WITHOUT_COLON);
  else if (apropos_search_string)
    gnuview_add_path (apropos_search_string, INFOPATH_APPEND_WITHOUT_COLON);

  system (gnuviewpath);

  exit (0);
}

static void remember_gnuview_program_name (char *fullpath)
{
  char *filename;
  filename = gnuview_filename_non_directory (fullpath);
  program_name = strdup (filename);
}

/* Return a string describing the current version of GnuView. */
static char *gnuview_version_string ()
{
  static char *vstring = (char *)NULL;

  if (!vstring)
    {
      vstring = (char *)xmalloc (50);

      sprintf (vstring, "%d.%d", gnuview_major_version, gnuview_minor_version);
      if (gnuview_patch_level)
        sprintf (vstring + strlen (vstring), "-p%d", gnuview_patch_level);

      sprintf (vstring + strlen (vstring), " for OS/2 2.x and Warp");
    }
  return (vstring);
}

/* Produce a very brief descripton of the available options and exit with
   an error. */
static void usage ()
{
  fprintf (stderr,"\nGnuView, Version %s.\n", gnuview_version_string ());
  fprintf (stderr,"%s\n%s\n%s\n%s\n%s\n",
"Usage: gnuview -f INF-file [-d dir] [-o outfile-file] [-n node-name]...",
"               [--directory dir] [--file INF-file] [--node node-name]...",
"               [--help] [--output output-file] [--subnodes] [--version]",
"               [--dribble dribble-file] [--restore from-file]",
"               [--index-search string] [menu-selection ...]");
  exit (1);
}

/* Produce a scaled down description of the available options to GnuView. */
static void gnuview_short_help ()
{
  printf ("%s", "\
Here is a quick description of GnuView's options.  For a more complete\n\
description of how to use GnuView, type `gnuview gnuview options'.\n\
\n\
   --directory DIR		Add DIR to BOOKSHELF.\n\
   --file FILENAME		Specify INF file to visit.\n\
   --index-search STRING        Specify STRING to search for in the index.\n\
   --node NODENAME		Specify nodes in first visited INF file.\n\
   --output FILENAME		Output selected nodes to FILENAME.\n\
   --dribble FILENAME		Remember user keystrokes in FILENAME.\n\
   --restore FILENAME		Read initial keystrokes from FILENAME.\n\
   --subnodes			Recursively output menu items.\n\
   --help			Get this help message.\n\
   --version			Display GnuView's version information.\n\
\n\
Remaining arguments to GnuView are treated as the names of menu\n\
items in the initial node visited.  You can easily move to the\n\
node of your choice by specifying the menu names which describe\n\
the path to that node.  For example, `gnuview emacs buffers'.\n");

  exit (0);
}

/* Add PATH to the list of paths found in BOOKSHELF.  2nd argument says
   whether to put PATH at the front or end of BOOKSHELF. */
static void gnuview_add_path (char *path, int where)
{
  int len;

  if ((where != INFOPATH_APPEND_WITHOUT_COLON) &&
      (where != INFOPATH_PREPEND_WITHOUT_COLON))
    os2_path_sep (path);

  if (!gnuviewpath)
    {
      gnuviewpath = (char *)xmalloc (gnuviewpath_size = 200 + strlen (path));
      gnuviewpath[0] = '\0';
    }

  len = strlen (path) + strlen (gnuviewpath);

  if (len + 2 >= gnuviewpath_size)
    gnuviewpath = (char *) xrealloc (gnuviewpath,
				     (gnuviewpath_size += (2 * len) + 2));

  if (!*gnuviewpath)
    strcpy (gnuviewpath, path);
  else if (where == INFOPATH_APPEND)
    {
      strcat (gnuviewpath, COLONSTR);
      strcat (gnuviewpath, path);
    }
  else if (where == INFOPATH_APPEND_WITHOUT_COLON)
    {
      strcat (gnuviewpath, path);
    }
  else if (where == INFOPATH_PREPEND)
    {
      char *temp = strdup (gnuviewpath);
      strcpy (gnuviewpath, path);
      strcat (gnuviewpath, COLONSTR);
      strcat (gnuviewpath, temp);
      free (temp);
    }
  else if (where == INFOPATH_PREPEND_WITHOUT_COLON)
    {
      char *temp = strdup (gnuviewpath);
      strcpy (gnuviewpath, path);
      strcat (gnuviewpath, temp);
      free (temp);
    }
}

/* Return a pointer to the part of PATHNAME that simply defines the file. */
static char *gnuview_filename_non_directory (char *pathname)
{
  char *filename;

  os2_path_sep (pathname);

  filename = (char *) strrchr (pathname, PATHSEPCHR);

  if (filename) filename++;
  else          filename = pathname;

  return (filename);
}

/* Replace '/' with '\\' in the argument */
static void os2_path_sep (char *filename)
{
  char *c;

  c = filename;

  while (*c)
    {
      if ((*c) == '/')  (*c) = PATHSEPCHR;
      c++;
    }
}
