/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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


#include "xml.h"

//#define XMLDEBUG_TEST



static int xml_attribute(const char *, const char *, char *, int);
static int xml_process_tag(const char *, struct xml_handle *);
inline static int xml_next_tag(int, struct xml_handle *);



int xml_character_encoding_type(struct xml_handle *handle_p)
{
  if(handle_p==NULL)  return -1;

  return handle_p->encoding;
}


void xml_fwrite_encode_entity(FILE *file, const char *str)
{
  for(; *str; str++)
  {
    switch(*str)
    {
      case '<'  : fwrite("&lt;", 4, 1, file);
                  break;

      case '>'  : fwrite("&gt;", 4, 1, file);
                  break;

      case '&'  : fwrite("&amp;", 5, 1, file);
                  break;

      case '\'' : fwrite("&apos;", 6, 1, file);
                  break;

      case '\"' : fwrite("&quot;", 6, 1, file);
                  break;

      default   : fputc(*str, file);
                  break;
    }
  }
}


void xml_fnwrite_encode_entity(FILE *file, const char *str, int n)
{
  int i;


  for(i=0; i<n; i++)
  {
    switch(str[i])
    {
      case '<'  : fwrite("&lt;", 4, 1, file);
                  break;

      case '>'  : fwrite("&gt;", 4, 1, file);
                  break;

      case '&'  : fwrite("&amp;", 5, 1, file);
                  break;

      case '\'' : fwrite("&apos;", 6, 1, file);
                  break;

      case '\"' : fwrite("&quot;", 6, 1, file);
                  break;

      default   : fputc(str[i], file);
                  break;
    }
  }
}


void xml_strcpy_encode_entity(char *dest, const char *src)
{
  *dest = 0;

  for(; *src; src++)
  {
    switch(*src)
    {
      case '<'  : strcpy(dest, "&lt;");
                  dest += 4;
                  break;

      case '>'  : strcpy(dest, "&gt;");
                  dest += 4;
                  break;

      case '&'  : strcpy(dest, "&amp;");
                  dest += 5;
                  break;

      case '\'' : strcpy(dest, "&apos;");
                  dest += 6;
                  break;

      case '\"' : strcpy(dest, "&quot;");
                  dest += 6;
                  break;

      default   : *dest = *src;
                  *(++dest) = 0;
                  break;
    }
  }
}


int xml_strncpy_encode_entity(char *dest, const char *src, int n)
{
  int i=0;


  if(n < 1)
  {
    return 0;
  }

  *dest = 0;

  for(; *src; src++)
  {
    if((n - i) < 7)
    {
      break;
    }

    if(*src == '<')
    {
      strcpy(dest + i, "&lt;");
      i += 4;
      continue;
    }

    if(*src == '>')
    {
      strcpy(dest + i, "&gt;");
      i += 4;
      continue;
    }

    if(*src == '&')
    {
      strcpy(dest + i, "&amp;");
      i += 5;
      continue;
    }

    if(*src == '\'')
    {
      strcpy(dest + i, "&apos;");
      i += 6;
      continue;
    }

    if(*src == '\"')
    {
      strcpy(dest + i, "&quot;");
      i += 6;
      continue;
    }

    dest[i++] = *src;
  }

  dest[i] = 0;

  return i;
}


void xml_strcpy_decode_entity(char *dest, const char *src)
{
  int i, len, p=0;

  len = strlen(src);

  for(i=0; i<len; i++)
  {
    if((i + p) < (len - 4))
    {
      if(!strncmp(src + i + p, "&lt;", 4))
      {
        dest[i] = '<';
        p += 3;
        continue;
      }

      if(!strncmp(src + i + p, "&gt;", 4))
      {
        dest[i] = '>';
        p += 3;
        continue;
      }
    }

    if((i + p) < (len - 5))
    {
      if(!strncmp(src + i + p, "&amp;", 5))
      {
        dest[i] = '&';
        p += 4;
        continue;
      }
    }

    if((i + p) < (len - 6))
    {
      if(!strncmp(src + i + p, "&apos;", 6))
      {
        dest[i] = '\'';
        p += 5;
        continue;
      }

      if(!strncmp(src + i + p, "&quot;", 6))
      {
        dest[i] = '\"';
        p += 5;
        continue;
      }
    }

    dest[i] = src[i + p];

    if(dest[i] == 0)  break;
  }
}


