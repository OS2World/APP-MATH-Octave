/*
** ****************************************************************************
** _inf und _nan
** (c) 1995, Klaus Gebhardt
** ****************************************************************************
*/

static unsigned int _isnan_ = 2146959360;
static unsigned int _isinf_ = 2146435072;

double _merlin_inf(void)
{
  double r;
  unsigned int *p;

  p = (unsigned int *) &r;
  p[0] = 0;
  p[1] = _isinf_;

  return r;
}

double _merlin_nan(void)
{
  double r;
  unsigned int *p;

  p = (unsigned int *) &r;
  p[0] = 0;
  p[1] = _isnan_;

  return r;
}
