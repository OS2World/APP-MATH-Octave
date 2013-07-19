// Memory allocation test.

#include <iostream.h>
#include <sys/uflags.h>

char *char_alloc (long);

int main(void)
{
  _uflags (_UF_SBRK_MODEL, _UF_SBRK_ARBITRARY);

  char *s = char_alloc (34l * 1024l * 1024l);

  if (s)
    {
      cout << s << endl;
      delete s;
    }

  return 0;
}

char *char_alloc (long size)
{
  char *s = new char [size];

  if (s)  strcpy (s, "new erfolgreich ...");

  return s;
}

