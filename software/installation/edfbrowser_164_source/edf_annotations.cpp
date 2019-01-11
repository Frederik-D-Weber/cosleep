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



#include "edf_annotations.h"



static void process_events(void)
{
  qApp->processEvents();
}


int EDF_annotations::get_annotations(struct edfhdrblock *edf_hdr, int read_nk_trigger_signal)
{
  int i, j, k, p, r=0, n,
      edfsignals,
      datarecords,
      recordsize,
      discontinuous,
      *annot_ch,
      nr_annot_chns,
      max,
      onset,
      duration,
      duration_start,
      zero,
      max_tal_ln,
      error,
      annots_in_record,
      annots_in_tal,
      samplesize=2,
      nk_triggers_smpls=0,
      nk_triggers_bufoffset=0,
      nk_triggers_enabled=0,
      nk_triggers_channel=0,
      nk_triggers_cnt=0,
      sf,
      progress_steps;

  unsigned short nk_triggerfields=0,
                 nk_old_triggerfields=0;

  char *scratchpad,
       *cnv_buf,
       *time_in_txt,
       *duration_in_txt,
       nk_triggerlabel[16][32];


  long long data_record_duration,
            elapsedtime,
            time_tmp=0LL,
            nk_trigger_sample_duration=0LL;

  FILE *inputfile;

  struct edfparamblock *edfparam;

  struct annotationblock annotblock;

  inputfile = edf_hdr->file_hdl;
  edfsignals = edf_hdr->edfsignals;
  recordsize = edf_hdr->recordsize;
  edfparam = edf_hdr->edfparam;
  nr_annot_chns = edf_hdr->nr_annot_chns;
  datarecords = edf_hdr->datarecords;
  data_record_duration = edf_hdr->long_data_record_duration;
  discontinuous = edf_hdr->discontinuous;
  annot_ch = edf_hdr->annot_ch;

  if(edf_hdr->edfplus)
  {
    samplesize = 2;
  }
  if(edf_hdr->bdfplus)
  {
    samplesize = 3;
  }

  if((edf_hdr->edfplus) && (read_nk_trigger_signal))
  {
    if(data_record_duration == 1000000LL)
    {
      if(check_device(edf_hdr->plus_equipment) == 0)
      {
        for(i=0; i<edfsignals; i++)
        {
          if(!strcmp(edfparam[i].label, "Events/Markers  "))
          {
            sf = edf_hdr->edfparam[i].smp_per_record;

            error = 1;

            switch(sf)
            {
              case  10 : error = 0;
                         break;
              case  20 : error = 0;
                         break;
              case  50 : error = 0;
                         break;
              case 100 : error = 0;
                         break;
            }

            for(j=0; j<edfsignals; j++)
            {
              if(edf_hdr->edfparam[j].smp_per_record != sf)
              {
                if(!edf_hdr->edfparam[j].annotation)
                {
                  error = 1;
                }
              }
            }

            if(edf_hdr->nr_annot_chns != 1)  error = 1;

            if(!error)
            {
              nk_triggers_channel = i;

              nk_triggers_bufoffset = edfparam[nk_triggers_channel].buf_offset;

              nk_triggers_smpls = edfparam[nk_triggers_channel].smp_per_record;

              nk_trigger_sample_duration = data_record_duration / (long long)nk_triggers_smpls;

              strcpy(nk_triggerlabel[0], "CAL mode");
              strcpy(nk_triggerlabel[1], "RESET condition");
              strcpy(nk_triggerlabel[2], "External mark");
              strcpy(nk_triggerlabel[3], "Photo/HV mark");
              strcpy(nk_triggerlabel[4], "Remote mark");
              strcpy(nk_triggerlabel[5], "HV mark");
              strcpy(nk_triggerlabel[6], "DC trigger 9");
              strcpy(nk_triggerlabel[7], "DC trigger 10");
              strcpy(nk_triggerlabel[8], "DC trigger 11");
              strcpy(nk_triggerlabel[9], "DC trigger 12");
              strcpy(nk_triggerlabel[10], "DC trigger 13");
              strcpy(nk_triggerlabel[11], "DC trigger 14");
              strcpy(nk_triggerlabel[12], "DC trigger 15");
              strcpy(nk_triggerlabel[13], "DC trigger 16");
              strcpy(nk_triggerlabel[14], "");
              strcpy(nk_triggerlabel[15], "");

              nk_triggers_enabled = 1;

              edf_hdr->genuine_nk = 1;

              break;
            }
          }
        }
      }
    }
  }

  cnv_buf = (char *)calloc(1, recordsize);
  if(cnv_buf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(cnv_buf)");
    messagewindow.exec();
    return 1;
  }

  max_tal_ln = 0;

  for(i=0; i<nr_annot_chns; i++)
  {
    if(max_tal_ln<edfparam[annot_ch[i]].smp_per_record * samplesize)  max_tal_ln = edfparam[annot_ch[i]].smp_per_record * samplesize;
  }

  if(max_tal_ln<128)  max_tal_ln = 128;

  scratchpad = (char *)calloc(1, max_tal_ln + 3);
  if(scratchpad==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(scratchpad)");
    messagewindow.exec();
    free(cnv_buf);
    return 1;
  }

  time_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(time_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(time_in_txt)");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    return 1;
  }

  duration_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(duration_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Memory allocation error occurred when trying to read annotations.\n(duration_in_txt)");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    free(time_in_txt);
    return 1;
  }

  if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile annotations. (fseek())");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    free(time_in_txt);
    free(duration_in_txt);
    return 2;
  }

  QProgressDialog progress("Scanning file for annotations...", "Abort", 0, datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  elapsedtime = 0;

  for(i=0; i<datarecords; i++)
  {
    if(!(i%progress_steps))
    {
      progress.setValue(i);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        edf_hdr->annots_not_read = 1;

        free(cnv_buf);
        free(scratchpad);
        free(time_in_txt);
        free(duration_in_txt);
        return 11;
      }
    }

    if(fread(cnv_buf, recordsize, 1, inputfile)!=1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile annotations. (fread())");
      messagewindow.exec();
      free(cnv_buf);
      free(scratchpad);
      free(time_in_txt);
      free(duration_in_txt);
      return 2;
    }


/************** process annotationsignals (if any) **************/

    error = 0;

    for(r=0; r<nr_annot_chns; r++)
    {
      n = 0;
      zero = 0;
      onset = 0;
      duration = 0;
      duration_start = 0;
      scratchpad[0] = 0;
      annots_in_tal = 0;
      annots_in_record = 0;

      p = edfparam[annot_ch[r]].buf_offset;
      max = edfparam[annot_ch[r]].smp_per_record * samplesize;

/************** process one annotation signal ****************/

      if(cnv_buf[p + max - 1]!=0)
      {
        error = 5;
        goto END;
      }

      if(!r)  /* if it's the first annotation signal, then check */
      {       /* the timekeeping annotation */
        error = 1;

        for(k=0; k<(max-2); k++)
        {
          scratchpad[k] = cnv_buf[p + k];

          if(scratchpad[k]==20)
          {
            if(cnv_buf[p + k + 1]!=20)
            {
              error = 6;
              goto END;
            }
            scratchpad[k] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            else
            {
              time_tmp = get_long_time(scratchpad);
              if(i)
              {
                if(discontinuous)
                {
                  if((time_tmp-elapsedtime)<data_record_duration)
                  {
                    error = 4;
                    goto END;
                  }
                }
                else
                {
                  if((time_tmp-elapsedtime)!=data_record_duration)
                  {
                    error = 3;
                    goto END;
                  }
                }
              }
              else
              {
                if(time_tmp>=TIME_DIMENSION)
                {
                  error = 2;
                  goto END;
                }
                else
                {
                  edf_hdr->starttime_offset = time_tmp;
                }
              }
              elapsedtime = time_tmp;
              error = 0;
              break;
            }
          }
        }
      }

      for(k=0; k<max; k++)
      {
        scratchpad[n] = cnv_buf[p + k];

        if(!scratchpad[n])
        {
          if(!zero)
          {
            if(k)
            {
              if(cnv_buf[p + k - 1]!=20)
              {
                error = 33;
                goto END;
              }
            }
            n = 0;
            onset = 0;
            duration = 0;
            duration_start = 0;
            scratchpad[0] = 0;
            annots_in_tal = 0;
          }
          zero++;
          continue;
        }
        if(zero>1)
        {
          error = 34;
          goto END;
        }
        zero = 0;

        if((scratchpad[n]==20)||(scratchpad[n]==21))
        {
          if(scratchpad[n]==21)
          {
            if(duration||duration_start||onset||annots_in_tal)
            {               /* it's not allowed to have multiple duration fields */
              error = 35;   /* in one TAL or to have a duration field which is   */
              goto END;     /* not immediately behind the onsetfield             */
            }
            duration_start = 1;
          }

          if((scratchpad[n]==20)&&onset&&(!duration_start))
          {
            if(r||annots_in_record)
            {
              if(n >= 0)
              {
                memset(&annotblock, 0, sizeof(annotationblock));
                annotblock.file_num = edf_hdr->file_num;
                annotblock.onset = get_long_time(time_in_txt);
                for(j=0; j<n; j++)
                {
                  if(j==MAX_ANNOTATION_LEN)  break;
                  annotblock.annotation[j] = scratchpad[j];
                }
                annotblock.annotation[j] = 0;

                if(duration)
                {
                  strcpy(annotblock.duration, duration_in_txt);

                  annotblock.long_duration = get_long_time(duration_in_txt);
                }

                if(edfplus_annotation_add_item(&edf_hdr->annot_list, annotblock))
                {
                  progress.reset();
                  QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred while reading annotations.");
                  messagewindow.exec();
                  free(cnv_buf);
                  free(scratchpad);
                  free(time_in_txt);
                  free(duration_in_txt);
                  return 1;
                }
              }
            }

            annots_in_tal++;
            annots_in_record++;
            n = 0;
            continue;
          }

          if(!onset)
          {
            scratchpad[n] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            onset = 1;
            n = 0;
            strcpy(time_in_txt, scratchpad);
            continue;
          }

          if(duration_start)
          {
            scratchpad[n] = 0;
            if(is_duration_number(scratchpad))
            {
              error = 37;
              goto END;
            }

            for(j=0; j<n; j++)
            {
              if(j==15)  break;
              duration_in_txt[j] = scratchpad[j];
              if((duration_in_txt[j]<32)||(duration_in_txt[j]>126))
              {
                duration_in_txt[j] = '.';
              }
            }
            duration_in_txt[j] = 0;

            duration = 1;
            duration_start = 0;
            n = 0;
            continue;
          }
        }

        n++;
      }

 END:

/****************** end ************************/

      if(error)
      {
        progress.reset();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not read annotations because there is an EDF or BDF incompatibility in this file.\n"
                                                                  "For more information, run the EDF/BDF compatibility checker in the Tools menu.");
        messagewindow.exec();
        free(cnv_buf);
        free(scratchpad);
        free(time_in_txt);
        free(duration_in_txt);
        return 9;
      }
    }

