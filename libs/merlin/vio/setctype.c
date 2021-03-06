#define INCL_VIO
#include <os2.h>

#define MERLIN_VIO
#include "../merlin.h"

void merlin_setctype (int cur_t)
{
  VIOCURSORINFO CD;
  VIOMODEINFO MD;

  int height;

  MD.cb = sizeof (MD);
  VioGetMode(&MD, 0);
  height = MD.vres/MD.row;
  CD.cx = 0;

  switch (cur_t)
    {
    case MERLIN_NOCURSOR:
      CD.attr   = 0xffff;
      CD.yStart = CD.cEnd = 0;
      break;

    case MERLIN_SOLIDCURSOR:
      CD.attr   = 1;
      CD.yStart = 0;
      CD.cEnd   = height - 1;
      break;

    case MERLIN_NORMALCURSOR:
    default:
      CD.attr   = 1;
      CD.yStart = height <= 8 ? height - 1 : height - 2;
      CD.cEnd   = height - 1;
    }

  VioSetCurType(&CD, 0);
}