int xml_strncpy_decode_entity(char *dest, const char *src, int n)
{
  int i, len, p=0;

  if(n < 1)
  {
    return 0;
  }

  len = strlen(src);

  for(i=0; i<len; i++)
  {
    if((i + p) < (len - 4))
    {
      if(!strncmp(src + i + p, "&lt;", 4))
      {
        dest[i] = '<';
        p += 3;
        continue;
      }

      if(!strncmp(src + i + p, "&gt;", 4))
      {
        dest[i] = '>';
        p += 3;
        continue;
      }
    }

    if((i + p) < (len - 5))
    {
      if(!strncmp(src + i + p, "&amp;", 5))
      {
        dest[i] = '&';
        p += 4;
        continue;
      }
    }

    if((i + p) < (len - 6))
    {
      if(!strncmp(src + i + p, "&apos;", 6))
      {
        dest[i] = '\'';
        p += 5;
        continue;
      }

      if(!strncmp(src + i + p, "&quot;", 6))
      {
        dest[i] = '\"';
        p += 5;
        continue;
      }
    }

    dest[i] = src[i + p];

    if(i >= (n - 1))
    {
      i++;

      break;
    }
  }

  return i;
}


int xml_get_name_of_element(struct xml_handle *handle_p, char *buf, int sz)
{
  if(handle_p == NULL)  return XML_ERROR_INV_HDL;

  if(sz < 2)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  strncpy(buf, handle_p->elementname[handle_p->level], sz);

  buf[sz-1] = 0;

  return 0;
}


int xml_get_attribute_of_element(struct xml_handle *handle_p, const char *attr_name, char *str_buf, int content_len)
{
  if(handle_p == NULL)  return XML_ERROR_INV_HDL;

  if(strlen(attr_name) < 1)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  if(content_len < 1)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  return xml_attribute(handle_p->attributes[handle_p->level], attr_name, str_buf, content_len);
}


static int xml_attribute(const char *data, const char *item, char *result, int result_len)
{
  int i, j,
      data_len,
      item_len,
      quote=0;

  if(data == NULL)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  data_len = strlen(data);
  item_len = strlen(item);

  if((data_len < 4) || (item_len >= (data_len - 4)))
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  for(i=0; i<(data_len - item_len); i++)
  {
    if(data[i] == '\"')
    {
      if(quote)
      {
        quote--;
      }
      else
      {
        quote++;
      }
    }

    if(quote)  continue;

    if(!strncmp(data + i, item, item_len))
    {
      if((i + item_len) < (data_len - 3))
      {
        if(!strncmp(data + i + item_len, "=\"", 2))
        {
          i += (item_len + 2);

          for(j=i; j<data_len; j++)
          {
            if(data[j] == '\"')
            {
              if((j - i) < result_len)
              {
                if(result != NULL)
                {
                  xml_strncpy_decode_entity(result, data + i, j - i);
                  result[j - i] = 0;
                }

                return j - i;
              }
            }
          }
        }
        else  continue;
      }

#ifdef XMLDEBUG_TEST
      printf("XML: error at line: %i\n", __LINE__);
#endif
      return XML_ERROR_GEN;
    }
  }

#ifdef XMLDEBUG_TEST
  printf("XML: error at line: %i\n", __LINE__);
#endif
  return XML_ERROR_NOTFOUND;
}


struct xml_handle * xml_get_handle(const char *filename)
{
  int err;

  char scratchpad[4096];

  struct xml_handle *handle_p;

