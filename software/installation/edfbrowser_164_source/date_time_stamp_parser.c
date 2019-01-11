/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utc_date_time.h"
#include "date_time_stamp_parser.h"


#define DATE_TIME_STAMP_LEN     20



long long parse_date_time_stamp(const char *src)
{
  int i, len, probes;

  char str[1024],
       *ptr;

  len = strlen(src);

  if(len < DATE_TIME_STAMP_LEN + 4)  goto ERROR;

  strcpy(str, src);

  len = strlen(str);

  if(len < DATE_TIME_STAMP_LEN)  goto ERROR;

  probes = len - DATE_TIME_STAMP_LEN + 1;

  for(i=0; i<probes; i++)
  {
    ptr = str + i;

    if((ptr[2] != '-') || (ptr[6] != '-') || (ptr[11] != '_') || (ptr[14] != 'h') || (ptr[17] != 'm'))  continue;

    if((ptr[0] < '0') || (ptr[0] > '9') || (ptr[1] < '0') || (ptr[1] > '9') ||
       (ptr[7] < '0') || (ptr[7] > '9') || (ptr[8] < '0') || (ptr[8] > '9') ||
       (ptr[9] < '0') || (ptr[9] > '9') || (ptr[10] < '0') || (ptr[10] > '9'))  continue;

    if((ptr[12] < '0') || (ptr[12] > '9') || (ptr[13] < '0') || (ptr[13] > '9') ||
       (ptr[15] < '0') || (ptr[15] > '9') || (ptr[16] < '0') || (ptr[16] > '9') ||
       (ptr[18] < '0') || (ptr[18] > '9') || (ptr[19] < '0') || (ptr[19] > '9'))  continue;

    break;
  }

  if(i == probes)  goto ERROR;

/*
 * We are looking for a string formatted like this:
 * 03-MAY-2013_14h45m49
 *
 */

  ptr[DATE_TIME_STAMP_LEN] = 0;
  ptr[2] = 0;
  ptr[6] = 0;
  ptr[11] = 0;
  ptr[14] = 0;
  ptr[17] = 0;

  struct date_time_struct dat_time;

  dat_time.day = atoi(ptr);
  dat_time.year = atoi(ptr + 7);
  dat_time.hour = atoi(ptr + 12);
  dat_time.minute = atoi(ptr + 15);
  dat_time.second = atoi(ptr + 18);

  if((dat_time.day < 1) || (dat_time.day > 31) || (dat_time.year < 1970) || (dat_time.year > 2999) ||
    (dat_time.hour < 0) || (dat_time.hour > 23) || (dat_time.minute < 0) || (dat_time.minute > 59) ||
    (dat_time.second < 0) || (dat_time.second > 59))  goto ERROR;

  dat_time.month = -1;

  if(!strcmp(ptr + 3, "JAN"))  dat_time.month = 1;
  if(!strcmp(ptr + 3, "FEB"))  dat_time.month = 2;
  if(!strcmp(ptr + 3, "MAR"))  dat_time.month = 3;
  if(!strcmp(ptr + 3, "APR"))  dat_time.month = 4;
  if(!strcmp(ptr + 3, "MAY"))  dat_time.month = 5;
  if(!strcmp(ptr + 3, "JUN"))  dat_time.month = 6;
  if(!strcmp(ptr + 3, "JUL"))  dat_time.month = 7;
  if(!strcmp(ptr + 3, "AUG"))  dat_time.month = 8;
  if(!strcmp(ptr + 3, "SEP"))  dat_time.month = 9;
  if(!strcmp(ptr + 3, "OCT"))  dat_time.month = 10;
  if(!strcmp(ptr + 3, "NOV"))  dat_time.month = 11;
  if(!strcmp(ptr + 3, "DEC"))  dat_time.month = 12;

  if(dat_time.month == -1)  goto ERROR;

  long long utc_time;

  date_time_to_utc(&utc_time, dat_time);

  return utc_time;

ERROR:

  return -1LL;
}