/************** process NK triggers ****************/

    if(nk_triggers_enabled)
    {
      if(nk_triggers_cnt < 100000)
      {
        for(k=0; k<nk_triggers_smpls; k++)
        {
          nk_triggerfields = *((unsigned char *)cnv_buf + nk_triggers_bufoffset + (k * 2) + 1);

          nk_triggerfields <<= 8;

          nk_triggerfields += *((unsigned char *)cnv_buf + nk_triggers_bufoffset + (k * 2));

          for(j=0; j<14; j++)
          {
            if((nk_triggerfields & (1 << j)) && (!(nk_old_triggerfields & (1 << j))))
            {
              nk_triggers_cnt++;

              memset(&annotblock, 0, sizeof(annotationblock));
              annotblock.file_num = edf_hdr->file_num;
              annotblock.onset = ((long long)i * data_record_duration) + ((long long)k * nk_trigger_sample_duration);
              annotblock.onset += edf_hdr->starttime_offset;
              strcpy(annotblock.annotation, nk_triggerlabel[j]);
              annotblock.ident = (1 << ANNOT_ID_NK_TRIGGER);

              if(edfplus_annotation_add_item(&edf_hdr->annot_list, annotblock))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred while reading annotations.");
                messagewindow.exec();
                free(cnv_buf);
                free(scratchpad);
                free(time_in_txt);
                free(duration_in_txt);
                return 1;
              }
            }
          }

          nk_old_triggerfields = nk_triggerfields;
        }
      }

      edf_hdr->nk_triggers_read = 1;
    }
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  qApp->processEvents();

  edfplus_annotation_sort(&edf_hdr->annot_list, &process_events);

  QApplication::restoreOverrideCursor();

  progress.reset();

  free(cnv_buf);
  free(scratchpad);
  free(time_in_txt);
  free(duration_in_txt);

  return 0;
}