  handle_p = (struct xml_handle *)calloc(1, sizeof(struct xml_handle));
  if(handle_p==NULL)  return NULL;

  handle_p->file = fopen(filename, "rb");
  if(handle_p->file==NULL)
  {
    xml_close(handle_p);
    return NULL;
  }

  handle_p->offset[handle_p->level] = xml_next_tag(0, handle_p);
  if(handle_p->offset[handle_p->level] < 0)
  {
    xml_close(handle_p);
    return NULL;
  }

  if(strlen(handle_p->tag_search_result) < 3)
  {
    xml_close(handle_p);
    return NULL;
  }

  if((handle_p->tag_search_result[0] == '?')  && (handle_p->tag_search_result[strlen(handle_p->tag_search_result) - 1] == '?'))
  {
    if(xml_attribute(handle_p->tag_search_result, "encoding", scratchpad, 4096) < 0)
    {
      handle_p->encoding = 0;  // attribute encoding not present
    }
    else
    {
      if((!strcmp(scratchpad, "ISO-8859-1")) || (!strcmp(scratchpad, "iso-8859-1")))
      {
        handle_p->encoding = 1;
      }
      else if((!strcmp(scratchpad, "UTF-8"))  || (!strcmp(scratchpad, "utf-8")))
        {
          handle_p->encoding = 2;
        }
        else
        {
          handle_p->encoding = 99999;  // unknown encoding
        }
    }
  }

  while((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
  {
    handle_p->offset[handle_p->level] = xml_next_tag(handle_p->offset[handle_p->level], handle_p);
    if(handle_p->offset[handle_p->level] < 0)
    {
      xml_close(handle_p);
      return NULL;
    }
  }

  err = xml_process_tag(handle_p->tag_search_result , handle_p);
  if(err)
  {
    xml_close(handle_p);
    return NULL;
  }

  return handle_p;
}


int xml_get_content_of_element(struct xml_handle *handle_p, char *buf, int sz)
{
  int i, j, offset, len, deep=0, cdata=0, mem_sz;

  if(handle_p==NULL)  return XML_ERROR_INV_HDL;

  offset = handle_p->offset[handle_p->level];

  len = strlen(handle_p->elementname[handle_p->level]);

  while(1)
  {
    offset = xml_next_tag(offset, handle_p);
    if(offset < 0)  return offset;

    if((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
    {
      continue;
    }

    if(handle_p->tag_search_result[0]=='/')
    {
      if(deep)  deep--;
      else  break;
    }
    else
    {
      deep++;
    }
  }

  offset -= (len + 3);

  if(offset < handle_p->offset[handle_p->level])
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  fseek(handle_p->file, handle_p->offset[handle_p->level], SEEK_SET);

  mem_sz = offset - handle_p->offset[handle_p->level] + 2;
  if(mem_sz > handle_p->content_sz[handle_p->level])
  {
    if(mem_sz < XML_STRBUFLEN)  mem_sz = XML_STRBUFLEN;
    free(handle_p->content[handle_p->level]);
    handle_p->content[handle_p->level] = (char *)malloc(mem_sz);
    if(handle_p->content[handle_p->level] == NULL)
    {
      handle_p->content_sz[handle_p->level] = 0;
      return XML_ERROR_MALLOC;
    }
    else
    {
      handle_p->content_sz[handle_p->level] = mem_sz;
    }
  }

  handle_p->content[handle_p->level][0] = 0;

  if(offset > handle_p->offset[handle_p->level])
  {
    if(fread(handle_p->content[handle_p->level], offset - handle_p->offset[handle_p->level], 1, handle_p->file) != 1)
    {
#ifdef XMLDEBUG_TEST
      printf("XML: error at line: %i\n", __LINE__);
#endif
      return XML_ERROR_GEN;
    }
  }

  len = offset - handle_p->offset[handle_p->level];

  handle_p->content[handle_p->level][len] = 0;

  if(len > 11)  // cdata?
  {
    if(!(strncmp(handle_p->content[handle_p->level], "<![CDATA[", 9)))  // cdata
    {
      cdata = 1;

      len -= 9;

      for(j=0; j<len; j++)
      {
        handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+9];
      }

      len -= 3;

      handle_p->content[handle_p->level][len] = 0;
    }
  }

  if(cdata == 0)
  {
    for(i=0; handle_p->content[handle_p->level][i]!=0; i++)
    {
      if(handle_p->content[handle_p->level][i]=='&')
      {
        if(i <= (len - 3))
        {
          if(!strncmp(handle_p->content[handle_p->level] + i + 1, "lt;", 3))
          {
            handle_p->content[handle_p->level][i] = '<';

            for(j=i+1; handle_p->content[handle_p->level][j-1]!=0; j++)
            {
              handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+3];
            }
          }

          if(!strncmp(handle_p->content[handle_p->level] + i + 1, "gt;", 3))
          {
            handle_p->content[handle_p->level][i] = '>';

            for(j=i+1; handle_p->content[handle_p->level][j-1]!=0; j++)
            {
              handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+3];
            }
          }
        }

        if(i <= (len - 5))
        {
          if(!strncmp(handle_p->content[handle_p->level] + i + 1, "apos;", 5))
          {
            handle_p->content[handle_p->level][i] = '\'';

            for(j=i+1; handle_p->content[handle_p->level][j-1]!=0; j++)
            {
              handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+5];
            }
          }

          if(!strncmp(handle_p->content[handle_p->level] + i + 1, "quot;", 5))
          {
            handle_p->content[handle_p->level][i] = '\"';

            for(j=i+1; handle_p->content[handle_p->level][j-1]!=0; j++)
            {
              handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+5];
            }
          }
        }

        if(i <= (len - 4))
        {
          if(!strncmp(handle_p->content[handle_p->level] + i + 1, "amp;", 4))
          {
            for(j=i+1; handle_p->content[handle_p->level][j-1]!=0; j++)
            {
              handle_p->content[handle_p->level][j] = handle_p->content[handle_p->level][j+4];
            }
          }
        }
      }
    }
  }

  if(sz < 2)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  strncpy(buf, handle_p->content[handle_p->level], sz);

  buf[sz-1] = 0;

  return 0;
}


