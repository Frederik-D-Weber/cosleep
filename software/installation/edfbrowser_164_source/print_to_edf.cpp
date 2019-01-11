/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "print_to_edf.h"


void print_screen_to_edf(UI_Mainwindow *mainwindow)
{
  int i, j, k, p,
      n=0,
      records,
      records_written,
      signalcomps,
      duration_factor[MAXFILES],
      temp=0,
      edfplus=0,
      tallen,
      annotationlist_nr=-1,
      annotations_left=1,
      add_one_sec=0,
      annot_smp_per_record=16,
      annot_list_sz=0,
      annot_cnt=0,
      type,
      len;

  long long duration,
            smpls_written[MAXSIGNALS],
            s2,
            preamble=0LL,
            smpls_preamble[MAXSIGNALS],
            taltime=0LL,
            l_temp,
            referencetime,
            annot_difftime=0LL;

  char path[MAX_PATH_LENGTH],
       scratchpad[512],
       datrecduration[8],
       *viewbuf;

  double frequency,
         frequency2,
         dig_value;

  FILE *outputfile;


  struct signalcompblock **signalcomp;

  union {
          signed int one_signed;
          signed short two_signed[2];
          unsigned char four[4];
        } wr_var;

  union {
          signed short one_signed;
          unsigned char two[2];
        } null_bytes[MAXSIGNALS];

  struct date_time_struct date_time;

  struct annotation_list *annot_list=NULL;

  struct annotationblock *annot_ptr=NULL;

/////////////////////////////////////////////////////////////////////////

  signalcomps = mainwindow->signalcomps;
  signalcomp = mainwindow->signalcomp;
  viewbuf = mainwindow->viewbuf;

  if((!mainwindow->files_open)||(!signalcomps))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Print to EDF", "Put a signal on the screen and try again.");
    messagewindow.exec();
    return;
  }

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(mainwindow->edfheaderlist[i]->bdf)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Print to EDF", "BDF files can not be printed to EDF.");
      messagewindow.exec();
      return;
    }

    if(mainwindow->edfheaderlist[i]->discontinuous)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Print to EDF", "Sorry, discontinues EDF files can not be printed to EDF.");
      messagewindow.exec();
      return;
    }

    annot_list = &mainwindow->edfheaderlist[i]->annot_list;

    if(mainwindow->edfheaderlist[i]->edfplus)
    {
      edfplus = 1;

      for(j=0; j<mainwindow->edfheaderlist[i]->nr_annot_chns; j++)
      {
        if(mainwindow->edfheaderlist[i]->edfparam[mainwindow->edfheaderlist[i]->annot_ch[j]].smp_per_record>annot_smp_per_record)
        {
          annot_smp_per_record = mainwindow->edfheaderlist[i]->edfparam[mainwindow->edfheaderlist[i]->annot_ch[j]].smp_per_record;
        }
      }
    }
  }

  annot_smp_per_record += 16;

  taltime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_offset;

  duration = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(signalcomp[i]->edfhdr->long_data_record_duration>duration)
    {
      duration = signalcomp[i]->edfhdr->long_data_record_duration;
      n = i;
    }
  }

  fseeko(signalcomp[n]->edfhdr->file_hdl, 244LL, SEEK_SET);
  if(fread(datrecduration, 8, 1, signalcomp[n]->edfhdr->file_hdl)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
    messagewindow.exec();
    return;
  }

  temp = 0;

  for(i=0; i<signalcomps; i++)
  {
    if(duration % signalcomp[i]->edfhdr->long_data_record_duration)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "This combination of files can not be printed to EDF\n"
                                                                "because the datarecordblock durations are not an integer multiple.");
      messagewindow.exec();
      return;
    }

    duration_factor[signalcomp[i]->filenum] = duration / signalcomp[i]->edfhdr->long_data_record_duration;

    temp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record;
  }

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strcat(path, "_screenprint.edf");

  strcpy(path, QFileDialog::getSaveFileName(0, "Print to EDF", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, selected file is in use.");
    messagewindow.exec();
    return;
  }

  outputfile = fopeno(path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not create a file for writing.");
    messagewindow.exec();
    return;
  }

  referencetime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime;

  if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime<0)
  {
    if((-mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime) % TIME_DIMENSION)
    {
      preamble = TIME_DIMENSION - ((-mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime) % TIME_DIMENSION);

      referencetime--;

      add_one_sec = 1;
    }
  }
  else
  {
    preamble = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime % TIME_DIMENSION;

    if(preamble)  add_one_sec = 1;
  }

  for(i=0; i<signalcomps; i++)
  {
    smpls_preamble[i] = preamble / (signalcomp[i]->edfhdr->long_data_record_duration / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record);
  }

  referencetime += (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime / TIME_DIMENSION);

  utc_to_date_time(referencetime, &date_time);

