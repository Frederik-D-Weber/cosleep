/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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


#include "filteredblockread.h"


double * FilteredBlockReadClass::init_signalcomp(struct signalcompblock *i_signalcomp, int i_datarecord_cnt, int skip_f)
{
  skip_filters = skip_f;

  if(i_signalcomp == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1;

    return NULL;
  }
  signalcomp = i_signalcomp;

  hdr = signalcomp->edfhdr;
  if(hdr == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1;

    return NULL;
  }

  inputfile = hdr->file_hdl;
  if(inputfile == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1;

    return NULL;
  }

  datarecord_cnt = i_datarecord_cnt;

  if((datarecord_cnt > hdr->datarecords) || (datarecord_cnt < 1))
  {
    datarecord_cnt = -1;
    total_samples = -1;

    return NULL;
  }

  samples_per_datrec = hdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  total_samples = samples_per_datrec * datarecord_cnt;

  if(processed_samples_buf != NULL)
  {
    free(processed_samples_buf);
  }
  processed_samples_buf = (double *)malloc(total_samples * sizeof(double));
  if(processed_samples_buf == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1;

    return NULL;
  }

  if(readbuf != NULL)
  {
    free(readbuf);
  }
  readbuf = (char *)malloc(hdr->recordsize  * datarecord_cnt);
  if(readbuf == NULL)
  {
    datarecord_cnt = -1;
    total_samples = -1;

    free(processed_samples_buf);
    processed_samples_buf = NULL;
    return NULL;
  }

  bitvalue = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;

  return processed_samples_buf;
}


int FilteredBlockReadClass::samples_in_buf(void)
{
  return total_samples;
}


FilteredBlockReadClass::FilteredBlockReadClass()
{
  readbuf = NULL;
  processed_samples_buf = NULL;

  datarecord_cnt = -1;
  total_samples = -1;

  inputfile = NULL;
  hdr = NULL;
  signalcomp = NULL;
}


FilteredBlockReadClass::~FilteredBlockReadClass()
{
  if(processed_samples_buf != NULL)
  {
    free(processed_samples_buf);
  }

  if(readbuf != NULL)
  {
    free(readbuf);
  }
}


int FilteredBlockReadClass::process_signalcomp(int datarecord_start)
{
  int j, k, s;

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;


  if((total_samples < 1) || (datarecord_cnt < 1))
  {
    return -1;
  }

  if((datarecord_start < 0) || (datarecord_start >= hdr->datarecords))
  {
    return -2;
  }

  if(datarecord_cnt > (hdr->datarecords - datarecord_start))
  {
    return -3;
  }

  if(fseeko(inputfile, ((long long)hdr->hdrsize) + (((long long)datarecord_start) * ((long long) hdr->recordsize)), SEEK_SET) == -1LL)
  {
    return -4;
  }

  if(fread(readbuf, hdr->recordsize * datarecord_cnt, 1, inputfile) != 1)
  {
    return -5;
  }

  if((readbuf == NULL) || (processed_samples_buf == NULL))
  {
    return -6;
  }

  for(s=0; s<total_samples; s++)
  {
    dig_value = 0.0;

    for(j=0; j<signalcomp->num_of_signals; j++)
    {
      if(signalcomp->edfhdr->bdf)
      {
        var.two[0] = *((unsigned short *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s % samples_per_datrec) * 3)));

        var.four[2] = *((unsigned char *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s % samples_per_datrec) * 3)
          + 2));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        f_tmp = var.one_signed;
      }

      if(signalcomp->edfhdr->edf)
      {
        f_tmp = *(((short *)(
          readbuf
          + (signalcomp->edfhdr->recordsize * (s / samples_per_datrec))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset))
          + (s % samples_per_datrec));
      }

      f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
      f_tmp *= signalcomp->factor[j];

      dig_value += f_tmp;
    }

    if(!skip_filters)
    {
      if(signalcomp->spike_filter)
      {
        dig_value = run_spike_filter(dig_value, signalcomp->spike_filter);
      }

      for(k=0; k<signalcomp->filter_cnt; k++)
      {
        dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
      }

      for(k=0; k<signalcomp->ravg_filter_cnt; k++)
      {
        dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
      }

      for(k=0; k<signalcomp->fidfilter_cnt; k++)
      {
        dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
      }

      if(signalcomp->plif_ecg_filter)
      {
        dig_value = plif_run_subtract_filter(dig_value, signalcomp->plif_ecg_filter);
      }

      if(signalcomp->ecg_filter)
      {
        dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
      }

      if(signalcomp->zratio_filter)
      {
        dig_value = run_zratio_filter(dig_value, signalcomp->zratio_filter);
      }

      dig_value *= signalcomp->polarity;
    }

    processed_samples_buf[s] = (dig_value * bitvalue);
  }

  return 0;
}















