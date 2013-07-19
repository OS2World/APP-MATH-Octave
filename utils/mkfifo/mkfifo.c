/*
** ****************************************************************************
** This implements the mkfifo command under OS/2.
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

#include "getopt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MERLIN_PROCESS
#include <merlin.h>

static int delete_nmp_p    = 0;
static int exit_server_p   = 0;
static int list_nmp_p      = 0;
static char *mode_string   = NULL;

/*
** Structure describing the options that mkfifo accepts.  We pass this
** structure to getopt_long ().  If you add or otherwise change this
** structure, you must also change the string which follows it.
*/

static struct option long_options[] =
{
  { "delete",  0, 0, 'd' },
  { "exit",    0, 0, 'x' },
  { "help",    0, 0, 'h' },
  { "list",    0, 0, 'l' },
  { "mode",    1, 0, 'm' },
  { "version", 0, 0, 'v' },
  {NULL, 0, NULL, 0}
};

/* String describing the shorthand versions of the long options found above. */
static char *short_options = "m:dxhlv?";


static void print_help ()
{
  fprintf (stderr, "\nmkfifo for OS/2, Version 1.0\n"
	   "usage: mkfifo [options] named_pipe ...\n\n"
	   "[options]       --delete,  -d\n"
	   "                --exit,    -x\n"
	   "                --help,    -h\n"
	   "                --list,    -l\n"
	   "                --mode,    -m\n"
	   "                --version, -v\n");
  exit (0);
}


static void print_version ()
{
  fprintf (stderr, "\nmkfifo for OS/2, Version 1.0");
  exit (0);
}


static void usage ()
{
  fprintf (stderr, "\nmkfifo for OS/2, Version 1.0\n"
"usage: mkfifo [options] named_pipe ...\nwith the folloing options:\n"
"--delete, -d, --exit, -x, --help, -h, --list, -l, --mode, -m, --version, -v");
  exit (0);
}


int main (int argc, char *argv[])
{
  int getopt_long_index;
  int option_character;
  int rc;

  while (1)
    {
      option_character = getopt_long
        (argc, argv, short_options, long_options, &getopt_long_index);

      if (option_character == EOF) break;

      if (option_character == 0 && long_options[getopt_long_index].flag == 0)
        option_character = long_options[getopt_long_index].val;

      switch (option_character)
        {
        case 0:
          break;

        case 'd':
	  delete_nmp_p = 1;
          break;

        case 'h':
	  print_help ();
          break;

        case 'l':
          list_nmp_p = 1;
          break;

        case 'v':
	  print_version ();
          break;

        case 'x':
	  exit_server_p = 1;
          break;

        case 'm':
	  if (mode_string)  free (mode_string);
          mode_string = strdup (optarg);
          break;

        default:
          usage ();
        }
    }

  if ((optind == argc) && !list_nmp_p && !exit_server_p)
    usage ();

  while (optind != argc)
    {
      if (delete_nmp_p)  rc = merlin_mkfifo_remove (argv[optind++]);
      else               rc = merlin_mkfifo (argv[optind++], 0);
    }

  if (list_nmp_p)     rc = merlin_mkfifo_list ();

  if (exit_server_p)  rc = merlin_mkfifo_exit ();

  return 0;
}