int xml_goto_nth_element_inside(struct xml_handle *handle_p, const char *name, int n)
{
  int len, offset, deep=0, cnt=0, ts_len, has_endslash, err;

  if(handle_p==NULL)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_INV_HDL;
  }

  if(handle_p->level >= (XML_MAX_ED - 2))
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_MEMBUFSZ;
  }

  len = strlen(name);

  offset = handle_p->offset[handle_p->level];

  while(1)
  {
    while(1)
    {
      offset = xml_next_tag(offset, handle_p);
      if(offset < 0)  return offset;

      if((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
      {
        continue;
      }

      if(handle_p->tag_search_result[0]=='/')
      {
        if(deep)
        {
          deep--;
        }
        else
        {
#ifdef XMLDEBUG_TEST
          printf("XML: error at line: %i\n", __LINE__);
#endif
          return XML_ERROR_NOTFOUND;
        }
      }
      else
      {
        ts_len = strlen(handle_p->tag_search_result);

        if(handle_p->tag_search_result[ts_len - 1] == '/')
        {
          has_endslash = 1;
        }
        else
        {
          has_endslash = 0;

          deep++;
        }

        break;
      }
    }

    if((deep + has_endslash) == 1)
    {
      if((int)strlen(handle_p->tag_search_result) >= len)
      {
        if(!strncmp(handle_p->tag_search_result, name, len))
        {
          if((handle_p->tag_search_result[len]==' ') ||
             (handle_p->tag_search_result[len]==0) ||
             (handle_p->tag_search_result[len]=='\r') ||
             (handle_p->tag_search_result[len]=='\n'))
          {
            if(cnt==n)
            {
              handle_p->level++;
              handle_p->offset[handle_p->level] = offset;
              handle_p->one_tag[handle_p->level] = 0;

              err = xml_process_tag(handle_p->tag_search_result, handle_p);
              if(err)  return err;

              return 0;
            }

            cnt++;
          }
        }
      }
    }
  }

#ifdef XMLDEBUG_TEST
  printf("XML: error at line: %i\n", __LINE__);
#endif
  return XML_ERROR_GEN;
}


int xml_goto_next_element_with_same_name(struct xml_handle *handle_p)
{
  int len, offset, deep=0, err, ts_len, has_endslash;

  if(handle_p==NULL)  return XML_ERROR_INV_HDL;

  len = strlen(handle_p->elementname[handle_p->level]);
  offset = handle_p->offset[handle_p->level];

  if(handle_p->one_tag[handle_p->level] == 0)
  {
    while(1)
    {
      offset = xml_next_tag(offset, handle_p);
      if(offset < 0)
      {
#ifdef XMLDEBUG_TEST
        printf("XML: error at line: %i\n", __LINE__);
#endif
        return XML_ERROR_GEN;
      }

      if((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
      {
        continue;
      }

      if(handle_p->tag_search_result[0]=='/')
      {
        if(deep)  deep--;
        else  break;
      }
      else
      {
        ts_len = strlen(handle_p->tag_search_result);

        if(handle_p->tag_search_result[ts_len - 1] != '/')
        {
          deep++;
        }
      }
    }
  }

  while(1)
  {
    while(1)
    {
      offset = xml_next_tag(offset, handle_p);
      if(offset < 0)  return offset;

      if((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
      {
        continue;
      }

      if(handle_p->tag_search_result[0]=='/')
      {
        if(deep)  deep--;
        else
        {
#ifdef XMLDEBUG_TEST
          printf("XML: error at line: %i\n", __LINE__);
#endif
          return XML_ERROR_GEN;
        }
      }
      else
      {
        ts_len = strlen(handle_p->tag_search_result);

        if(handle_p->tag_search_result[ts_len - 1] == '/')
        {
          has_endslash = 1;
        }
        else
        {
          has_endslash = 0;

          deep++;
        }

        break;
      }
    }

    if((deep + has_endslash) == 1)
    {
      if((int)strlen(handle_p->tag_search_result) >= len)
      {
        if(!strncmp(handle_p->tag_search_result, handle_p->elementname[handle_p->level], len))
        {
          if((handle_p->tag_search_result[len]==' ') ||
             (handle_p->tag_search_result[len]==0) ||
             (handle_p->tag_search_result[len]=='\r') ||
             (handle_p->tag_search_result[len]=='\n'))
          {
            handle_p->offset[handle_p->level] = offset;
            handle_p->one_tag[handle_p->level] = 0;

            err = xml_process_tag(handle_p->tag_search_result, handle_p);
            if(err)  return err;

            return 0;
          }
        }
      }
    }
  }

#ifdef XMLDEBUG_TEST
  printf("XML: error at line: %i\n", __LINE__);
#endif
  return XML_ERROR_GEN;
}


int xml_goto_next_element_at_same_level(struct xml_handle *handle_p)
{
  int offset, deep=0, ts_len, err;

  if(handle_p==NULL)  return XML_ERROR_INV_HDL;

  offset = handle_p->offset[handle_p->level];

  while(1)
  {
    offset = xml_next_tag(offset, handle_p);
    if(offset < 0)  return offset;

    if((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
    {
      continue;
    }

    if(handle_p->tag_search_result[0]=='/')
    {
      if(deep)  deep--;
      else  break;
    }
    else
    {
      ts_len = strlen(handle_p->tag_search_result);

      if(handle_p->tag_search_result[ts_len - 1] != '/')
      {
        deep++;
      }
    }
  }

  offset = xml_next_tag(offset, handle_p);
  if(offset < 0)  return offset;

  while((handle_p->tag_search_result[0]=='!') || (handle_p->tag_search_result[0]=='?'))
  {
    offset = xml_next_tag(offset, handle_p);
    if(offset < 0)  return offset;
  }

  if(handle_p->tag_search_result[0]=='/')
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  err = xml_process_tag(handle_p->tag_search_result, handle_p);
  if(err)  return err;

  handle_p->offset[handle_p->level] = offset;

  return 0;
}


/* Read the element name and copy it into the field of the structure */
/* Read the attributes and copy them into the field of the structure */
/* Check if the element has one or two tags */
static int xml_process_tag(const char *str, struct xml_handle *handle_p)
{
  int len, i, p, mem_sz;

  if(handle_p==NULL)  return XML_ERROR_INV_HDL;

  len = strlen(str);
  if(!len)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  if((str[0]==' ')||(str[0]=='>'))
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  for(i=0; i<len; i++)
  {
    if((str[i]==' ')||(str[i]=='>')||(str[i]=='\n')||(str[i]=='\r'))  break;
  }

  mem_sz = i + 2;
  if(mem_sz > handle_p->elementname_sz[handle_p->level])
  {
    if(mem_sz < XML_STRBUFLEN)  mem_sz = XML_STRBUFLEN;
    free(handle_p->elementname[handle_p->level]);
    handle_p->elementname[handle_p->level] = (char *)malloc(mem_sz);
    if(handle_p->elementname[handle_p->level] == NULL)
    {
      handle_p->elementname_sz[handle_p->level] = 0;
      return XML_ERROR_MALLOC;
    }
    else
    {
      handle_p->elementname_sz[handle_p->level] = mem_sz;
    }
  }

  handle_p->elementname[handle_p->level][0] = 0;

  strncpy(handle_p->elementname[handle_p->level], str, i);
  handle_p->elementname[handle_p->level][i] = 0;

  while((str[i]=='\n')||(str[i]=='\r'))  i++;

  if(str[i]!=' ')  return 0;

  i++;

  p = i;

  for(; i<len; i++)
  {
    if(str[i]=='>')  break;
  }

  if(str[i-1] == '/')
  {
    i--;

    handle_p->one_tag[handle_p->level] = 1;
  }
  else
  {
    handle_p->one_tag[handle_p->level] = 0;
  }

  len = i - p;

  if(len < 2)  return 0;

  if(str[len] == '/')  len--;

  mem_sz = len + 2;
  if(mem_sz > handle_p->attributes_sz[handle_p->level])
  {
    if(mem_sz < XML_STRBUFLEN)  mem_sz = XML_STRBUFLEN;
    free(handle_p->attributes[handle_p->level]);
    handle_p->attributes[handle_p->level] = (char *)malloc(mem_sz);
    if(handle_p->attributes[handle_p->level] == NULL)
    {
      handle_p->attributes_sz[handle_p->level] = 0;
      return XML_ERROR_MALLOC;
    }
    else
    {
      handle_p->attributes_sz[handle_p->level] = mem_sz;
    }
  }

  handle_p->attributes[handle_p->level][0] = 0;

  strncpy(handle_p->attributes[handle_p->level], str + p, len);
  handle_p->attributes[handle_p->level][len] = 0;

  return 0;
}


void xml_close(struct xml_handle *handle_p)  /* delete everything and close the file */
{
  int i;

  if(handle_p==NULL)  return;

  if(handle_p->file != NULL)  fclose(handle_p->file);

  free(handle_p->tag_search_result);

  for(i=0; i<XML_MAX_ED; i++)
  {
    free(handle_p->elementname[i]);
    free(handle_p->attributes[i]);
    free(handle_p->content[i]);
  }

  free(handle_p);
}


void xml_goto_root(struct xml_handle *handle_p) /* go to rootlevel and delete everything beneath */
{
  if(handle_p==NULL)  return;

  handle_p->level = 0;
}


void xml_go_up(struct xml_handle *handle_p) /* go one level up and delete everything beneath */
{
  if(handle_p==NULL)  return;

  if(handle_p->level > 0)  handle_p->level--;

  return;
}


inline static int xml_next_tag(int offset, struct xml_handle *handle_p) /* returns offset after '>' */
{
  int temp, fp1=0, fp2=0, tagstart=0,
      tag_char_cnt=0,
      comment=0,
      cdata=0,
      mem_sz;

  char circ_buf[16];


  fseek(handle_p->file, offset, SEEK_SET);

  while(1)
  {
    temp = fgetc(handle_p->file);
    if(temp==EOF)
    {
#ifdef XMLDEBUG_TEST
      printf("XML: error at line: %i\n", __LINE__);
#endif
      return XML_ERROR_GEN;
    }

    if(tagstart)
    {
      circ_buf[tag_char_cnt % 16] = temp;

      tag_char_cnt++;

      if(tag_char_cnt == 3)
      {
        if(!strncmp(circ_buf, "!--", 3))    // start of comment
        {
          comment = 1;
        }
      }

      if(tag_char_cnt == 8)
      {
        if(!strncmp(circ_buf, "![CDATA[", 8))    // start of cdata
        {
          cdata = 1;
        }
      }
    }

    if(!comment)
    {
      if(!cdata)
      {
        if(temp=='<')
        {
          if(tagstart)
          {
#ifdef XMLDEBUG_TEST
            printf("XML: error at line: %i\n", __LINE__);
#endif
            return XML_ERROR_GEN;
          }

          tagstart = 1;
          fp1 = ftell(handle_p->file);
        }

        if(temp=='>')
        {
          if(!tagstart)
          {
#ifdef XMLDEBUG_TEST
            printf("XML: error at line: %i\n", __LINE__);
#endif
            return XML_ERROR_GEN;
          }

          offset = ftell(handle_p->file);
          fp2 = offset - 1;

          break;
        }
      }
      else  // cdata
      {
        if((temp=='>') && (tag_char_cnt > 10))
        {
          if((circ_buf[(tag_char_cnt - 2) % 16] == ']') && (circ_buf[(tag_char_cnt - 3) % 16] == ']'))
          {  // end of cdata
            cdata = 0;
            tagstart = 0;
            fp1 = 0;
            fp2 = 0;
            tag_char_cnt = 0;
          }
        }
      }
    }
    else  // comment
    {
      if((temp=='>') && (tag_char_cnt > 5))
      {
        if((circ_buf[(tag_char_cnt - 2) % 16] == '-') && (circ_buf[(tag_char_cnt - 3) % 16] == '-'))
        {  // end of comment
          comment = 0;
          tagstart = 0;
          fp1 = 0;
          fp2 = 0;
          tag_char_cnt = 0;
        }
      }
    }
  }

  if(!fp2)
  {
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  fseek(handle_p->file, fp1, SEEK_SET);

  mem_sz = fp2 - fp1 + 2;
  if(mem_sz > handle_p->tag_search_result_sz)
  {
    if(mem_sz < XML_STRBUFLEN)  mem_sz = XML_STRBUFLEN;
    free(handle_p->tag_search_result);
    handle_p->tag_search_result = (char *)malloc(mem_sz);
    if(handle_p->tag_search_result == NULL)
    {
      handle_p->tag_search_result_sz = 0;
#ifdef XMLDEBUG_TEST
      printf("XML: error at line: %i\n", __LINE__);
#endif
      return XML_ERROR_MALLOC;
    }
    else
    {
      handle_p->tag_search_result_sz = mem_sz;
    }
  }

  handle_p->tag_search_result[0] = 0;

  if(fread(handle_p->tag_search_result, fp2 - fp1, 1, handle_p->file) != 1)
  {
    handle_p->tag_search_result[0] = 0;
#ifdef XMLDEBUG_TEST
    printf("XML: error at line: %i\n", __LINE__);
#endif
    return XML_ERROR_GEN;
  }

  handle_p->tag_search_result[fp2 - fp1] = 0;

  return offset;
}