int EDF_annotations::is_duration_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0] == '.')||(str[l-1] == '.'))  return 1;

  for(i=0; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



int EDF_annotations::is_onset_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(l<2)  return 1;

  if((str[0]!='+')&&(str[0]!='-'))  return 1;

  if((str[1] == '.')||(str[l-1] == '.'))  return 1;

  for(i=1; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



long long EDF_annotations::get_long_time(char *str)
{
  int i, len, hasdot=0, dotposition=0, neg=0;

  long long value=0, radix;

  if(str[0] == '+')
  {
    str++;
  }
  else if(str[0] == '-')
    {
      neg = 1;
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
    radix = TIME_DIMENSION;

    for(i=dotposition-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }

    radix = TIME_DIMENSION / 10;

    for(i=dotposition+1; i<len; i++)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix /= 10;
    }
  }
  else
  {
    radix = TIME_DIMENSION;

    for(i=len-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  if(neg)  value = -value;

  return value;
}


int EDF_annotations::check_device(char *str)
{
  int error = 1;

  if(strlen(str) < 29)  return 1;

  if(!strncmp(str, "Nihon Kohden EEG-1100A V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-1100B V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-1100C V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden QI-403A   V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden QI-403A   V02.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-2100  V01.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden EEG-2100  V02.00", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden DAE-2100D V01.30", 29))  error = 0;
  if(!strncmp(str, "Nihon Kohden DAE-2100D V02.00", 29))  error = 0;
//  if(!strncmp(str, "Nihon Kohden EEG-1200A V01.00", 29))  error = 0;

  return error;
}





