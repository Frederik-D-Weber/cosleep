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


#include "mainwindow.h"


void UI_Mainwindow::setup_viewbuf()
{
  int i, j, k, r, s,
      temp=0,
      skip,
      totalsize,
      hasprefilter=0,
      readsize=0,
      dif;

  double pre_time=0.0,
         d_temp=0.0,
         dig_value;

  long long l_temp,
            datarecords;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  struct date_time_struct date_time_str;


  for(i=0; i<files_open; i++) edfheaderlist[i]->prefiltertime = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->filter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->filter_cnt; k++)
      {
        if(pre_time < (1.0 / signalcomp[i]->filter[k]->cutoff_frequency))
        {
          pre_time = (1.0 / signalcomp[i]->filter[k]->cutoff_frequency);
        }
      }
    }

    if(signalcomp[i]->spike_filter)
    {
      hasprefilter = 1;

      if(pre_time < 5.0)
      {
        pre_time = 5.0;
      }
    }

    if(signalcomp[i]->plif_ecg_filter)
    {
      hasprefilter = 1;

      if(pre_time < 2.0)
      {
        pre_time = 2.0;
      }
    }

    if(signalcomp[i]->ravg_filter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->ravg_filter_cnt; k++)
      {
        if(pre_time < ((double)(signalcomp[i]->ravg_filter[k]->size + 3) / ((double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record / signalcomp[i]->edfhdr->data_record_duration)))
        {
          pre_time = (double)(signalcomp[i]->ravg_filter[k]->size + 3) / ((double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record / signalcomp[i]->edfhdr->data_record_duration);
        }
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      hasprefilter = 1;

      if(pre_time < 10.0)
      {
        pre_time = 10.0;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      hasprefilter = 1;

      if(pre_time < 4.0)
      {
        pre_time = 4.0;
      }
    }

    if(signalcomp[i]->fidfilter_cnt)
    {
      hasprefilter = 1;

      for(k=0; k<signalcomp[i]->fidfilter_cnt; k++)
      {
        if(pre_time < ((2.0 * signalcomp[i]->fidfilter_order[k]) / signalcomp[i]->fidfilter_freq[k]))
        {
          pre_time = (2.0 * signalcomp[i]->fidfilter_order[k]) / signalcomp[i]->fidfilter_freq[k];
        }
      }
    }
  }

  if(hasprefilter)
  {
    for(i=0; i<signalcomps; i++)
    {
      if((signalcomp[i]->filter_cnt) || (signalcomp[i]->spike_filter) || (signalcomp[i]->ravg_filter_cnt) || (signalcomp[i]->fidfilter_cnt) || (signalcomp[i]->plif_ecg_filter != NULL) || (signalcomp[i]->ecg_filter != NULL) || (signalcomp[i]->zratio_filter != NULL))
      {
        signalcomp[i]->edfhdr->prefiltertime = (long long)(pre_time * ((double)TIME_DIMENSION));
        if(signalcomp[i]->edfhdr->prefiltertime>signalcomp[i]->edfhdr->viewtime)
        {
          signalcomp[i]->edfhdr->prefiltertime = signalcomp[i]->edfhdr->viewtime;
          if(signalcomp[i]->edfhdr->prefiltertime<0) signalcomp[i]->edfhdr->prefiltertime = 0;
        }
      }
    }

    totalsize = 0;

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->edfhdr->prefiltertime)  signalcomp[i]->records_in_viewbuf = (signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration) - ((signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;
      else signalcomp[i]->records_in_viewbuf = 0;

      signalcomp[i]->viewbufsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

      if(signalcomp[i]->edfhdr->prefiltertime)
      {
        signalcomp[i]->samples_in_prefilterbuf = (signalcomp[i]->records_in_viewbuf - 1) * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record;

        signalcomp[i]->samples_in_prefilterbuf
        += (int)(((double)(signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration)
        / (double)signalcomp[i]->edfhdr->long_data_record_duration)
        * (double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record);
      }
      else
      {
        signalcomp[i]->samples_in_prefilterbuf = 0;
      }

      if(!i)
      {
        signalcomp[i]->viewbufoffset = 0;
        totalsize = signalcomp[i]->viewbufsize;
      }
      else
      {
        skip = 0;

        for(j=0; j<i; j++)
        {
          if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
          {
            skip = 1;
            signalcomp[i]->viewbufoffset = signalcomp[j]->viewbufoffset;
            signalcomp[i]->records_in_viewbuf = signalcomp[j]->records_in_viewbuf;
            signalcomp[i]->viewbufsize = signalcomp[j]->viewbufsize;
            break;
          }
        }

        if(!skip)
        {
          signalcomp[i]->viewbufoffset = totalsize;
          totalsize += signalcomp[i]->viewbufsize;
        }
      }
    }

    if(viewbuf!=NULL)
    {
      free(viewbuf);
      viewbuf = NULL;
    }

    viewbuf = (char *)malloc(totalsize);
    if(viewbuf==NULL)
    {
      live_stream_active = 0;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: Memory allocation error:\n\"prefilterbuf\"");
      messagewindow.exec();
      remove_all_signals();
      return;
    }

    for(i=0; i<signalcomps; i++)
    {
      if(!i)
      {
        datarecords = (signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration;

        signalcomp[i]->prefilter_starttime = datarecords * signalcomp[i]->edfhdr->long_data_record_duration;

        if((signalcomp[i]->viewbufsize>0)&&(datarecords<signalcomp[i]->edfhdr->datarecords))
        {
          fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)(signalcomp[i]->edfhdr->hdrsize + (datarecords * signalcomp[i]->edfhdr->recordsize)), SEEK_SET);

          if(signalcomp[i]->viewbufsize>((signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize))
          {
            signalcomp[i]->viewbufsize = (signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize;
          }

          if(fread(viewbuf + signalcomp[i]->viewbufoffset, signalcomp[i]->viewbufsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
          {
            live_stream_active = 0;
            QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred. 2");
            messagewindow.exec();
            remove_all_signals();
            return;
          }
        }
      }
      else
      {
        skip = 0;

        for(j=0; j<i; j++)
        {
          if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
          {
            skip = 1;
            break;
          }
        }

        if(!skip)
        {
          datarecords = (signalcomp[i]->edfhdr->viewtime - signalcomp[i]->edfhdr->prefiltertime) / signalcomp[i]->edfhdr->long_data_record_duration;

          signalcomp[i]->prefilter_starttime = datarecords * signalcomp[i]->edfhdr->long_data_record_duration;

          if((signalcomp[i]->viewbufsize>0)&&(datarecords<signalcomp[i]->edfhdr->datarecords))
          {
            fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)(signalcomp[i]->edfhdr->hdrsize + (datarecords * signalcomp[i]->edfhdr->recordsize)), SEEK_SET);

            if(signalcomp[i]->viewbufsize>((signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize))
            {
              signalcomp[i]->viewbufsize = (signalcomp[i]->edfhdr->datarecords - datarecords) * signalcomp[i]->edfhdr->recordsize;
            }

            if(fread(viewbuf + signalcomp[i]->viewbufoffset, signalcomp[i]->viewbufsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
            {
              live_stream_active = 0;
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred. 3");
              messagewindow.exec();
              remove_all_signals();
              return;
            }
          }
        }
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->zratio_filter != NULL)
      {
        l_temp = signalcomp[i]->prefilter_starttime % (TIME_DIMENSION * 2LL); // necessary for the Z-ratio filter
        if(l_temp != 0L)
        {
          temp = (TIME_DIMENSION * 2LL) - l_temp;

          l_temp = temp;

          signalcomp[i]->prefilter_reset_sample = (l_temp / signalcomp[i]->edfhdr->long_data_record_duration)
          * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record;

          signalcomp[i]->prefilter_reset_sample
          += (int)(((double)(l_temp % signalcomp[i]->edfhdr->long_data_record_duration)
          / (double)signalcomp[i]->edfhdr->long_data_record_duration)
          * (double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record);
        }
        else
        {
          signalcomp[i]->prefilter_reset_sample = 0;
        }

// printf("records_in_viewbuf is %lli\n"
//       "samples_in_prefilterbuf is %i\n"
//       "l_temp is %lli\n"
//       "temp is %i\n"
//       "prefilter_reset_sample is %i\n\n",
//       signalcomp[i]->records_in_viewbuf,
//       signalcomp[i]->samples_in_prefilterbuf,
//       l_temp,
//       temp,
//       signalcomp[i]->prefilter_reset_sample);

      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if((!signalcomp[i]->filter_cnt) && (!signalcomp[i]->spike_filter) && (!signalcomp[i]->ravg_filter_cnt) && (!signalcomp[i]->fidfilter_cnt) && (!signalcomp[i]->plif_ecg_filter) && (signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL)) continue;

      for(s=0; s<signalcomp[i]->samples_in_prefilterbuf; s++)
      {
        dig_value = 0.0;

        for(k=0; k<signalcomp[i]->num_of_signals; k++)
        {
          if(signalcomp[i]->edfhdr->bdf)
          {
            var.two[0] = *((unsigned short *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].buf_offset
              + ((s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record) * 3)));

            var.four[2] = *((unsigned char *)(
              viewbuf
              + signalcomp[i]->viewbufoffset
              + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
              + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].buf_offset
              + ((s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record) * 3)
              + 2));

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            d_temp = var.one_signed;
          }

          if(signalcomp[i]->edfhdr->edf)
          {
            d_temp = *(((short *)(
            viewbuf
            + signalcomp[i]->viewbufoffset
            + (signalcomp[i]->edfhdr->recordsize * (s / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record))
            + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].buf_offset))
            + (s % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].smp_per_record));
          }

          d_temp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[k]].offset;
          d_temp *= signalcomp[i]->factor[k];

          dig_value += d_temp;
        }

        if(signalcomp[i]->spike_filter)
        {
          dig_value = run_spike_filter(dig_value, signalcomp[i]->spike_filter);
        }

        for(j=0; j<signalcomp[i]->filter_cnt; j++)
        {
          dig_value = first_order_filter(dig_value, signalcomp[i]->filter[j]);
        }

        for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
        {
          dig_value = run_ravg_filter(dig_value, signalcomp[i]->ravg_filter[j]);
        }

        for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
        {
          dig_value = signalcomp[i]->fidfuncp[j](signalcomp[i]->fidbuf[j], dig_value);
        }

        if(signalcomp[i]->plif_ecg_filter)
        {
          dig_value = plif_run_subtract_filter(dig_value, signalcomp[i]->plif_ecg_filter);
        }

        if(signalcomp[i]->ecg_filter != NULL)
        {
          if(s == 0)
          {
            reset_ecg_filter(signalcomp[i]->ecg_filter);
          }

          dig_value = run_ecg_filter(dig_value, signalcomp[i]->ecg_filter);
        }

        if(signalcomp[i]->zratio_filter != NULL)
        {
          if(s == signalcomp[i]->prefilter_reset_sample)
          {
            reset_zratio_filter(signalcomp[i]->zratio_filter);
          }

          dig_value = run_zratio_filter(dig_value, signalcomp[i]->zratio_filter);
        }
      }
    }

    for(i=0; i<signalcomps; i++)
    {
      if(signalcomp[i]->samples_in_prefilterbuf > 0)
      {
        if(signalcomp[i]->spike_filter)
        {
          spike_filter_save_buf(signalcomp[i]->spike_filter);
        }

        for(j=0; j<signalcomp[i]->filter_cnt; j++)
        {
          signalcomp[i]->filterpreset_a[j] = signalcomp[i]->filter[j]->old_input;
          signalcomp[i]->filterpreset_b[j] = signalcomp[i]->filter[j]->old_output;
        }

        for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
        {
          ravg_filter_save_buf(signalcomp[i]->ravg_filter[j]);
        }

        for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
        {
          memcpy(signalcomp[i]->fidbuf2[j], signalcomp[i]->fidbuf[j], fid_run_bufsize(signalcomp[i]->fid_run[j]));
        }

        if(signalcomp[i]->plif_ecg_filter)
        {
          plif_subtract_filter_state_copy(signalcomp[i]->plif_ecg_filter_sav, signalcomp[i]->plif_ecg_filter);
        }

        if(signalcomp[i]->ecg_filter != NULL)
        {
          ecg_filter_save_buf(signalcomp[i]->ecg_filter);
        }

        if(signalcomp[i]->zratio_filter != NULL)
        {
          zratio_filter_save_buf(signalcomp[i]->zratio_filter);
        }
      }
    }
  }

  totalsize = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfhdr->viewtime>=0)  signalcomp[i]->records_in_viewbuf = ((pagetime + (signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration)) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;
    else  signalcomp[i]->records_in_viewbuf = ((pagetime + ((-(signalcomp[i]->edfhdr->viewtime)) % signalcomp[i]->edfhdr->long_data_record_duration)) / signalcomp[i]->edfhdr->long_data_record_duration) + 1;

    signalcomp[i]->viewbufsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

    signalcomp[i]->samples_on_screen = (int)(((double)pagetime / (double)signalcomp[i]->edfhdr->long_data_record_duration) * (double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record);

    if(signalcomp[i]->edfhdr->viewtime<0)
    {
      d_temp =
        (((double)(-(signalcomp[i]->edfhdr->viewtime)))
        / (double)signalcomp[i]->edfhdr->long_data_record_duration)
        * (double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record;

      if(d_temp>2147483648.0)
      {
        signalcomp[i]->sample_start = 2147483647LL;
      }
      else
      {
        signalcomp[i]->sample_start = (int)d_temp;
      }
    }
    else
    {
      signalcomp[i]->sample_start = 0;
    }

    if(signalcomp[i]->edfhdr->viewtime>=0)
    {
      signalcomp[i]->sample_timeoffset_part = ((double)(signalcomp[i]->edfhdr->viewtime % signalcomp[i]->edfhdr->long_data_record_duration) / (double)signalcomp[i]->edfhdr->long_data_record_duration) * (double)signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record;
      signalcomp[i]->sample_timeoffset = (int)(signalcomp[i]->sample_timeoffset_part);
      signalcomp[i]->sample_timeoffset_part -= signalcomp[i]->sample_timeoffset;
    }
    else
    {
      signalcomp[i]->sample_timeoffset_part = 0.0;
      signalcomp[i]->sample_timeoffset = 0;
    }

    if(!i)
    {
      signalcomp[i]->viewbufoffset = 0;
      totalsize = signalcomp[i]->viewbufsize;
    }
    else
    {
      skip = 0;

      for(j=0; j<i; j++)
      {
        if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
        {
          skip = 1;
          signalcomp[i]->viewbufoffset = signalcomp[j]->viewbufoffset;
          signalcomp[i]->records_in_viewbuf = signalcomp[j]->records_in_viewbuf;
          signalcomp[i]->viewbufsize = signalcomp[j]->viewbufsize;
          break;
        }
      }

      if(!skip)
      {
        signalcomp[i]->viewbufoffset = totalsize;
        totalsize += signalcomp[i]->viewbufsize;
      }
    }
  }

  if(viewbuf!=NULL)
  {
    free(viewbuf);
    viewbuf = NULL;
  }

  if(totalsize)
  {
    viewbuf = (char *)malloc(totalsize);
    if(viewbuf==NULL)
    {
      live_stream_active = 0;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.\n"
                                                                "Decrease the timescale and try again.");
      messagewindow.exec();

      remove_all_signals();
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(!i)
    {
      if(signalcomp[i]->edfhdr->viewtime>=0)
      {
        datarecords = signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration;
      }
      else
      {
        datarecords = 0;
      }

      dif = signalcomp[i]->edfhdr->datarecords - datarecords;

      if(dif<=0)
      {
        memset(viewbuf + signalcomp[i]->viewbufoffset, 0, signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize);

        signalcomp[i]->sample_stop = 0;
      }
      else
      {
        if(dif<signalcomp[i]->records_in_viewbuf)
        {
          readsize = dif * signalcomp[i]->edfhdr->recordsize;

          memset(viewbuf + signalcomp[i]->viewbufoffset + readsize, 0, (signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize) - readsize);

          signalcomp[i]->sample_stop = (dif * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record) - signalcomp[i]->sample_timeoffset;
        }
        else
        {
          readsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;

          signalcomp[i]->sample_stop = signalcomp[i]->samples_on_screen;
        }

        l_temp = signalcomp[i]->edfhdr->hdrsize;
        l_temp += (datarecords * signalcomp[i]->edfhdr->recordsize);

        fseeko(signalcomp[i]->edfhdr->file_hdl, l_temp, SEEK_SET);

        if(fread(viewbuf + signalcomp[i]->viewbufoffset, readsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
        {
          live_stream_active = 0;
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred. 5");
          messagewindow.exec();
          remove_all_signals();
          return;
        }
      }
    }
    else
    {
      skip = 0;

      for(j=0; j<i; j++)
      {
        if(signalcomp[i]->edfhdr->file_hdl==signalcomp[j]->edfhdr->file_hdl)
        {
          skip = 1;
          break;
        }
      }

      if(signalcomp[i]->edfhdr->viewtime>=0)
      {
        datarecords = signalcomp[i]->edfhdr->viewtime / signalcomp[i]->edfhdr->long_data_record_duration;
      }
      else
      {
        datarecords = 0;
      }

      dif = signalcomp[i]->edfhdr->datarecords - datarecords;

      if(dif<=0)
      {
        if(!skip)
        {
          memset(viewbuf + signalcomp[i]->viewbufoffset, 0, signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize);
        }

        signalcomp[i]->sample_stop = 0;
      }
      else
      {
        if(dif<signalcomp[i]->records_in_viewbuf)
        {
          if(!skip)
          {
            readsize = dif * signalcomp[i]->edfhdr->recordsize;

            memset(viewbuf + signalcomp[i]->viewbufoffset + readsize, 0, (signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize) - readsize);
          }

          signalcomp[i]->sample_stop = (dif * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record) - signalcomp[i]->sample_timeoffset;
        }
        else
        {
          if(!skip)
          {
            readsize = signalcomp[i]->records_in_viewbuf * signalcomp[i]->edfhdr->recordsize;
          }

          signalcomp[i]->sample_stop = signalcomp[i]->samples_on_screen;
        }

        if(!skip)
        {
          l_temp = signalcomp[i]->edfhdr->hdrsize;
          l_temp += (datarecords * signalcomp[i]->edfhdr->recordsize);

          fseeko(signalcomp[i]->edfhdr->file_hdl, l_temp, SEEK_SET);

          if(fread(viewbuf + signalcomp[i]->viewbufoffset, readsize, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
          {
            live_stream_active = 0;
            QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred. 6");
            messagewindow.exec();
            remove_all_signals();
            return;
          }
        }
      }
    }

    signalcomp[i]->sample_stop += signalcomp[i]->sample_start;
  }

  if(signalcomps && (!signal_averaging_active))
  {
    viewtime_string[0] = 0;

    if(viewtime_indicator_type == 2)
    {
      l_temp = (edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) / TIME_DIMENSION;
      l_temp += edfheaderlist[sel_viewtime]->utc_starttime;
      utc_to_date_time(l_temp, &date_time_str);

      snprintf(viewtime_string, 32, "%2i-%s ", date_time_str.day, date_time_str.month_str);
    }

    if((edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset)>=0LL)
    {
      if(viewtime_indicator_type > 0)
      {
        snprintf(viewtime_string + strlen(viewtime_string), 32, "%2i:%02i:%02i.%04i (",
                (int)((((edfheaderlist[sel_viewtime]->l_starttime + edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) / TIME_DIMENSION)/ 3600LL) % 24LL),
                (int)((((edfheaderlist[sel_viewtime]->l_starttime + edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) / TIME_DIMENSION) % 3600LL) / 60LL),
                (int)(((edfheaderlist[sel_viewtime]->l_starttime + edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) / TIME_DIMENSION) % 60LL),
                (int)(((edfheaderlist[sel_viewtime]->l_starttime + edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) % TIME_DIMENSION) / 1000LL));
      }

      snprintf(viewtime_string + strlen(viewtime_string), 32, "%i:%02i:%02i.%04i",
              (int)((edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION)/ 3600LL),
              (int)(((edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION) % 3600LL) / 60LL),
              (int)((edfheaderlist[sel_viewtime]->viewtime / TIME_DIMENSION) % 60LL),
              (int)((edfheaderlist[sel_viewtime]->viewtime % TIME_DIMENSION) / 1000LL));

      if(viewtime_indicator_type > 0)
      {
        sprintf(viewtime_string + strlen(viewtime_string), ")");
      }
    }
    else
    {
      l_temp = edfheaderlist[sel_viewtime]->l_starttime + ((edfheaderlist[sel_viewtime]->viewtime + edfheaderlist[sel_viewtime]->starttime_offset) % (86400LL * TIME_DIMENSION));
      if(l_temp<=0)
      {
        l_temp += (86400LL * TIME_DIMENSION);
      }

      if(viewtime_indicator_type > 0)
      {
        snprintf(viewtime_string + strlen(viewtime_string), 32, "%2i:%02i:%02i.%04i (",
                (int)((((l_temp) / TIME_DIMENSION)/ 3600LL) % 24LL),
                (int)((((l_temp) / TIME_DIMENSION) % 3600LL) / 60LL),
                (int)(((l_temp) / TIME_DIMENSION) % 60LL),
                (int)(((l_temp) % TIME_DIMENSION) / 1000LL));
      }

      l_temp = -edfheaderlist[sel_viewtime]->viewtime;

      snprintf(viewtime_string + strlen(viewtime_string), 32, "-%i:%02i:%02i.%04i",
              (int)((l_temp / TIME_DIMENSION)/ 3600LL),
              (int)(((l_temp / TIME_DIMENSION) % 3600LL) / 60LL),
              (int)((l_temp / TIME_DIMENSION) % 60LL),
              (int)((l_temp % TIME_DIMENSION) / 1000LL));

      if(viewtime_indicator_type > 0)
      {
        sprintf(viewtime_string + strlen(viewtime_string), ")");
      }
    }

    if(pagetime >= (3600LL * TIME_DIMENSION))
    {
      snprintf(pagetime_string, 32, "%i:%02i:%02i.%04i",
              ((int)(pagetime / TIME_DIMENSION)) / 3600,
              (((int)(pagetime / TIME_DIMENSION)) % 3600) / 60,
              ((int)(pagetime / TIME_DIMENSION)) % 60,
              (int)((pagetime % TIME_DIMENSION) / 1000LL));
    }
    else if(pagetime >= (60LL * TIME_DIMENSION))
      {
        snprintf(pagetime_string, 32, "%i:%02i.%04i",
                ((int)(pagetime / TIME_DIMENSION)) / 60,
                ((int)(pagetime / TIME_DIMENSION)) % 60,
                (int)((pagetime % TIME_DIMENSION) / 1000LL));
      }
      else if(pagetime >= TIME_DIMENSION)
      {
        snprintf(pagetime_string, 32, "%i.%04i sec",
                (int)(pagetime / TIME_DIMENSION),
                (int)((pagetime % TIME_DIMENSION) / 1000LL));
      }
      else
      {
        convert_to_metric_suffix(pagetime_string, (double)pagetime / TIME_DIMENSION, 3);

        strcat(pagetime_string, "S");
      }

    remove_trailing_zeros(viewtime_string);
    remove_trailing_zeros(pagetime_string);
  }

  if(!signal_averaging_active)
  {
    if(print_to_edf_active)
    {
      print_to_edf_active = 0;
    }
    else
    {
      if(signalcomps && (!live_stream_active))
      {
        positionslider->blockSignals(true);

        long long record_duration = edfheaderlist[sel_viewtime]->long_data_record_duration * edfheaderlist[sel_viewtime]->datarecords;

        record_duration -= pagetime;

        if(edfheaderlist[sel_viewtime]->viewtime<=0)
        {
          positionslider->setValue(0);
        }
        else
        {
          if(edfheaderlist[sel_viewtime]->viewtime>=record_duration)
          {
            positionslider->setValue(1000000);
          }
          else
          {
            if(record_duration<pagetime)
            {
              positionslider->setValue(1000000);
            }
            else
            {
              positionslider->setValue(edfheaderlist[sel_viewtime]->viewtime * 1000000LL / record_duration);
            }
          }
        }

        slidertoolbar->setEnabled(true);
      }
      else
      {
        slidertoolbar->setEnabled(false);

        positionslider->blockSignals(true);
      }

      maincurve->drawCurve_stage_1();

      if(signalcomps && (!live_stream_active))
      {
        positionslider->blockSignals(false);
      }
    }

    for(r=0; r<MAXSPECTRUMDOCKS; r++)
    {
      if(spectrumdock[r]->dock->isVisible())
      {
        spectrumdock[r]->rescan();
      }
    }
  }

//   printf("\n");
//
//   for(int n=0; n<signalcomps; n++)
//   {
//     printf("signalcomp: %i  filenum: %i  signal: %i  viewbufoffset: %i  buf_offset: %i\n",
//            n,
//            signalcomp[n]->filenum, signalcomp[n]->edfsignal[0],
//            signalcomp[n]->viewbufoffset,
//            signalcomp[n]->edfhdr->edfparam[signalcomp[n]->edfsignal[0]].buf_offset);
//   }
}

















