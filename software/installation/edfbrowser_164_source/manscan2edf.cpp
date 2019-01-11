/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#include "manscan2edf.h"


#define MBILINEBUFSZ 4096
#define MBIMAXEVENTS 10000
#define MBIMAXEVLEN 40



struct segment_prop_struct{
        long long data_offset;
        int sweeps;
        int channels;
        int samplefrequency;
        int sec_duration;
        char label[MAXSIGNALS][17];
        double gain[MAXSIGNALS];
        int startdate_year;
        int startdate_month;
        int startdate_day;
        int starttime_hour;
        int starttime_minute;
        int starttime_second;
        double hpf;
        double lpf;
        char recorder_version[128];
        char recorder_brand[128];
        char datafilename[MAX_PATH_LENGTH];
        long long starttime_offset;
        int ev_cnt;
        long long ev_onset[MBIMAXEVENTS];
        long long ev_duration[MBIMAXEVENTS];
        char ev_description[MBIMAXEVENTS][MBIMAXEVLEN + 1];
        };




UI_MANSCAN2EDFwindow::UI_MANSCAN2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Manscan to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 430, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 430, 100, 25);
  pushButton2->setText("Close");

  textEdit1 = new QTextEdit(myobjectDialog);
  textEdit1->setGeometry(20, 20, 560, 380);
  textEdit1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  sprintf(txt_string, "Manscan MICROAMPS binary data to EDF+ converter.\n");
  textEdit1->append(txt_string);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_MANSCAN2EDFwindow::SelectFileButton()
{
  FILE *header_inputfile=NULL,
       *data_inputfile=NULL;

  int i, j, k, n, tmp, hdl, chns=0, sf, blocks, *buf, progress_steps, segment_cnt, n_segments=0;

  long long filesize, ll_tmp;

  char header_filename[MAX_PATH_LENGTH],
       txt_string[2048],
       edf_filename[MAX_PATH_LENGTH],
       data_filename[MAX_PATH_LENGTH],
       scratchpad[MAX_PATH_LENGTH];

  double d_tmp;

  struct segment_prop_struct *segment_properties;

  union{
         int one;
         char four[4];
       } var;

  struct date_time_struct dtm_struct;

  pushButton1->setEnabled(false);

  strcpy(header_filename, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "MBI files (*.mbi *.MBI)").toLocal8Bit().data());

  if(!strcmp(header_filename, ""))
  {
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_opendir, header_filename, MAX_PATH_LENGTH);

  header_inputfile = fopeno(header_filename, "rb");
  if(header_inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.\n", header_filename);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    pushButton1->setEnabled(true);
    return;
  }

  get_filename_from_path(scratchpad, header_filename, MAX_PATH_LENGTH);

  snprintf(txt_string, 2048, "Read file: %s", scratchpad);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  fseeko(header_inputfile, 0LL, SEEK_END);
  filesize = ftello(header_inputfile);
  if(filesize<10)
  {
    textEdit1->append("Error, filesize is too small.\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  segment_properties = (struct segment_prop_struct *)malloc(sizeof(struct segment_prop_struct));
  if(segment_properties == NULL)
  {
    textEdit1->append("Malloc error (struct segment_prop_struct)");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  n_segments = get_number_of_segments(header_inputfile);

////////////////// start segment processing ///////////////////////////////////////////

  for(segment_cnt=0; segment_cnt < n_segments; segment_cnt++)
  {
    memset(segment_properties, 0, sizeof(struct segment_prop_struct));

    for(i=0; i<MAXSIGNALS; i++)
    {
      sprintf(segment_properties->label[i], "ch.%i", i + 1);
      segment_properties->gain[i] = 1.0;
    }

    segment_properties->samplefrequency = 256;

    if(get_worddatafile(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"WordDataFile\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_channel_gain(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Channel gain\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_start_date(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Startdate\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_sample_rate(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"SampleRate\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_filter_settings(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Filtersettings\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_recorder_version(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Filtersettings\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_starttime_offset(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Starttime offset\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(get_events(segment_properties, segment_cnt, header_inputfile))
    {
      textEdit1->append("Error, can not find or process item \"Events\".\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    segment_properties->sec_duration = segment_properties->sweeps / segment_properties->samplefrequency;

  // for(i=0; i<segment_properties->ev_cnt; i++)
  // {
  //   printf("%s  %lli  %lli\n", segment_properties->ev_description[i], segment_properties->ev_onset[i], segment_properties->ev_duration[i]);
  // }
  //
  // for(i=0; i<segment_properties->channels; i++)
  // {
  //   printf("%s    %.10f\n", segment_properties->label[i], segment_properties->gain[i]);
  // }
  // printf("data_offset is %lli   sweeps is %i datafileheader_filename is %s\n", segment_properties->data_offset, segment_properties->sweeps, segment_properties->datafilename);
  //
  // printf("startdate_day is %i   startdate_month is %i   startdate_year is %i\n", segment_properties->startdate_day, segment_properties->startdate_month, segment_properties->startdate_year);
  //
  // printf("starttime is:  %02i:%02i:%02i    starttime offset is: %lli\n", segment_properties->starttime_hour, segment_properties->starttime_minute, segment_properties->starttime_second, segment_properties->starttime_offset);
  //
  // printf("samplerate is: %i   hpf is %f   lpf is %f\n", segment_properties->samplefrequency, segment_properties->hpf, segment_properties->lpf);
  //
  // printf("recorder version: %s   recorder brand: %s\n", segment_properties->recorder_version, segment_properties->recorder_brand);
  //
  // printf("recording duration: %i seconds\n", segment_properties->sec_duration);

    chns = segment_properties->channels;

    sf = segment_properties->samplefrequency;

    blocks = segment_properties->sec_duration;

    if(blocks < 1)
    {
      snprintf(txt_string, 2048, "Skipping segment %i because it's shorter dan one second.", segment_cnt + 1);
      textEdit1->append(txt_string);

      continue;
    }

    buf = (int *)malloc(sf * chns * sizeof(int));
    if(buf == NULL)
    {
      textEdit1->append("Malloc() error (buf)\n");
      fclose(header_inputfile);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    strcpy(edf_filename, header_filename);

    remove_extension_from_filename(edf_filename);

    if(n_segments > 1)
    {
      sprintf(edf_filename + strlen(edf_filename), "_%02i", segment_cnt + 1);
    }

    strcat(edf_filename, ".edf");

    get_directory_from_path(data_filename, header_filename, MAX_PATH_LENGTH - 1);

    strcat(data_filename, "/");

    strcat(data_filename, segment_properties->datafilename);

    data_inputfile = fopeno(data_filename, "rb");
    if(data_inputfile==NULL)
    {
      snprintf(txt_string, 2048, "Can not open file %s for reading.\n", data_filename);
      textEdit1->append(QString::fromLocal8Bit(txt_string));
      fclose(header_inputfile);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    fseeko(data_inputfile, 0LL, SEEK_END);
    filesize = ftello(data_inputfile);
    if(filesize < (segment_properties->data_offset + (chns * 2)))
    {
      textEdit1->append("Error, filesize is too small.\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    n = (filesize - segment_properties->data_offset) / (chns * 2);
    if(n < segment_properties->sweeps)
    {
      segment_properties->sweeps = n;

      snprintf(txt_string, 2048, "Warning: samples in datafile are less than in headerfile.");
      textEdit1->append(txt_string);
    }

    get_filename_from_path(scratchpad, edf_filename, MAX_PATH_LENGTH - 1);

    snprintf(txt_string, 2048, "Convert a segment to %s ...", scratchpad);
    textEdit1->append(txt_string);

    hdl = edfopen_file_writeonly(edf_filename, EDFLIB_FILETYPE_EDFPLUS, chns);

    if(hdl<0)
    {
      snprintf(txt_string, 2048, "Can not open file %s for writing.\n", edf_filename);
      textEdit1->append(QString::fromLocal8Bit(txt_string));
      fclose(header_inputfile);
      fclose(data_inputfile);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_samplefrequency(hdl, i, sf))
      {
        textEdit1->append("Error: edf_set_samplefrequency()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_minimum(hdl, i, -32768))
      {
        textEdit1->append("Error: edf_set_digital_minimum()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_digital_maximum(hdl, i, 32767))
      {
        textEdit1->append("Error: edf_set_digital_maximum()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_label(hdl, i, segment_properties->label[i]))
      {
        textEdit1->append("Error: edf_set_label()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    for(i=0; i<chns; i++)
    {
      if(segment_properties->gain[i] > 0.999)
      {
        d_tmp = 32767.0 / segment_properties->gain[i];

        strcpy(scratchpad, "uV");
      }
      else
      {
        d_tmp = 32.767 / segment_properties->gain[i];

        strcpy(scratchpad, "mV");
      }

      if(edf_set_physical_dimension(hdl, i, scratchpad))
      {
        textEdit1->append("Error: edf_set_physical_dimension()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }

      if(edf_set_physical_maximum(hdl, i, d_tmp))
      {
        textEdit1->append("Error: edf_set_physical_maximum()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }

      if(edf_set_physical_minimum(hdl, i, -(d_tmp)))
      {
        textEdit1->append("Error: edf_set_physical_minimum()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    dtm_struct.year = segment_properties->startdate_year;
    dtm_struct.month = segment_properties->startdate_month;
    dtm_struct.day = segment_properties->startdate_day;
    dtm_struct.hour = segment_properties->starttime_hour;
    dtm_struct.minute = segment_properties->starttime_minute;
    dtm_struct.second = segment_properties->starttime_second;

    date_time_to_utc(&ll_tmp, dtm_struct);

    ll_tmp += segment_properties->starttime_offset / TIME_DIMENSION;

    utc_to_date_time(ll_tmp, &dtm_struct);

    if(edf_set_startdatetime(hdl, dtm_struct.year, dtm_struct.month, dtm_struct.day, dtm_struct.hour, dtm_struct.minute, dtm_struct.second))
    {
      textEdit1->append("Error: edf_set_startdatetime()\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      edfclose_file(hdl);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_equipment(hdl, segment_properties->recorder_brand))
    {
      textEdit1->append("Error: edf_set_equipment()\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      edfclose_file(hdl);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_recording_additional(hdl, segment_properties->recorder_version))
    {
      textEdit1->append("Error: edf_set_recording_additional()\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      edfclose_file(hdl);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    scratchpad[0] = 0;

    if(segment_properties->hpf > 0.0001)
    {
      sprintf(scratchpad + strlen(scratchpad), "HP:%f", segment_properties->hpf);

      remove_trailing_zeros(scratchpad);

      strcat(scratchpad, "Hz ");
    }

    if(segment_properties->lpf > 0.0001)
    {
      sprintf(scratchpad + strlen(scratchpad), "LP:%f", segment_properties->lpf);

      remove_trailing_zeros(scratchpad);

      strcat(scratchpad, "Hz ");
    }

    for(i=0; i<chns; i++)
    {
      if(edf_set_prefilter(hdl, i, scratchpad))
      {
        textEdit1->append("Error: edf_set_prefilter()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

  ////////////////// start conversion ///////////////////////////////////////////

    QProgressDialog progress("Converting a segment...", "Abort", 0, blocks);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(200);

    progress_steps = blocks / 100;
    if(progress_steps < 1)
    {
      progress_steps = 1;
    }

    fseeko(data_inputfile, segment_properties->data_offset, SEEK_SET);

    for(i=0; i<blocks; i++)
    {
      if(!(i%progress_steps))
      {
        progress.setValue(i);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(header_inputfile);
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          free(segment_properties);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(j=0; j<sf; j++)
      {
        for(k=0; k<chns; k++)
        {
          var.four[2] = fgetc(data_inputfile);
          tmp = fgetc(data_inputfile);
          if(tmp == EOF)
          {
            progress.reset();
            textEdit1->append("A read error occurred during conversion.\n");
            fclose(header_inputfile);
            fclose(data_inputfile);
            edfclose_file(hdl);
            free(buf);
            free(segment_properties);
            pushButton1->setEnabled(true);
            return;
          }
          var.four[3] = tmp;

          buf[(k * sf) + j] = (var.one >> 16);
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    progress.reset();

////////////////// end of conversion ///////////////////////////////////////////

    if(edfwrite_annotation_latin1(hdl, 0LL, -1LL, "Recording starts"))
    {
      textEdit1->append("Error: edfwrite_annotation_latin1()\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      edfclose_file(hdl);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    for(i=0; i<segment_properties->ev_cnt; i++)
    {
      if(edfwrite_annotation_latin1(hdl, segment_properties->ev_onset[i], segment_properties->ev_duration[i], segment_properties->ev_description[i]))
      {
        textEdit1->append("Error: edfwrite_annotation_latin1()\n");
        fclose(header_inputfile);
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        free(segment_properties);
        pushButton1->setEnabled(true);
        return;
      }
    }

    if(edfwrite_annotation_latin1(hdl, (long long)blocks * 10000LL, -1LL, "Recording ends"))
    {
      textEdit1->append("Error: edfwrite_annotation_latin1()\n");
      fclose(header_inputfile);
      fclose(data_inputfile);
      edfclose_file(hdl);
      free(buf);
      free(segment_properties);
      pushButton1->setEnabled(true);
      return;
    }

    fclose(data_inputfile);

    edfclose_file(hdl);

    free(buf);

    textEdit1->append("... conversion of segment done.");
  }

////////////////// end of segment processing ///////////////////////////////////////////

  fclose(header_inputfile);

  free(segment_properties);

  textEdit1->append("Ready.\n");

  pushButton1->setEnabled(true);
}


int UI_MANSCAN2EDFwindow::get_events(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int i,
      order=0,
      startsmpl,
      durationsmpl,
      ev_cnt=0,
      segment_cnt=0;

  char linebuf[MBILINEBUFSZ],
       annot_descr[MBIMAXEVLEN + 1];

  long long onset,
            duration,
            sf;


  sf = segprop->samplefrequency;

  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return 0;
    }

    strntolower(linebuf, 33);

    if((!(strncmp(linebuf, "event beginsample durationsamples", 33))) ||
      (!(strncmp(linebuf, "event durationsamples beginsample", 33))))
    {
      if(!(strncmp(linebuf, "event beginsample durationsamples", 33)))
      {
        order = 1;
      }
      else
      {
        order = 0;
      }

      if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
      {
        return 0;
      }

      strntolower(linebuf, 22);

      if(!(strncmp(linebuf, "string integer integer", 22)))
      {
        for(i=0; ; i++)
        {
          if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
          {
            return -1;
          }

          if(strlen(linebuf) == 0)
          {
            break;
          }

          if(strlen(linebuf) < 5)
          {
            return -1;
          }

          if(order)
          {
            if(sscanf(linebuf, "%40s %i %i", annot_descr, &startsmpl, &durationsmpl) != 3)
            {
              return -1;
            }
          }
          else
          {
            if(sscanf(linebuf, "%40s %i %i", annot_descr, &durationsmpl, &startsmpl) != 3)
            {
              return -1;
            }
          }

          if(startsmpl < 0)
          {
            continue;
          }

          onset = ((long long)startsmpl * 10000LL) / sf;

          if(durationsmpl < 2)
          {
            duration = -1LL;
          }
          else
          {
            duration = ((long long)durationsmpl * 10000LL) / sf;
          }

          strcpy(segprop->ev_description[ev_cnt], annot_descr);

          segprop->ev_onset[ev_cnt] = onset;

          segprop->ev_duration[ev_cnt] = duration;

          segprop->ev_cnt = ++ev_cnt;

          if(ev_cnt >= MBIMAXEVENTS)
          {
            return 0;
          }
        }
      }
      else
      {
        continue;
      }
    }
  }

  return 0;
}


int UI_MANSCAN2EDFwindow::get_starttime_offset(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int segment_cnt=0;

  char linebuf[MBILINEBUFSZ];

  long long time;


  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return 0;
    }

    strntolower(linebuf, 24);

    if(!(strncmp(linebuf, "offsetdisplayexperiment ", 24)))
    {
      break;
    }
  }

  if(strlen(linebuf) < 25)
  {
    return -1;
  }

  time = get_long_time(linebuf + 24);

  segprop->starttime_offset = time;

  return 0;
}


int UI_MANSCAN2EDFwindow::get_recorder_version(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int segment_cnt=0;

  char linebuf[MBILINEBUFSZ];


  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return 0;
    }

    strntolower(linebuf, 25);

    if(!(strncmp(linebuf, "comment recorder version ", 25)))
    {
      break;
    }
  }

  if(strlen(linebuf) < 26)
  {
    return -1;
  }

  strncpy(segprop->recorder_version, linebuf + 25, 100);

  return 0;
}


int UI_MANSCAN2EDFwindow::get_filter_settings(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int segment_cnt=0;

  char linebuf[MBILINEBUFSZ],
       *chrpntr;

  double lpf, hpf;


  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return 0;
    }

    strntolower(linebuf, 18);

    if(!(strncmp(linebuf, "comment microamps ", 18)))
    {
      break;
    }
  }

  if(strlen(linebuf) < 53)
  {
    return -1;
  }

  strncpy(segprop->recorder_brand, linebuf + 8, 13);

  chrpntr = linebuf + 18;

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  if(strncmp(chrpntr, "highpass ", 9))
  {
    return -1;
  }

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  hpf = atof(chrpntr);

  if((hpf < 0.000001) || (hpf > 100000.0))
  {
    return -1;
  }

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  if(strncmp(chrpntr, "hz, ", 4))
  {
    return -1;
  }

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  if(strncmp(chrpntr, "lowpass ", 8))
  {
    return -1;
  }

  chrpntr++;

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  lpf = atof(chrpntr);

  if((lpf < 0.000001) || (lpf > 100000.0))
  {
    return -1;
  }

  chrpntr = strchr(chrpntr, ' ');
  if(chrpntr == NULL)
  {
    return -1;
  }

  chrpntr++;

  if(strncmp(chrpntr, "hz", 2))
  {
    return -1;
  }

  segprop->hpf = hpf;
  segprop->lpf = lpf;

  return 0;
}


int UI_MANSCAN2EDFwindow::get_sample_rate(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int sf,
      segment_cnt=0;

  char linebuf[MBILINEBUFSZ];

  double d_tmp;


  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return 0;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "samplerate ", 11)))
    {
      if(strlen(linebuf) < 12)
      {
        return -1;
      }

      break;
    }

    strntolower(linebuf, 16);

    if(!(strncmp(linebuf, "sampletodisplay ", 16)))
    {
      if(strlen(linebuf) < 17)
      {
        return -1;
      }

      break;
    }
  }

  if(!(strncmp(linebuf, "samplerate ", 11)))
  {
    sf = atoi(linebuf + 11);
  }
  else
  {
    d_tmp = atof(linebuf + 16);

    if(d_tmp < 0.00001)
    {
      return -1;
    }

    d_tmp = 1.0 / d_tmp;

    sf = d_tmp;

    d_tmp -= sf;

    if(d_tmp > 0.5)
    {
      sf++;
    }
  }

  if((sf < 1) || (sf > 100000))
  {
    return -1;
  }

  segprop->samplefrequency = sf;

  return 0;
}


int UI_MANSCAN2EDFwindow::get_start_date(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int p, p2, len,
      segment_cnt=0,
      starttime_second,
      starttime_minute,
      starttime_hour,
      startdate_day,
      startdate_month,
      startdate_year;

  char linebuf[MBILINEBUFSZ];


  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return -1;
    }

    strntolower(linebuf, 15);

    if(!(strncmp(linebuf, "experimenttime ", 15)))
    {
      break;
    }
  }

  len = strlen(linebuf);

  if(len < 23)
  {
    return -1;
  }

  p = 15;

  if(((linebuf[p+2] != '/') || (linebuf[p+5] != '/')) && ((linebuf[p+2] != '-') || (linebuf[p+5] != '-')))
  {
    return -1;
  }

  if((linebuf[p]   < '0') || (linebuf[p]   > '9') ||
     (linebuf[p+1] < '0') || (linebuf[p+1] > '9') ||
     (linebuf[p+3] < '0') || (linebuf[p+3] > '9') ||
     (linebuf[p+4] < '0') || (linebuf[p+4] > '9') ||
     (linebuf[p+6] < '0') || (linebuf[p+6] > '9') ||
     (linebuf[p+7] < '0') || (linebuf[p+7] > '9'))
  {
    return -1;
  }

  if((len - p) > 9)
  {
    if((linebuf[p+8] >= '0') && (linebuf[p+8] <= '9') && (linebuf[p+9] >= '0') && (linebuf[p+9] <= '9'))
    {
      startdate_year = atoi(linebuf + p + 6);

      p2 = p + 10;
    }
    else
    {
      startdate_year = atoi(linebuf + p + 6);
      if(startdate_year >= 70)
      {
        startdate_year += 1900;
      }
      else
      {
        startdate_year += 2000;
      }

      p2 = p + 8;
    }
  }
  else
  {
    startdate_year = atoi(linebuf + p + 6);
    if(startdate_year >= 70)
    {
      startdate_year += 1900;
    }
    else
    {
      startdate_year += 2000;
    }

    p2 = p + 8;
  }

  startdate_day = atoi(linebuf + p + 3);

  startdate_month = atoi(linebuf + p);

  if((startdate_day < 1) || (startdate_day > 31) || (startdate_month < 1) || (startdate_month > 12))
  {
    return -1;
  }

  segprop->startdate_day = startdate_day;
  segprop->startdate_month = startdate_month;
  segprop->startdate_year = startdate_year;

  p = p2;

  if(linebuf[p] == ' ')
  {
    p++;

    if((len - p) > 7)
    {
      if((linebuf[p+2] == ':') && (linebuf[p+5] == ':'))
      {
        if((linebuf[p]   < '0') || (linebuf[p]   > '9') ||
           (linebuf[p+1] < '0') || (linebuf[p+1] > '9') ||
           (linebuf[p+3] < '0') || (linebuf[p+3] > '9') ||
           (linebuf[p+4] < '0') || (linebuf[p+4] > '9') ||
           (linebuf[p+6] < '0') || (linebuf[p+6] > '9') ||
           (linebuf[p+7] < '0') || (linebuf[p+7] > '9'))
        {
          return -1;
        }

        starttime_hour   = atoi(linebuf + p);
        starttime_minute = atoi(linebuf + p + 3);
        starttime_second = atoi(linebuf + p + 6);

        if((starttime_hour < 0) || (starttime_hour > 23))
        {
          starttime_hour = 0;
          starttime_minute = 0;
          starttime_second = 0;
          return -1;
        }

        if((starttime_minute < 0) || (starttime_minute > 59))
        {
          starttime_hour = 0;
          starttime_minute = 0;
          starttime_second = 0;
          return -1;
        }

        if((starttime_second < 0) || (starttime_second > 59))
        {
          starttime_hour = 0;
          starttime_minute = 0;
          starttime_second = 0;
          return -1;
        }

        segprop->starttime_hour = starttime_hour;
        segprop->starttime_minute = starttime_minute;
        segprop->starttime_second = starttime_second;
      }
    }
  }

  return 0;
}


int UI_MANSCAN2EDFwindow::get_channel_gain(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int i, j, len,
      segment_cnt=0,
      channels,
      column;

  char linebuf[MBILINEBUFSZ],
       *chrpntr,
       label[17];

  double gain;

  channels = segprop->channels;

  if((channels < 1) || (channels > MAXSIGNALS))
  {
    return -1;
  }

  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 256);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return -1;
    }

    if(!(strncmp(linebuf, "channel ", 8)))
    {
      if(strstr(linebuf, "unitdisplaytofile") != NULL)
      {
        break;
      }
    }
  }

  chrpntr = linebuf;

  for(column=1; ; column++)
  {
    chrpntr = strchr(chrpntr, ' ') + 1;

    if((chrpntr == NULL) || (strlen(chrpntr) < 17))
    {
      return -1;
    }

    if(!(strncmp(chrpntr, "unitdisplaytofile", 17)))
    {
      break;
    }
  }

  for(i=0; i<channels; i++)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    chrpntr = strchr(linebuf, ' ');
    if(chrpntr == NULL)
    {
      return -1;
    }

    len = chrpntr - linebuf;

    if(len < 1)
    {
      return -1;
    }

    if(len > 16)
    {
      len = 16;
    }

    strncpy(label, linebuf, len);

    label[len] = 0;

    chrpntr++;

    for(j=1; j<column; j++)
    {
      chrpntr = strchr(chrpntr, ' ');
      if(chrpntr == NULL)
      {
        return -1;
      }

      chrpntr++;
    }

    if(chrpntr >= (linebuf + strlen(linebuf)))
    {
      return -1;
    }

    gain = atof(chrpntr);

    for(j=0; j<channels; j++)
    {
      if(!(strcmp(label, segprop->label[j])))
      {
        segprop->gain[j] = gain;

        break;
      }
    }
  }

  return 0;
}


int UI_MANSCAN2EDFwindow::get_worddatafile(struct segment_prop_struct *segprop, int segment, FILE *inputfile)
{
  int i, j, p, len, tmp,
      segment_cnt=0,
      channels;

  char linebuf[MBILINEBUFSZ],
       scratchpad[256];

  rewind(inputfile);

  while(segment_cnt != segment)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      return -1;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      return -1;
    }

    strntolower(linebuf, 12);

    if(!(strncmp(linebuf, "worddatafile", 12)))
    {
      break;
    }
  }

  if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
  {
    return -1;
  }

  channels = 0;
  j = 0;

  for(i=0; ; i++)
  {
    if(linebuf[i] == 0)
    {
      if(j > 0)
      {
        if(j > 16)
        {
          j = 16;
        }

        segprop->label[channels][j] = 0;

        channels++;
      }

      break;
    }

    if(linebuf[i] == ' ')
    {
      if(j > 0)
      {
        if(j > 16)
        {
          j = 16;
        }

        segprop->label[channels][j] = 0;

        channels++;
      }

      j = 0;

      continue;
    }

    if(channels >= MAXSIGNALS)
    {
      return -1;
    }

    if(j < 16)
    {
      segprop->label[channels][j++] = linebuf[i];
    }
  }

  if(channels < 1)
  {
    return -1;
  }

  segprop->channels = channels;

  if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)  // data offset, sweeps, filename
  {
    return -1;
  }

  if(strlen(linebuf) < 9)
  {
    return -1;
  }

  for(len=0; ; len++)
  {
    if((linebuf[len] == ' ') || (linebuf[len] == 0))
    {
      break;
    }
  }

  if((len < 1) || (len > 255))
  {
    return -1;
  }

  strncpy(scratchpad, linebuf, len);
  scratchpad[len] = 0;
  tmp = atoi(scratchpad);  // field 1

  for(p=len; ; p++)
  {
    if((linebuf[p] != ' ') || (linebuf[p] == 0))
    {
      break;
    }
  }

  for(len=0; ; len++)
  {
    if((linebuf[p+len] == ' ') || (linebuf[p+len] == 0))
    {
      break;
    }
  }

  if((len < 1) || (len > 255))
  {
    return -1;
  }

  strncpy(scratchpad, linebuf + p, len);
  scratchpad[len] = 0;
  segprop->data_offset = atoi(scratchpad);  // field 2

  segprop->data_offset <<= 32;
  segprop->data_offset += tmp;

  for(p+=len; ; p++)
  {
    if((linebuf[p] != ' ') || (linebuf[p] == 0))
    {
      break;
    }
  }

  for(len=0; ; len++)
  {
    if((linebuf[p+len] == ' ') || (linebuf[p+len] == 0))
    {
      break;
    }
  }

  if((len < 1) || (len > 255))
  {
    return -1;
  }

  strncpy(scratchpad, linebuf + p, len);
  scratchpad[len] = 0;
  segprop->sweeps = atoi(scratchpad);  // field 3

  for(p+=len; ; p++)
  {
    if((linebuf[p] != ' ') || (linebuf[p] == 0))
    {
      break;
    }
  }

  for(len=0; ; len++)
  {
    if((linebuf[p+len] == ' ') || (linebuf[p+len] == 0))
    {
      break;
    }
  }

  if((len < 3) || (len > 255))
  {
    return -1;
  }

  strncpy(scratchpad, linebuf + p, len);
  scratchpad[len] = 0;
  strcpy(segprop->datafilename, scratchpad);  // field 4

  return 0;
}


int UI_MANSCAN2EDFwindow::get_number_of_segments(FILE *inputfile)
{
  int segment_cnt=1;

  char linebuf[MBILINEBUFSZ];

  rewind(inputfile);

  while(1)
  {
    if(fgetline(linebuf, MBILINEBUFSZ, inputfile) == NULL)
    {
      break;
    }

    strntolower(linebuf, 11);

    if(!(strncmp(linebuf, "datatypeid ", 11)))
    {
      segment_cnt++;
    }
  }

  return segment_cnt;
}


char * UI_MANSCAN2EDFwindow::fgetline(char *s, int size, FILE *stream)
{
  int i, j, n,
      spc1=0,
      spc2=0;

  char *result;

  result = fgets(s, size, stream);
  if(result == NULL)
  {
    return result;
  }

  for(i=0; ; i++)
  {
    if(s[i] == 0)
    {
      break;
    }

    if(s[i] == '\t')
    {
      s[i] = ' ';
    }

    if((s[i] == '\n') || (s[i] == '\r'))
    {
      s[i] = 0;

      break;
    }
  }

  remove_leading_spaces(s);

  for(i=0; ; i++)
  {
    if(s[i] == ' ')
    {
      if((spc1 == 0) && (spc2 == 0))
      {
        spc1 = i;
      }

      spc2 = i;
    }
    else
    {
      if(spc1 != spc2)
      {
        if(s[i] == 0)
        {
          s[spc1] = 0;

          return result;
        }

        n = spc2 - spc1;

        for(j=spc1; ; j++)
        {
          s[j] = s[j+n];

          if(s[j] == 0)
          {
            break;
          }
        }
      }

      spc1 = 0;
      spc2 = 0;
    }

    if(s[i] == 0)
    {
      break;
    }
  }

  return result;
}


long long UI_MANSCAN2EDFwindow::get_long_time(const char *str)
{
  int i, j=0, len=0, hasdot=0, dotposition=0, decimals=0;

  long long value=0LL, radix, sign=1LL;


  for(radix = TIME_DIMENSION; radix > 1LL; radix /=10)
  {
    decimals++;
  }

  if(str[0] == '+')
  {
    str++;
  }
  else
  {
    if(str[0] == '-')
    {
      sign = -1LL;

      str++;
    }
  }

  for(i=0; ; i++)
  {
    if(((str[i]<'0') || (str[i]>'9')) && (str[i]!='.'))
    {
      len = i;
      break;
    }

    if(str[i]=='.')
    {
      if(hasdot)
      {
        len = i;
        break;
      }

      hasdot = 1;
      dotposition = i;
    }
    else
    {
      if(hasdot)
      {
        if(++j > decimals)
        {
          len = i;
          break;
        }
      }
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

  return value * sign;
}

























