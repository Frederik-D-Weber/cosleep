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


#ifndef xml_INCLUDED
#define xml_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define XML_STRBUFLEN    4096
/* XML max element depth */
#define XML_MAX_ED         32

#define XML_ERROR_GEN      -1
#define XML_ERROR_STRLEN   -2
#define XML_ERROR_MEMBUFSZ -3
#define XML_ERROR_INV_HDL  -4
#define XML_ERROR_NOTFOUND -5
#define XML_ERROR_MALLOC   -6


struct xml_handle
{
  FILE *file;
  int encoding;
  int level;
  int offset[XML_MAX_ED];
  int one_tag[XML_MAX_ED];
  char *elementname[XML_MAX_ED];
  int elementname_sz[XML_MAX_ED];
  char *attributes[XML_MAX_ED];
  int attributes_sz[XML_MAX_ED];
  char *tag_search_result;
  int tag_search_result_sz;
  char *content[XML_MAX_ED];
  int content_sz[XML_MAX_ED];
};


struct xml_handle * xml_get_handle(const char *);
void xml_close(struct xml_handle *);
void xml_go_up(struct xml_handle *);
int xml_goto_nth_element_inside(struct xml_handle *, const char *, int);
int xml_goto_next_element_with_same_name(struct xml_handle *);
int xml_goto_next_element_at_same_level(struct xml_handle *);
void xml_goto_root(struct xml_handle *);
int xml_get_content_of_element(struct xml_handle *, char *, int);
int xml_character_encoding_type(struct xml_handle *);
int xml_get_attribute_of_element(struct xml_handle *, const char *, char *, int);
int xml_get_name_of_element(struct xml_handle *, char *, int);
void xml_fwrite_encode_entity(FILE *, const char *);
void xml_fnwrite_encode_entity(FILE *, const char *, int);
void xml_strcpy_encode_entity(char *, const char *);
int xml_strncpy_encode_entity(char *, const char *, int);
void xml_strcpy_decode_entity(char *, const char *);
int xml_strncpy_decode_entity(char *, const char *, int);

#endif












