/************************* write EDF-header ***************************************/

  rewind(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl);

  rewind(outputfile);

  if(edfplus)
  {
    if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus)
    {
      if(fread(scratchpad, 88, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad, 88, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
    else
    {
      fprintf(outputfile, "0       X X X X ");

      if(fread(scratchpad, 88, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad + 8, 72, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    fprintf(outputfile, "Startdate %02i-", date_time.day);

    switch(date_time.month)
    {
      case  1 : fprintf(outputfile, "JAN");
                break;
      case  2 : fprintf(outputfile, "FEB");
                break;
      case  3 : fprintf(outputfile, "MAR");
                break;
      case  4 : fprintf(outputfile, "APR");
                break;
      case  5 : fprintf(outputfile, "MAY");
                break;
      case  6 : fprintf(outputfile, "JUN");
                break;
      case  7 : fprintf(outputfile, "JUL");
                break;
      case  8 : fprintf(outputfile, "AUG");
                break;
      case  9 : fprintf(outputfile, "SEP");
                break;
      case 10 : fprintf(outputfile, "OCT");
                break;
      case 11 : fprintf(outputfile, "NOV");
                break;
      case 12 : fprintf(outputfile, "DEC");
                break;
      default : fprintf(outputfile, "ERR");
                break;
    }

    fprintf(outputfile, "-%04i ", date_time.year);

    if(mainwindow->edfheaderlist[mainwindow->sel_viewtime]->edfplus)
    {
      if(fread(scratchpad, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(scratchpad[10]=='X')
      {
        if(fwrite(scratchpad + 12, 58, 1, outputfile)!=1)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
          messagewindow.exec();
          fclose(outputfile);
          return;
        }
      }
      else
      {
        if(fwrite(scratchpad + 22, 58, 1, outputfile)!=1)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
          messagewindow.exec();
          fclose(outputfile);
          return;
        }
      }
    }
    else
    {
      fprintf(outputfile, "X X X ");

      if(fread(scratchpad, 80, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      if(fwrite(scratchpad + 28, 52, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }
  else
  {
    if(fread(scratchpad, 168, 1, mainwindow->edfheaderlist[mainwindow->sel_viewtime]->file_hdl)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }

    if(fwrite(scratchpad, 168, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  fprintf(outputfile,
          "%02i.%02i.%02i%02i.%02i.%02i",
          date_time.day,
          date_time.month,
          date_time.year % 100,
          date_time.hour,
          date_time.minute,
          date_time.second);

  fprintf(outputfile, "%-8i", (signalcomps + edfplus) * 256 + 256);

  if(edfplus)
  {
    fprintf(outputfile, "EDF+C");

    for(i=0; i<39; i++)  fputc(' ', outputfile);
  }
  else  for(i=0; i<44; i++)  fputc(' ', outputfile);

  records = (int)(mainwindow->pagetime / duration);

  if(add_one_sec)
  {
    records += TIME_DIMENSION / duration;
  }

  fprintf(outputfile, "%-8i", records);

  if(fwrite(datrecduration, 8, 1, outputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
    messagewindow.exec();
    fclose(outputfile);
    return;
  }

  fprintf(outputfile, "%-4i", signalcomps + edfplus);

  for(i=0; i<signalcomps; i++)
  {
    strcpy(scratchpad, signalcomp[i]->signallabel);
    strcat(scratchpad, "                ");
    if(fwrite(scratchpad, 16, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(edfplus)
  {
    if(fwrite("EDF Annotations ", 16, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if(fwrite(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].transducer, 80, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(edfplus)
  {
    for(i=0; i<80; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    strcpy(scratchpad, signalcomp[i]->physdimension);
    strcat(scratchpad, "        ");
    if(fwrite(scratchpad, 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(edfplus)
  {
    for(i=0; i<8; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)((signalcomp[i]->edfsignal[0] * 8) + (104 * signalcomp[i]->edfhdr->edfsignals) + 256), SEEK_SET);
      if(fread(scratchpad, 8, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
      if(fwrite(scratchpad, 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      if(fwrite("0       ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      if(fwrite("-1      ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(edfplus)
  {
    if(fwrite("-1      ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fseeko(signalcomp[i]->edfhdr->file_hdl, (long long)((signalcomp[i]->edfsignal[0] * 8) + (112 * signalcomp[i]->edfhdr->edfsignals) + 256), SEEK_SET);
      if(fread(scratchpad, 8, 1, signalcomp[i]->edfhdr->file_hdl)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
      if(fwrite(scratchpad, 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      if(fwrite("1000    ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      if(fwrite("1       ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(edfplus)
  {
    if(fwrite("1       ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min);

      if(signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min<0)
      {
        null_bytes[i].one_signed = 0;
      }
      else
      {
        null_bytes[i].one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min;
      }
    }
    else
    {
      if(fwrite("-32768  ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }

      null_bytes[i].one_signed = -32768;
    }
  }

  if(edfplus)
  {
    if(fwrite("-32768  ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
    {
      fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max);
    }
    else
    {
      if(fwrite("32767   ", 8, 1, outputfile)!=1)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
        messagewindow.exec();
        fclose(outputfile);
        return;
      }
    }
  }

  if(edfplus)
  {
    if(fwrite("32767   ", 8, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  for(i=0; i<signalcomps; i++)
  {
    strcpy(scratchpad, signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].prefilter);
    strcat(scratchpad, "                                                                                ");
    for(p = strlen(scratchpad) - 1; p>=0; p--)
    {
      if(scratchpad[p]!=' ')  break;
    }
    p++;
    if(p) p++;

    if(signalcomp[i]->spike_filter)
    {
      p += sprintf(scratchpad + p, "Spike:%f", signalcomp[i]->spike_filter->velocity);

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad[k]!='0')  break;
      }

      if(scratchpad[k]=='.')  scratchpad[k] = 0;
      else  scratchpad[k+1] = 0;

      p = strlen(scratchpad);
    }

    for(j=0; j<signalcomp[i]->filter_cnt; j++)
    {
      if(signalcomp[i]->filter[j]->is_LPF == 1)
      {
        p += sprintf(scratchpad + p, "LP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      if(signalcomp[i]->filter[j]->is_LPF == 0)
      {
        p += sprintf(scratchpad + p, "HP:%f", signalcomp[i]->filter[j]->cutoff_frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad[k]!='0')  break;
      }

      if(scratchpad[k]=='.')  scratchpad[k] = 0;
      else  scratchpad[k+1] = 0;

      strcat(scratchpad, "Hz ");

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->fidfilter_cnt; j++)
    {
      type = signalcomp[i]->fidfilter_type[j];

      frequency = signalcomp[i]->fidfilter_freq[j];

      frequency2 = signalcomp[i]->fidfilter_freq2[j];

      if(type == 0)
      {
        p += sprintf(scratchpad + p, "HP:%f", frequency);
      }

      if(type == 1)
      {
        p += sprintf(scratchpad + p, "LP:%f", frequency);
      }

      if(type == 2)
      {
        p += sprintf(scratchpad + p, "N:%f", frequency);
      }

      if(type == 3)
      {
        p += sprintf(scratchpad + p, "BP:%f", frequency);
      }

      if(type == 4)
      {
        p += sprintf(scratchpad + p, "BS:%f", frequency);
      }

      for(k=(p-1); k>0; k--)
      {
        if(scratchpad[k]!='0')  break;
      }

      if(scratchpad[k]=='.')  scratchpad[k] = 0;
      else  scratchpad[k+1] = 0;

      p = strlen(scratchpad);

      if((type == 3) || (type == 4))
      {
        p += sprintf(scratchpad + p, "-%f", frequency2);

        for(k=(p-1); k>0; k--)
        {
          if(scratchpad[k]!='0')  break;
        }

        if(scratchpad[k]=='.')  scratchpad[k] = 0;
        else  scratchpad[k+1] = 0;
      }

      strcat(scratchpad, "Hz ");

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    for(j=0; j<signalcomp[i]->ravg_filter_cnt; j++)
    {
      if(signalcomp[i]->ravg_filter_type[j] == 0)
      {
        p += sprintf(scratchpad + p, "HP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      if(signalcomp[i]->ravg_filter_type[j] == 1)
      {
        p += sprintf(scratchpad + p, "LP:%iSmpls ", signalcomp[i]->ravg_filter[j]->size);
      }

      p = strlen(scratchpad);

      if(p>80)  break;
    }

    if(signalcomp[i]->ecg_filter != NULL)
    {
      p += sprintf(scratchpad + p, "ECG:HR ");
    }

    if(signalcomp[i]->zratio_filter != NULL)
    {
      p += sprintf(scratchpad + p, "Z-ratio ");
    }

    for(;p<81; p++)
    {
      scratchpad[p] = ' ';
    }

    if(fwrite(scratchpad, 80, 1, outputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
      messagewindow.exec();
      fclose(outputfile);
      return;
    }
  }

  if(edfplus)
  {
    for(i=0; i<80; i++)  fputc(' ', outputfile);
  }

  for(i=0; i<signalcomps; i++)
  {
    fprintf(outputfile, "%-8i", signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record
     * duration_factor[signalcomp[i]->filenum]);
  }

  if(edfplus)
  {
    fprintf(outputfile, "%-8i", annot_smp_per_record);
  }

  for(i=0; i<signalcomps; i++)
  {
    for(j=0; j<32; j++)  fputc(' ', outputfile);
  }

  if(edfplus)
  {
    for(i=0; i<32; i++)  fputc(' ', outputfile);
  }

/////////////////////////////////////////////////////////////////////

  for(i=0; i<MAXSIGNALS; i++)  smpls_written[i] = 0;

  mainwindow->print_to_edf_active = 1;

  mainwindow->setup_viewbuf();

  viewbuf = mainwindow->viewbuf;

  if(viewbuf==NULL)
  {
    fclose(outputfile);
    return;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(records_written=0; records_written<records; records_written++)
  {
    qApp->processEvents();

    for(i=0; i<signalcomps; i++)
    {
      for(k=0; k<signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].smp_per_record * duration_factor[signalcomp[i]->filenum]; k++)
      {
        if(smpls_preamble[i])
        {
          smpls_preamble[i]--;

          fputc(null_bytes[i].two[0], outputfile);
          if(fputc(null_bytes[i].two[1], outputfile)==EOF)
          {
            QApplication::restoreOverrideCursor();
            QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
            messagewindow.exec();
            fclose(outputfile);
            return;
          }
        }
        else
        {
          dig_value = 0;

          s2 = smpls_written[i] + signalcomp[i]->sample_timeoffset - signalcomp[i]->sample_start;

          for(j=0; j<signalcomp[i]->num_of_signals; j++)
          {
            if((smpls_written[i]<signalcomp[i]->sample_stop)&&(s2>=0))
            {
              if(signalcomp[i]->edfhdr->edf)
              {
                temp = *(((short *)(
                  viewbuf
                  + signalcomp[i]->viewbufoffset
                  + (signalcomp[i]->edfhdr->recordsize * (s2 / signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record))
                  + signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].buf_offset))
                  + (s2 % signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].smp_per_record));
              }

              temp += signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
              temp *= signalcomp[i]->factor[j];

              temp -= signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[j]].offset;
            }
            else
            {
              temp = 0;
            }

            dig_value += temp;
          }

          if(signalcomp[i]->spike_filter)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime<=0)
              {
                reset_spike_filter(signalcomp[i]->spike_filter);
              }
              else
              {
                spike_filter_restore_buf(signalcomp[i]->spike_filter);
              }
            }

            dig_value = run_spike_filter(dig_value, signalcomp[i]->spike_filter);
          }

          for(p=0; p<signalcomp[i]->filter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime==0)
              {
                reset_filter(dig_value, signalcomp[i]->filter[p]);
              }
              else
              {
                signalcomp[i]->filter[p]->old_input = signalcomp[i]->filterpreset_a[p];
                signalcomp[i]->filter[p]->old_output = signalcomp[i]->filterpreset_b[p];
              }
            }

            dig_value = first_order_filter(dig_value, signalcomp[i]->filter[p]);
          }

          for(p=0; p<signalcomp[i]->ravg_filter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime!=0)
              {
                ravg_filter_restore_buf(signalcomp[i]->ravg_filter[p]);
              }
            }

            dig_value = run_ravg_filter(dig_value, signalcomp[i]->ravg_filter[p]);
          }

          for(p=0; p<signalcomp[i]->fidfilter_cnt; p++)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime!=0)
              {
                memcpy(signalcomp[i]->fidbuf[p], signalcomp[i]->fidbuf2[p], fid_run_bufsize(signalcomp[i]->fid_run[p]));
              }
            }

            dig_value = signalcomp[i]->fidfuncp[p](signalcomp[i]->fidbuf[p], dig_value);
          }

          if(signalcomp[i]->plif_ecg_filter)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime<=0)
              {
                plif_reset_subtract_filter(signalcomp[i]->plif_ecg_filter, 0);
              }
              else
              {
                plif_subtract_filter_state_copy(signalcomp[i]->plif_ecg_filter, signalcomp[i]->plif_ecg_filter_sav);
              }
            }

            dig_value = plif_run_subtract_filter(dig_value, signalcomp[i]->plif_ecg_filter);
          }

          if(signalcomp[i]->ecg_filter != NULL)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime != 0)
              {
                ecg_filter_restore_buf(signalcomp[i]->ecg_filter);
              }
            }

            dig_value = run_ecg_filter(dig_value, signalcomp[i]->ecg_filter);

            dig_value = (dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue * 65.536) - 32768.0;
          }

          if(signalcomp[i]->zratio_filter != NULL)
          {
            if(smpls_written[i]==signalcomp[i]->sample_start)
            {
              if(mainwindow->edfheaderlist[signalcomp[i]->filenum]->viewtime != 0)
              {
                zratio_filter_restore_buf(signalcomp[i]->zratio_filter);
              }
            }

            dig_value = run_zratio_filter(dig_value, signalcomp[i]->zratio_filter);

            dig_value = dig_value * signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].bitvalue * 32768.0;
          }

          dig_value *= signalcomp[i]->polarity;

          if((smpls_written[i]>=signalcomp[i]->sample_start)&&(smpls_written[i]<signalcomp[i]->sample_stop))
          {
            wr_var.one_signed = dig_value;

            if((signalcomp[i]->ecg_filter == NULL) && (signalcomp[i]->zratio_filter == NULL))
            {
              if(wr_var.one_signed>signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max)
              {
                wr_var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_max;
              }
              if(wr_var.one_signed<signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min)
              {
                wr_var.one_signed = signalcomp[i]->edfhdr->edfparam[signalcomp[i]->edfsignal[0]].dig_min;
              }
            }
            else
            {
              if(wr_var.one_signed > 32767)
              {
                wr_var.one_signed = 32767;
              }
              if(wr_var.one_signed < -32768)
              {
                wr_var.one_signed = -32768;
              }
            }

            fputc(wr_var.four[0], outputfile);
            if(fputc(wr_var.four[1], outputfile)==EOF)
            {
              QApplication::restoreOverrideCursor();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
              messagewindow.exec();
              fclose(outputfile);
              return;
            }
          }
          else
          {
            fputc(null_bytes[i].two[0], outputfile);
            if(fputc(null_bytes[i].two[1], outputfile)==EOF)
            {
              QApplication::restoreOverrideCursor();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while writing to outputfile.");
              messagewindow.exec();
              fclose(outputfile);
              return;
            }
          }

          smpls_written[i]++;
        }
      }
    }

    if(edfplus)
    {
      tallen = fprintf(outputfile, "+%i.%07i",
      (int)(taltime / TIME_DIMENSION),
      (int)(taltime % TIME_DIMENSION));

      fputc(20, outputfile);
      fputc(20, outputfile);
      fputc(0, outputfile);

      tallen += 3;

      while(annotations_left)
      {
        while((annot_list_sz < 1) || (annot_cnt >= annot_list_sz))
        {
          annotationlist_nr++;
          if(annotationlist_nr>=mainwindow->files_open)
          {
            annotations_left = 0;
            annot_list = NULL;
            annot_list_sz = 0;
            break;
          }
          annot_list = &mainwindow->edfheaderlist[annotationlist_nr]->annot_list;

          annot_list_sz = edfplus_annotation_size(annot_list);

          annot_cnt = 0;

          if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[annotationlist_nr]->utc_starttime) * TIME_DIMENSION;
          }

          if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime) * TIME_DIMENSION;
          }

          if((mainwindow->viewtime_sync==VIEWTIME_UNSYNCED) || (mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
          {
            annot_difftime = (referencetime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime) * TIME_DIMENSION;
            annot_difftime += (mainwindow->edfheaderlist[annotationlist_nr]->viewtime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime);
          }

          if(annotationlist_nr != mainwindow->sel_viewtime)
          {
            annot_difftime -= mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_offset;
            annot_difftime += mainwindow->edfheaderlist[annotationlist_nr]->starttime_offset;
          }
        }

        if(!annotations_left)  break;

        if((annot_list != NULL) && (annot_cnt < annot_list_sz))
        {
          annot_ptr = edfplus_annotation_get_item(annot_list, annot_cnt);

          l_temp = annot_ptr->onset - annot_difftime;

          if((l_temp >= 0LL) && (l_temp < (mainwindow->pagetime + TIME_DIMENSION)))
          {
            tallen += fprintf(outputfile, "%+i.%07i",
            (int)(l_temp / TIME_DIMENSION),
            (int)(l_temp % TIME_DIMENSION));

            if(annot_ptr->duration[0]!=0)
            {
              fputc(21, outputfile);
              tallen++;

              tallen += fprintf(outputfile, "%s", annot_ptr->duration);
            }

            fputc(20, outputfile);
            tallen++;

            tallen += fprintf(outputfile, "%s", annot_ptr->annotation);

            fputc(20, outputfile);
            fputc(0, outputfile);
            tallen += 2;

            annot_cnt++;

            break;
          }
          else
          {
            annot_cnt++;

            continue;
          }
        }
      }

      for(j=tallen; j<(annot_smp_per_record * 2); j++)
      {
        fputc(0, outputfile);
      }

      taltime += duration;
    }
  }

  QApplication::restoreOverrideCursor();

  fclose(outputfile);
}























