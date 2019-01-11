/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
*
* Email: teuniz@gmail.com
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/



#include "edf_annot_list.h"


#define EDF_ANNOT_MEMBLOCKSZ_II   1000

#define TIME_DIMENSION_II (10000000LL)



int edfplus_annotation_add_item(struct annotation_list *list, struct annotationblock annotation)
{
  struct annotationblock *tmp_block_list;

  int *tmp_idx;

  if(list == NULL)  return -1;

  if(list->used_sz >= list->mem_sz)
  {
    tmp_block_list = (struct annotationblock *)realloc(list->items, (list->mem_sz + EDF_ANNOT_MEMBLOCKSZ_II) * sizeof(struct annotationblock));
    if(tmp_block_list == NULL)
    {
      return -2;
    }

    list->items = tmp_block_list;

    tmp_idx = (int *)realloc(list->idx, (list->mem_sz + EDF_ANNOT_MEMBLOCKSZ_II) * sizeof(int));
    if(tmp_idx == NULL)
    {
      return -3;
    }

    list->idx = tmp_idx;

    list->mem_sz += EDF_ANNOT_MEMBLOCKSZ_II;
  }

  list->items[list->used_sz] = annotation;

  list->idx[list->sz] = list->used_sz;

  list->sz++;

  list->used_sz++;

  return 0;
}


void edfplus_annotation_remove_item(struct annotation_list *list, int n)
{
  int i;

  if(list == NULL)  return;

  if((n >= list->sz) || (n < 0) || (list->sz < 1))  return;

  list->sz--;

  for(i=n; i<list->sz; i++)
  {
    list->idx[i] = list->idx[i+1];
  }
}


int edfplus_annotation_size(struct annotation_list *list)
{
  if(list == NULL)  return 0;

  return list->sz;
}


int edfplus_annotation_cnt(struct annotation_list *list, struct annotationblock *annot)
{
  int i, n=0;

  struct annotationblock *tmp_annot;

  for(i=0; i<list->sz; i++)
  {
    tmp_annot = &list->items[list->idx[i]];

    if(!strcmp(tmp_annot->annotation, annot->annotation))
    {
      if(!tmp_annot->hided_in_list)  n++;
    }
  }

  return n;
}


void edfplus_annotation_empty_list(struct annotation_list *list)
{
  if(list == NULL)  return;

  free(list->items);
  free(list->idx);
  list->items = NULL;
  list->idx = NULL;
  list->sz = 0;
  list->mem_sz = 0;
  list->used_sz = 0;
}


int edfplus_annotation_get_index(struct annotation_list *list, struct annotationblock *annot)
{
  int i;

  if(list == NULL)  return -2;

  for(i=0; i<list->sz; i++)
  {
    if(&list->items[list->idx[i]] == annot)
    {
      return i;
    }
  }

  return -1;
}


struct annotationblock * edfplus_annotation_get_item(struct annotation_list *list, int n)
{
  if(list == NULL)  return NULL;

  if((n >= list->sz) || (n < 0))  return NULL;

  return &list->items[list->idx[n]];
}


struct annotationblock * edfplus_annotation_get_item_visible_only(struct annotation_list *list, int n)
{
  int i, j=0;

  if(list == NULL)  return NULL;

  if((n >= list->sz) || (n < 0))  return NULL;

  for(i=0; i<list->sz; i++)
  {
    if(list->items[list->idx[i]].hided_in_list)
    {
      continue;
    }

    if(n == j++)
    {
      return &list->items[list->idx[i]];
    }
  }

  return NULL;
}


struct annotationblock * edfplus_annotation_get_item_visible_only_cached(struct annotation_list *list, int n, int *i, int *j)
{
  if(list == NULL)  return NULL;

  if((n >= list->sz) || (n < 0))  return NULL;

  for(; *i<list->sz; (*i)++)
  {
    if(list->items[list->idx[*i]].hided_in_list)
    {
      continue;
    }

    if(n == (*j)++)
    {
      return &list->items[list->idx[(*i)++]];
    }
  }

  return NULL;
}


int edfplus_annotation_get_index_at(struct annotation_list *list, long long onset, int start)
{
  int i;

  if(list == NULL)  return -1;

  if(start < 0)  return -1;

  for(i=start; i<list->sz; i++)
  {
    if(list->items[list->idx[i]].onset >= onset)
    {
      return i;
    }
  }

  return -1;
}


void edfplus_annotation_sort(struct annotation_list *list, void (*callback)(void))  /* sort the annotationlist based on the onset time */
{
  int i, j, p, idx;

  long long onset;

  if(list == NULL)  return;

  for(i=0; i<(list->sz - 1); i++)
  {
    if(!(i % 1000))
    {
      if(callback != NULL)
      {
        callback();
      }
    }

    if(list->items[list->idx[i+1]].onset >= list->items[list->idx[i]].onset)
    {
      continue;
    }

    idx = list->idx[i+1];

    onset = list->items[idx].onset;

    for(j=i/2, p=i; (p-j)>0; )
    {
      if(onset < list->items[list->idx[j]].onset)
      {
        p = j;

        j /= 2;
      }
      else
      {
        if((p-j) == 1)
        {
          j++;

          break;
        }

        j = (j + p) / 2;
      }
    }

    memmove(list->idx + j + 1, list->idx + j, (i - j + 1) * sizeof(int));

    list->idx[j] = idx;
  }
}


struct annotation_list * edfplus_annotation_create_list_copy(struct annotation_list *list)
{
  struct annotation_list *cpy;

  if(list == NULL)  return NULL;

