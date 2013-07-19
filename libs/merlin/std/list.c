/*
** *****************************************************************************
** list.c
** (c) Klaus Gebhardt, 1999
** *****************************************************************************
*/

#define MERLIN_STD
#define MERLIN_MEMORY
#include "../merlin.h"

#include <memory.h>
#include <string.h>

#define _MERLIN_ENTRY_HEADER_LEN sizeof(MERLIN_LIST_ENTRY_HEADER)


MERLIN_LIST *merlin_list (int (filter)(MERLIN_LIST *, void *, long),
			  int (comparator)(void *, long, void *, long))
{
  MERLIN_LIST *list = (MERLIN_LIST *) xmalloc (sizeof(MERLIN_LIST));

  list->entries = 0;

  list->filter = filter;
  list->comparator = comparator;

  list->array = NULL;
  list->first = NULL;

  return list;
}


long merlin_list_array (MERLIN_LIST *list)
{
  long i;

  if (list->array)  free (list->array);
  list->array = NULL;

  if (list->entries == 0)  return 0;

  list->array = (void **) xmalloc (list->entries * sizeof (void *));

  list->array[0] = list->first;

  for (i = 1; i < list->entries; i++)
    list->array[i] = *((void **) list->array[i-1]);

  return i;
}


long merlin_list_add_entry (MERLIN_LIST *list, void *data, long data_len)
{
  void *p, *q , *entry;
  long q_data_len;
  void *q_data;
  int c;

  if (list->filter)
    {
      c = ((int (*)(MERLIN_LIST *, void *, long)) (list->filter))(list, data, data_len);
      if (c == 0)  return -1;
    }

  entry = xmalloc (_MERLIN_ENTRY_HEADER_LEN + data_len);
  ((MERLIN_LIST_ENTRY_HEADER *) entry)->data_len = data_len;
  memcpy (entry+_MERLIN_ENTRY_HEADER_LEN, data, data_len);

  p = (void *) list;
  q = ((MERLIN_LIST_ENTRY_HEADER *) p)->next;

  while (q != NULL)
    {
      q_data_len = ((MERLIN_LIST_ENTRY_HEADER *) q)->data_len;
      q_data = q+_MERLIN_ENTRY_HEADER_LEN;

      if (list->comparator)
	{
	  c = ((int (*)(void *, long, void *, long)) (list->comparator))
	    (data, data_len, q_data, q_data_len);

	  if (c < 0)        goto insert_entry;
	  else if (c == 0)  goto replace_entry;
	}

      p = q;
      q = ((MERLIN_LIST_ENTRY_HEADER *) p)->next;
    }

insert_entry:
  ((MERLIN_LIST_ENTRY_HEADER *) entry)->next = q;
  ((MERLIN_LIST_ENTRY_HEADER *) p)->next = entry;

  if (list->array)  free (list->array);
  list->array = NULL;

  list->entries++;
  return 1;

replace_entry:
  ((MERLIN_LIST_ENTRY_HEADER *) entry)->next = ((MERLIN_LIST_ENTRY_HEADER *) q)->next;;
  ((MERLIN_LIST_ENTRY_HEADER *) p)->next = entry;

  if (list->array)  free (list->array);
  list->array = NULL;

  free (q);
  return 0;
}


long merlin_list_iterate (MERLIN_LIST *list, int (func)(void *, long))
{
  long i = 0;

  void *q;
  int c;

  long q_data_len;
  void *q_data;

  q = list->first;

  while (q != NULL)
    {
      i++;

      q_data_len = ((MERLIN_LIST_ENTRY_HEADER *) q)->data_len;
      q_data = q+_MERLIN_ENTRY_HEADER_LEN;

      c = (func) (q_data, q_data_len);
      if (c != 0)  return i;

      q = *((void **) q);
    }

  return i;
}


void *merlin_list_find (MERLIN_LIST *list, void *search, long search_len)
{
  void *q;
  int c;

  long q_data_len;
  void *q_data;

  q = list->first;

  while (q != NULL)
    {
      q_data_len = ((MERLIN_LIST_ENTRY_HEADER *) q)->data_len;
      q_data = q+_MERLIN_ENTRY_HEADER_LEN;

      if (list->comparator)
	{
	  c = ((int (*)(void *, long, void *, long)) (list->comparator))
	    (search, search_len, q_data, q_data_len);

	  if (c == 0)      return q;
	  else if (c < 0)  return NULL;
	}
      else
	{
	  if ((q_data_len == search_len) && (strcmp ((char *) q_data, (char *) search) == 0))
	    return q;
	}

      q = *((void **) q);
    }

  return NULL;
}


long merlin_list_index_of (MERLIN_LIST *list, void *search, long search_len)
{
  long i;

  void *q;
  int c;

  long q_data_len;
  void *q_data;

  q = list->first;

  for (i = 0; i < list->entries; i++)
    {
      q_data_len = ((MERLIN_LIST_ENTRY_HEADER *) q)->data_len;
      q_data = q+_MERLIN_ENTRY_HEADER_LEN;

      if (list->comparator)
	{
	  c = ((int (*)(void *, long, void *, long)) (list->comparator))
	    (search, search_len, q_data, q_data_len);

	  if (c == 0)      return i;
	  else if (c < 0)  return -1;
	}
      else
	{
	  if ((q_data_len == search_len) && (strcmp ((char *) q_data, (char *) search) == 0))
	    return i;
	}

      q = *((void **) q);
    }

  return -1;
}


void *merlin_list_element_at (MERLIN_LIST *list, long i)
{
  if (i < 0 || i >= list->entries)  return NULL;

  if (list->array)
    {
      return list->array[i];
    }
  else
    {
      long j;
      void *q;

      q = list->first;
      for (j = 0; j < i; j++)  q = *((void **) q);

      return q;
    }
}
