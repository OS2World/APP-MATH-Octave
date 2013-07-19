/*
** *****************************************************************************
** list.c
** (c) Klaus Gebhardt, 1999
** *****************************************************************************
*/

#define MERLIN_STD
#define MERLIN_MEMORY
#include "../merlin.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define _MERLIN_PROPERTIES_BEGINNING      0
#define _MERLIN_PROPERTIES_INSIDE_KEY     1
#define _MERLIN_PROPERTIES_INSIDE_VALUE   2
#define _MERLIN_PROPERTIES_INSIDE_COMMENT 3


static int _merlin_properties_comparator (void *data1, long len1, void *data2, long len2)
{
  return strcmp ((char *) data1, (char *) data2);
}


MERLIN_LIST *merlin_properties ()
{
  return merlin_list (NULL, _merlin_properties_comparator);
}


long merlin_properties_save (MERLIN_LIST *list, FILE *fptr)
{
  void *p;

  char *key, *value;
  long key_len, value_len;

  long i;

  for (i = 0; i < list->entries; i++)
    {
      p = merlin_list_element_at (list, i);

      key   = (char *) p + sizeof(MERLIN_LIST_ENTRY_HEADER);
      key_len = strlen (key) + 1;

      value = (char *) p + sizeof(MERLIN_LIST_ENTRY_HEADER) + key_len;
      value_len = strlen (value) + 1;

      fprintf (fptr, "%s=%s\n", key, value);
    }

  return i;
}


long merlin_properties_load (MERLIN_LIST *list, FILE *fptr)
{
  char buffer[4096];
  int c, i = 0;
  int status = _MERLIN_PROPERTIES_BEGINNING;

  long j = 0;

  while ((c = fgetc (fptr)) != EOF)
   {
     if (c != ' ' && c != '\t' && c != '\r')
       {
	 switch (status)
	   {
	   case _MERLIN_PROPERTIES_BEGINNING:
	     if (c == '#')
	       {
		 status = _MERLIN_PROPERTIES_INSIDE_COMMENT;
	       }
	     else if (c != '\n')
	       {
		 status = _MERLIN_PROPERTIES_INSIDE_KEY;

		 i = 0;
		 buffer[i++] = c;
	       }
	     break;

	   case _MERLIN_PROPERTIES_INSIDE_COMMENT:
	     if (c == '\n')  status = _MERLIN_PROPERTIES_BEGINNING;
	     break;

	   case _MERLIN_PROPERTIES_INSIDE_KEY:
	     if (c == '=')
	       {
		 status = _MERLIN_PROPERTIES_INSIDE_VALUE;

		 buffer[i++] = 0;
	       }
	     else
	       {
		 buffer[i++] = c;
	       }
	     break;

	   case _MERLIN_PROPERTIES_INSIDE_VALUE:
	     if (c == '\n')
	       {
		 status = _MERLIN_PROPERTIES_BEGINNING;

		 buffer[i++] = 0;
		 merlin_list_add_entry (list, buffer, i);
		 j++;
	       }
	     else
	       {
		 buffer[i++] = c;
	       }
	     break;
	   }
       }
   }

  if (status == _MERLIN_PROPERTIES_INSIDE_VALUE)
    {
      buffer[i++] = 0;
      merlin_list_add_entry (list, buffer, i);
      j++;
    }

  return j;
}


void merlin_properties_set (MERLIN_LIST *list, char *key, char *value)
{
  char buffer[4096];
  long key_len;
  long value_len;

  key_len = strlen (key) + 1;
  memcpy (buffer, key, key_len);

  value_len = strlen (value) +1;
  memcpy (buffer+key_len, value, value_len);

  merlin_list_add_entry (list, buffer, key_len + value_len);
}


char *merlin_properties_get (MERLIN_LIST *list, char *key, char *default_value)
{
  void *res;

  char buffer[4096];
  long key_len;

  key_len = strlen (key) + 1;
  memcpy (buffer, key, key_len);

  res = merlin_list_find (list, buffer, key_len);

  if (!res)  return default_value;
  else       return (char *) res + sizeof(MERLIN_LIST_ENTRY_HEADER) + key_len;
}


int merlin_properties_scan (MERLIN_LIST *list, char *key, char *default_value, char *fmt, ...)
{
  int rc;
  va_list args;

  char *value;

  value = merlin_properties_get (list, key, default_value);
  if (!value) return EOF;

  va_start (args, fmt);
  rc = vsscanf (value, fmt, args);
  va_end (args);

  return rc;
}
