/* pc.c - terminal I/O calls for GNU info for OS/2
   and other PC support functions
   Kai Uwe Rommel, 1990-1993
 */

#include <stdio.h>
#include <stdlib.h>

#include <io.h>
#include <fcntl.h>
#include <string.h>

#define INCL_VIO
#include <os2emx.h>
#include <sys/kbdscan.h>

#include "pc.h"

static VIOCURSORINFO vci;

void pc_init (void)
{
  static VIOCURSORINFO vcin;

  if (_osmode == OS2_MODE)
    {
      VioGetCurType(&vci, 0);
      vcin = vci;
      vcin.yStart = 0;
      vcin.cEnd = -100;
      VioSetCurType(&vcin, 0);
    }
}

void pc_exit (void)
{
  if (_osmode == OS2_MODE)
    VioSetCurType(&vci, 0);
}

int pc_getc (int wait)
{
  int c;
  static int next;

  if (next)
    return c = next, next = 0, c;

  if ((c = _read_kbd (0, wait, 0)) == -1)
    return -1;

  if (c == 26)
    return next = 'V', 27;
  if (c != 0x00 && c != 0xE0)
    return c;

  if ((c = _read_kbd (0, wait, 0)) == -1)
    return -1;

  switch (c)
  {
  case K_F1:
    return 'H' - 64;

  case K_HOME:
    return 'A' - 64;
  case K_END:
    return 'E' - 64;

  case K_CTRL_HOME:
    return next = '<', 27;
  case K_CTRL_END:
    return next = '>', 27;

  case K_RIGHT:
    return 'F' - 64;
  case K_LEFT:
    return 'B' - 64;
  case K_DOWN:
    return 'N' - 64;
  case K_UP:
    return 'P' - 64;

  case K_CTRL_LEFT:
    return next = 'B', 27;
  case K_CTRL_RIGHT:
    return next = 'F', 27;

  case K_DEL:
  case K_PAGEUP:
    return next = 'V', 27;
  case K_PAGEDOWN:
    return 'V' - 64;

  default:
    return ' ';
  }
}