  cpy = (struct annotation_list *)calloc(1, sizeof(struct annotation_list));
  if(cpy == NULL)
  {
    return NULL;
  }

  if(list->sz > 0)
  {
    cpy->items = (struct annotationblock *)calloc(1, sizeof(struct annotationblock) * list->mem_sz);
    if(cpy->items == NULL)
    {
      free(cpy);
      return NULL;
    }

    cpy->idx = (int *)calloc(1, sizeof(int) * list->mem_sz);
    if(cpy->idx == NULL)
    {
      free(cpy->items);
      free(cpy);
      return NULL;
    }

    cpy->mem_sz = list->mem_sz;

    cpy->sz = list->sz;

    cpy->used_sz = list->used_sz;

    memcpy(cpy->items, list->items, cpy->mem_sz * sizeof(struct annotationblock));

    memcpy(cpy->idx, list->idx, cpy->mem_sz * sizeof(int));
  }

  return cpy;
}


void edfplus_annotation_copy_list(struct annotation_list *destlist, struct annotation_list *srclist)
{
  if((destlist == NULL) || (srclist == NULL)) return;

  if(destlist->sz > 0)
  {
    free(destlist->items);

    destlist->items = NULL;

    free(destlist->idx);

    destlist->idx = NULL;

    destlist->sz = 0;

    destlist->mem_sz = 0;

    destlist->used_sz = 0;
  }

  destlist->items = (struct annotationblock *)calloc(1, srclist->mem_sz * sizeof(struct annotationblock));
  if(destlist->items == NULL)
  {
    return;
  }

  destlist->idx = (int *)calloc(1, srclist->mem_sz * sizeof(int));
  if(destlist->idx == NULL)
  {
    free(destlist->items);
    destlist->items = NULL;
    return;
  }

  destlist->sz = srclist->sz;

  destlist->used_sz = srclist->used_sz;

  destlist->mem_sz = srclist->mem_sz;

  memcpy(destlist->items, srclist->items, destlist->mem_sz * sizeof(struct annotationblock));

  memcpy(destlist->idx, srclist->idx, destlist->mem_sz * sizeof(int));
}


int edfplus_annotation_remove_duplicates(struct annotation_list *list)
{
  int i, j, dup_cnt=0;

  struct annotationblock *annot;

  if(list == NULL)  return -1;

  if(list->sz < 2)  return 0;

  for(i=0; i<(list->sz - 1); i++)
  {
    annot = &list->items[list->idx[i]];

    if(annot->file_num != list->items[list->idx[i + 1]].file_num)  continue;

    if(annot->onset != list->items[list->idx[i + 1]].onset)  continue;

    if(strcmp(annot->annotation, list->items[list->idx[i + 1]].annotation))  continue;

    if(strcmp(annot->duration, list->items[list->idx[i + 1]].duration))  continue;

    list->sz--;

    for(j=i; j<(list->sz); j++)
    {
      list->idx[j] = list->idx[j+1];
    }

    dup_cnt++;
  }

  return dup_cnt;
}


int edfplus_annotation_get_max_annotation_strlen(struct annotation_list *list)
{
  int i, j,
      len,
      annot_descr_len=0,
      annot_duration_len=0,
      timestamp_digits=0,
      timestamp_decimals=0;

  char scratchpad[1024];

  struct annotationblock *annot;

  if(list == NULL)
  {
    return 0;
  }

  for(i=0; i<list->sz; i++)
  {
    annot = &list->items[list->idx[i]];
#ifdef _WIN32
    len = __mingw_snprintf(scratchpad, 256, "%lli", annot->onset / TIME_DIMENSION_II);
#else
    len = snprintf(scratchpad, 256, "%lli", annot->onset / TIME_DIMENSION_II);
#endif

    if(len > timestamp_digits)
    {
      timestamp_digits = len;
    }

    j = 10;

    for(len=7; len>0; len--)
    {
      if(annot->onset % j)
      {
        break;
      }

      j *= 10;
    }

    if(len > timestamp_decimals)
    {
      timestamp_decimals = len;
    }

    len = strlen(annot->annotation);

    if(len>annot_descr_len)
    {
      annot_descr_len = len;
    }

    len = strlen(annot->duration);

    if(len>annot_duration_len)
    {
      annot_duration_len = len;
    }
  }

  if(annot_duration_len)
  {
    annot_duration_len++;
  }

  if(timestamp_decimals)
  {
    timestamp_decimals++;
  }

// printf("annot_descr_len is %i\n"
//        "annot_duration_len is %i\n"
//        "timestamp_digits is %i\n"
//        "timestamp_decimals is %i\n",
//        annot_descr_len,
//        annot_duration_len,
//        timestamp_digits,
//        timestamp_decimals);

  return annot_descr_len + annot_duration_len + timestamp_digits + timestamp_decimals + 4;
}


long long edfplus_annotation_get_long_from_number(const char *str)
{
  int i, len, hasdot=0, dotposition=0;

  long long value=0, radix;

  if((str[0] == '+') || (str[0] == '-'))
  {
    str++;
  }

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      hasdot = 1;
      dotposition = i;
      break;
    }
  }

  if(hasdot)
  {
    radix = TIME_DIMENSION_II;

    for(i=dotposition-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }

    radix = TIME_DIMENSION_II / 10;

    for(i=dotposition+1; i<len; i++)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix /= 10;
    }
  }
  else
  {
    radix = TIME_DIMENSION_II;

    for(i=len-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  if(str[-1]=='-')  value = -value;

  return value;
}






















