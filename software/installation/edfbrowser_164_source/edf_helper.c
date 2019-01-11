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



#include "edf_helper.h"



int edfplus_annotation_get_tal_timestamp_digit_cnt(struct edfhdrblock *hdr)
{
  int timestamp_digits;

  char scratchpad[256];

  long long time;


  if(hdr==NULL)
  {
    return -1;
  }

  time = (hdr->datarecords * hdr->long_data_record_duration) / TIME_DIMENSION;

#ifdef _WIN32
  timestamp_digits = __mingw_snprintf(scratchpad, 256, "%lli", time);
#else
  timestamp_digits = snprintf(scratchpad, 256, "%lli", time);
#endif

  return timestamp_digits;
}


int edfplus_annotation_get_tal_timestamp_decimal_cnt(struct edfhdrblock *hdr)
{
  int i, j,
      timestamp_decimals;


  if(hdr==NULL)
  {
    return -1;
  }

  j = 10;

  for(timestamp_decimals=7; timestamp_decimals>0; timestamp_decimals--)
  {
    if(hdr->long_data_record_duration % j)
    {
      break;
    }

    j *= 10;
  }

  if((hdr->edfplus==1)||(hdr->bdfplus==1))
  {
    j = 10;

    for(i=7; i>0; i--)
    {
      if(hdr->starttime_offset % j)
      {
        break;
      }

      j *= 10;
    }

    if(i > timestamp_decimals)
    {
      timestamp_decimals = i;
    }
  }

  return timestamp_decimals;
}









