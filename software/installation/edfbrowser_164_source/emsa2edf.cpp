/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#include "emsa2edf.h"


UI_EMSA2EDFwindow::UI_EMSA2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Emsa to EDF+ converter");
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
  sprintf(txt_string, "Emsa to EDF+ converter.\n");
  textEdit1->append(txt_string);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_EMSA2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL,
       *outputfile=NULL,
       *logfile=NULL;

  int i, j, k,
      channels,
      record_size,
      samplefrequency,
      hdrsize,
      samples_in_record,
      bufsize,
      max_buf_records,
      raster,
      data_records,
      records_in_buf,
      error,
      len,
      seq_nr,
      year,
      b_year,
      b_date,
      seconds,
      deci_seconds,
      datrecduris1,
      gender,
      tallen=0,
      annot_cnt=0,
      patientcode=0,
      annots_written=0,
      time_on,
      time_off,
      duration_present=0;

  long long filesize;

  char path[MAX_PATH_LENGTH],
       scratchpad[256],
       *buf=NULL,
       date[9],
       time[9],
       month[4],
       patientname[128],
       b_month[4],
       *p,
       equipment[41],
       handedness[16],
       weight[16],
       birthdate[16],
       txt_string[2048],
       outputfilename[MAX_PATH_LENGTH];

  union{
         int number;
         char str[4];
       } var;

  logbuf = NULL;

  pushButton1->setEnabled(false);

  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "PLG files (*.plg *.PLG)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

/***************** check header ******************************/

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.\n", path);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 0LL, SEEK_END);
  filesize = ftello(inputfile);
  if(filesize<1538)
  {
    textEdit1->append("Error, filesize is too small.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  rewind(inputfile);
  if(fread(scratchpad, 3, 1, inputfile)!=1)
  {
    textEdit1->append("Read error.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }
  if((strncmp(scratchpad, "PBJ", 3))&&(strncmp(scratchpad, "BPC", 3)))
  {
    textEdit1->append("Error, file id does not match.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 3LL, SEEK_SET);
  channels = fgetc(inputfile);
  if(channels==EOF)
  {
    textEdit1->append("Read error.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }
  if(!channels)
  {
    textEdit1->append("Error, number of channels is 0.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  hdrsize = 1024 + (512 * channels);

  if(filesize<(hdrsize + (2 * channels)))
  {
    textEdit1->append("Error, filesize is too small.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 10LL, SEEK_SET);
  samplefrequency = 256 * fgetc(inputfile);
  fseeko(inputfile, 9LL, SEEK_SET);
  samplefrequency += fgetc(inputfile);
  if(!samplefrequency)
  {
    textEdit1->append("Error, samplefrequency is 0 Hz.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(filesize<(hdrsize + (2 * channels * samplefrequency)))
  {
    textEdit1->append("Error, filesize is too small.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(get_string(scratchpad, inputfile, 205, 32)!=8)
  {
    textEdit1->append("Error, can not read startdate.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }
  error = 0;
  if((scratchpad[0]<'0')||(scratchpad[0]>'9'))  error = 1;
  if((scratchpad[1]<'0')||(scratchpad[1]>'9'))  error = 1;
  date[6] = scratchpad[2];
  date[7] = scratchpad[3];
  date[3] = scratchpad[4];
  date[4] = scratchpad[5];
  date[0] = scratchpad[6];
  date[1] = scratchpad[7];
  date[2] = 0;
  date[5] = 0;
  date[8] = 0;
  if((date[0]<'0')||(date[0]>'9'))  error = 1;
  if((date[1]<'0')||(date[1]>'9'))  error = 1;
  if((date[3]<'0')||(date[3]>'9'))  error = 1;
  if((date[4]<'0')||(date[4]>'9'))  error = 1;
  if((date[6]<'0')||(date[6]>'9'))  error = 1;
  if((date[7]<'0')||(date[7]>'9'))  error = 1;
  scratchpad[4] = 0;
  year = atoi(scratchpad);

  if(get_string(scratchpad, inputfile, 214, 32)!=8)
  {
    textEdit1->append("Error, can not read starttime.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }
  strcpy(time, scratchpad);
  if(time[2] != ':')  error = 1;
  if(time[5] != ':')  error = 1;
  time[2] = 0;
  time[5] = 0;
  time[8] = 0;
  if((time[0]<'0')||(time[0]>'9'))  error = 1;
  if((time[1]<'0')||(time[1]>'9'))  error = 1;
  if((time[3]<'0')||(time[3]>'9'))  error = 1;
  if((time[4]<'0')||(time[4]>'9'))  error = 1;
  if((time[6]<'0')||(time[6]>'9'))  error = 1;
  if((time[7]<'0')||(time[7]>'9'))  error = 1;
  if((atoi(date)<1)||(atoi(date)>31))  error = 1;
  if((atoi(date+3)<1)||(atoi(date+3)>12))  error = 1;
  if(atoi(time)>23)  error = 1;
  if(atoi(time+3)>59)  error = 1;
  if(atoi(time+6)>59)  error = 1;
  if(error)
  {
    textEdit1->append("Error, can not read startdate or starttime.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }
  date[2] = '.';
  date[5] = '.';
  time[2] = '.';
  time[5] = '.';

  i = atoi(date+3);
  switch(i)
  {
    case  1 : strcpy(month, "JAN");
              break;
    case  2 : strcpy(month, "FEB");
              break;
    case  3 : strcpy(month, "MAR");
              break;
    case  4 : strcpy(month, "APR");
              break;
    case  5 : strcpy(month, "MAY");
              break;
    case  6 : strcpy(month, "JUN");
              break;
    case  7 : strcpy(month, "JUL");
              break;
    case  8 : strcpy(month, "AUG");
              break;
    case  9 : strcpy(month, "SEP");
              break;
    case 10 : strcpy(month, "OCT");
              break;
    case 11 : strcpy(month, "NOV");
              break;
    case 12 : strcpy(month, "DEC");
              break;
    default : strcpy(month, "ERR");
              break;
  }

  if(get_string(scratchpad, inputfile, 169, 32)!=8)
  {
    strcpy(birthdate, "X");
  }
  else
  {
    error = 0;
    for(i=0; i<8; i++)
    {
      if((scratchpad[i]<'0')||(scratchpad[i]>'9'))  error = 1;
    }
    scratchpad[8] = 0;
    b_date = atoi(scratchpad+6);
    if((b_date<1)||(b_date>31))  error = 1;
    scratchpad[6] = 0;
    i = atoi(scratchpad+4);
    if((i<1)||(i>12))  error = 1;
    switch(i)
    {
      case  1 : strcpy(b_month, "JAN");
                break;
      case  2 : strcpy(b_month, "FEB");
                break;
      case  3 : strcpy(b_month, "MAR");
                break;
      case  4 : strcpy(b_month, "APR");
                break;
      case  5 : strcpy(b_month, "MAY");
                break;
      case  6 : strcpy(b_month, "JUN");
                break;
      case  7 : strcpy(b_month, "JUL");
                break;
      case  8 : strcpy(b_month, "AUG");
                break;
      case  9 : strcpy(b_month, "SEP");
                break;
      case 10 : strcpy(b_month, "OCT");
                break;
      case 11 : strcpy(b_month, "NOV");
                break;
      case 12 : strcpy(b_month, "DEC");
                break;
      default : strcpy(b_month, "ERR");
                break;
    }
    scratchpad[4] = 0;
    b_year = atoi(scratchpad);
    if(error)
    {
      strcpy(birthdate, "X");
    }
    else
    {
      sprintf(birthdate, "%02i-%s-%04i", b_date, b_month, b_year);
    }
  }

  len = get_string(patientname, inputfile, 11, 30);
  if(len)
  {
    latin1_to_ascii(patientname, len);
    patientname[len] = 0;
    for(i=0; i<len; i++)
    {
      if(patientname[i]==' ')
      {
        patientname[i] = '_';
      }
    }
  }
  else
  {
    strcpy(patientname, "X");
  }

  fseeko(inputfile, 4LL, SEEK_SET);
  if(fread(var.str, 4, 1, inputfile)!=1)
  {
    textEdit1->append("Read error.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 8LL, SEEK_SET);
  seq_nr = fgetc(inputfile);
  if(seq_nr==EOF)
  {
    textEdit1->append("Read error.\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 203LL, SEEK_SET);
  gender = fgetc(inputfile);
  if((gender!='M')&&(gender!='F'))
  {
    gender = 'X';
  }

  len = fgetc(inputfile);
  switch(len)
  {
    case 'E' : strcpy(handedness, " lefthanded");  break;
    case 'D' : strcpy(handedness, " righthanded");  break;
    default  : handedness[0] = 0;  break;
  }

  len = get_string(weight + 1, inputfile, 351, 6);
  if(len)
  {
    latin1_to_ascii(weight + 1, len);

    weight[len] = 0;

    weight[0] = ' ';

    strcat(weight, " Kg");
  }
  else
  {
    weight[0] = 0;
  }

  len = get_string(equipment, inputfile, 309, 40);
  if(len)
  {
    latin1_to_ascii(equipment, len);
    equipment[len] = 0;
    for(i=0; i<len; i++)
    {
      if(equipment[i]==' ')
      {
        equipment[i] = '_';
      }
    }
  }
  else
  {
    strcpy(equipment, "X");
  }

  if(samplefrequency%10)
  {
    samples_in_record = samplefrequency;

    raster = samplefrequency * 2;

    datrecduris1 = 1;
  }
  else
  {
    samples_in_record = samplefrequency / 10;

    raster = (samplefrequency / 10) * 2;

    datrecduris1 = 0;
  }

  record_size = samples_in_record * channels * 2;

  data_records = (filesize - hdrsize) / record_size;

/***************** read logfile *****************************************/

  remove_extension_from_filename(path);
  strcat(path, ".LBK");

  logfile = fopeno(path, "rb");
  if(logfile==NULL)
  {
    remove_extension_from_filename(path);
    strcat(path, ".lbk");

    logfile = fopeno(path, "rb");
  }

  if(logfile!=NULL)
  {
    fseeko(logfile, 0LL, SEEK_END);
    annot_cnt = ftello(logfile) / 69;

    if(annot_cnt)
    {
      logbuf = (struct event_struct *)calloc(1, sizeof(struct event_struct[annot_cnt]));
      if(logbuf==NULL)
      {
        textEdit1->append("Malloc error. (logbuf)\n");
        fclose(inputfile);
        fclose(logfile);
        pushButton1->setEnabled(true);
        return;
      }

      for(i=0; i<annot_cnt; i++)
      {
        if(get_string(scratchpad, logfile, i * 69, 10)!=8)
        {
          textEdit1->append("Error, can not read event starttime in .LBK file.\n");
          fclose(inputfile);
          fclose(logfile);
          free(logbuf);
          return;
        }

        if(scratchpad[2] != ':')  error = 1;
        if(scratchpad[5] != ':')  error = 1;
        scratchpad[2] = 0;
        scratchpad[5] = 0;
        scratchpad[8] = 0;
        if((scratchpad[0]<'0')||(scratchpad[0]>'9'))  error = 1;
        if((scratchpad[1]<'0')||(scratchpad[1]>'9'))  error = 1;
        if((scratchpad[3]<'0')||(scratchpad[3]>'9'))  error = 1;
        if((scratchpad[4]<'0')||(scratchpad[4]>'9'))  error = 1;
        if((scratchpad[6]<'0')||(scratchpad[6]>'9'))  error = 1;
        if((scratchpad[7]<'0')||(scratchpad[7]>'9'))  error = 1;

        if(atoi(scratchpad)>23)  error = 1;
        if(atoi(scratchpad+3)>59)  error = 1;
        if(atoi(scratchpad+6)>59)  error = 1;
        if(error)
        {
          textEdit1->append("Error, can not read event starttime in .LBK file.\n");
          fclose(inputfile);
          fclose(logfile);
          free(logbuf);
          return;
        }

        time_on = atoi(scratchpad) * 3600;
        time_on += atoi(scratchpad+3) * 60;
        time_on += atoi(scratchpad+6);

        if(get_string(scratchpad, logfile, i * 69 + 9, 10)!=8)
        {
          textEdit1->append("Error, can not read event stoptime in .LBK file.\n");
          fclose(inputfile);
          fclose(logfile);
          free(logbuf);
          return;
        }

        if(scratchpad[2] != ':')  error = 1;
        if(scratchpad[5] != ':')  error = 1;
        scratchpad[2] = 0;
        scratchpad[5] = 0;
        scratchpad[8] = 0;
        if((scratchpad[0]<'0')||(scratchpad[0]>'9'))  error = 1;
        if((scratchpad[1]<'0')||(scratchpad[1]>'9'))  error = 1;
        if((scratchpad[3]<'0')||(scratchpad[3]>'9'))  error = 1;
        if((scratchpad[4]<'0')||(scratchpad[4]>'9'))  error = 1;
        if((scratchpad[6]<'0')||(scratchpad[6]>'9'))  error = 1;
        if((scratchpad[7]<'0')||(scratchpad[7]>'9'))  error = 1;

        if(atoi(scratchpad)>23)  error = 1;
        if(atoi(scratchpad+3)>59)  error = 1;
        if(atoi(scratchpad+6)>59)  error = 1;
        if(error)
        {
          textEdit1->append("Error, can not read event stoptime in .LBK file.\n");
          fclose(inputfile);
          fclose(logfile);
          free(logbuf);
          return;
        }

        time_off = atoi(scratchpad) * 3600;
        time_off += atoi(scratchpad+3) * 60;
        time_off += atoi(scratchpad+6);

        logbuf[i].duration = time_off - time_on;

        if(logbuf[i].duration<0)
        {
          textEdit1->append("Error, event duration in .LBK file is < 0.\n");
          fclose(inputfile);
          fclose(logfile);
          free(logbuf);
          return;
        }

        if(logbuf[i].duration)
        {
          duration_present = 1;
        }

        k = (atoi(time) * 3600);
        k += (atoi(time+3) * 60);
        k += atoi(time+6);
        logbuf[i].onset = time_on - k;
        if(logbuf[i].onset < 0)
        {
          logbuf[i].onset += 86400;
        }

        len = get_string(logbuf[i].description, logfile, i * 69 + 18, 40);

        if(len>2)
        {
          for(j=len-1; j>0; j--)
          {
            if(logbuf[i].description[j]==' ')
            {
              logbuf[i].description[j] = 0;
              len--;
            }
            else  break;
          }
        }

        latin1_to_utf8(logbuf[i].description, 80);

        len = strlen(logbuf[i].description);
        if(len>tallen)
        {
          tallen = len;
        }
      }

    }

    fclose(logfile);
  }
  else
  {
    textEdit1->append("Can not open LBK file, will continue without events.");
  }

  tallen += 14;

  if(annot_cnt)
  {
    tallen += 14;
  }

  if(duration_present)
  {
    tallen += 14;
  }

  if(tallen%2)
  {
    tallen++;
  }

/***************** write header *****************************************/

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);
  remove_extension_from_filename(outputfilename);
  strcat(outputfilename, ".edf");

  path[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(path, recent_savedir);
    strcat(path, "/");
  }
  strcat(path, outputfilename);

  strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    fclose(inputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopen(path, "wb");
  if(outputfile==NULL)
  {
    snprintf(txt_string, 2048, "Error, can not open file %s for writing.\n", path);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    fclose(inputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  fprintf(outputfile, "0       ");

  i = fprintf(outputfile, "%05i %c %s %s%s%s",
                          patientcode,
                          gender,
                          birthdate,
                          patientname,
                          handedness,
                          weight);
  for(; i<80; i++)
  {
    fputc(' ', outputfile);
  }

  i = fprintf(outputfile, "Startdate %02i-%s-%04i %05i%02i X %s",
                          atoi(date),
                          month,
                          year,
                          patientcode,
                          seq_nr,
                          equipment);
  for(; i<80; i++)
  {
    fputc(' ', outputfile);
  }

  if(fwrite(date, 8, 1, outputfile)!=1)
  {
    textEdit1->append("Write error.\n");
    fclose(inputfile);
    fclose(outputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  if(fwrite(time, 8, 1, outputfile)!=1)
  {
    textEdit1->append("Write error.\n");
    fclose(inputfile);
    fclose(outputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  fprintf(outputfile, "%-8i", (256 * (channels + 2)));

  fprintf(outputfile, "EDF+C                                       ");

  fprintf(outputfile, "%-8u", data_records);

  if(samplefrequency%10)
  {
    fprintf(outputfile, "1       ");
  }
  else
  {
    fprintf(outputfile, "0.1     ");
  }

  fprintf(outputfile, "%-4i", channels + 1);

  for(i=0; i<channels; i++)
  {
    len = get_string(scratchpad, inputfile, 1034 + (i * 512), 16);
    for(j=len; j<16; j++)
    {
      scratchpad[j] = ' ';
    }
    latin1_to_ascii(scratchpad, 16);
    scratchpad[16] = 0;
    fseeko(inputfile, (long long)(1025 + (i * 512)), SEEK_SET);
    if((fgetc(inputfile)=='E')&&(len<13))
    {
      fprintf(outputfile, "EEG ");
      if(fwrite(scratchpad, 12, 1, outputfile)!=1)
      {
        textEdit1->append("Write error.\n");
        fclose(inputfile);
        fclose(outputfile);
        free(logbuf);
        pushButton1->setEnabled(true);
        return;
      }
    }
    else
    {
      if(fwrite(scratchpad, 16, 1, outputfile)!=1)
      {
        textEdit1->append("Write error.\n");
        fclose(inputfile);
        fclose(outputfile);
        free(logbuf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if(fwrite("EDF Annotations ", 16, 1, outputfile)!=1)
  {
    textEdit1->append("Write error.\n");
    fclose(inputfile);
    fclose(outputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  for(i=0; i<(80*(channels+1)); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<channels; i++)
  {
    fprintf(outputfile, "uV      ");
  }
  fprintf(outputfile, "        ");

  for(i=0; i<channels; i++)
  {
    fprintf(outputfile, "-3276.8 ");
  }
  fprintf(outputfile, "-1      ");

  for(i=0; i<channels; i++)
  {
    fprintf(outputfile, "3276.7  ");
  }
  fprintf(outputfile, "1       ");

  for(i=0; i<(channels+1); i++)
  {
    fprintf(outputfile, "-32768  ");
  }

  for(i=0; i<(channels+1); i++)
  {
    fprintf(outputfile, "32767   ");
  }

  for(i=0; i<(80*(channels+1)); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<channels; i++)
  {
    fprintf(outputfile, "%-8i", samples_in_record);
  }
  fprintf(outputfile, "%-8i", tallen / 2);

  for(i=0; i<(32*(channels+1)); i++)
  {
    fputc(' ', outputfile);
  }

/****************** start conversion *****************/

  seconds = 0;

  deci_seconds = 0;

  record_size += tallen;

  bufsize = 4194304;

  if(record_size>bufsize)
  {
    bufsize = record_size;
  }

  buf = (char *)calloc(1, bufsize);
  if(buf==NULL)
  {
    textEdit1->append("Malloc error. (buf)\n");
    fclose(inputfile);
    fclose(outputfile);
    free(logbuf);
    pushButton1->setEnabled(true);
    return;
  }

  max_buf_records = bufsize / record_size;

  fseeko(inputfile, (long long)(hdrsize), SEEK_SET);

  while(data_records)
  {
    if(data_records>max_buf_records)  records_in_buf = max_buf_records;
    else  records_in_buf = data_records;

    for(i=0; i<records_in_buf; i++)
    {
      for(j=0; j<raster; j+=2)
      {
        for(k=0; k<channels; k++)
        {
          buf[j+(k*raster)+(i*record_size)] = fgetc(inputfile);
          buf[j+(k*raster)+(i*record_size)+1] = fgetc(inputfile);
        }
      }

      p = buf + (i * record_size) + (raster * channels);
      if(datrecduris1)
      {
        len = sprintf(p, "%+i", seconds);
        seconds++;
      }
      else
      {
        len = sprintf(p, "%+i.%i", seconds, deci_seconds);
        if(++deci_seconds>9)
        {
          deci_seconds = 0;
          seconds++;
        }
      }
      p[len++] = 20;
      p[len++] = 20;
      if(annots_written<annot_cnt)
      {
        p[len++] = 0;
        len += sprintf(p + len, "%+i", logbuf[annots_written].onset);
        if(logbuf[annots_written].duration)
        {
          p[len++] = 21;
          len += sprintf(p + len, "%i", logbuf[annots_written].duration);
        }
        p[len++] = 20;
        len += sprintf(p + len, "%s", logbuf[annots_written].description);
        p[len++] = 20;
        annots_written++;
      }
      for(; len<tallen; len++)
      {
        p[len] = 0;
      }
    }

    if(fwrite(buf, records_in_buf * record_size, 1, outputfile)!=1)
    {
      textEdit1->append("Write error.\n");
      fclose(inputfile);
      fclose(outputfile);
      free(buf);
      free(logbuf);
      pushButton1->setEnabled(true);
      return;
    }

    data_records -= records_in_buf;
  }

  free(logbuf);
  free(buf);
  fclose(inputfile);
  fclose(outputfile);

  snprintf(txt_string, 2048, "Done, your EDF+ file is %s\n", path);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  pushButton1->setEnabled(true);
}




int UI_EMSA2EDFwindow::get_string(char *str, FILE *file, int offset, int maxlen)
{
  int len;

  if(maxlen<1)
  {
    str[0] = 0;

    return 0;
  }

  fseeko(file, (long long)(offset), SEEK_SET);

  len = fgetc(file);

  if(len==EOF)
  {
    str[0] = 0;

    return 0;
  }

  if(len>maxlen)
  {
    len = maxlen;
  }

  if(fread(str, len, 1, file)!=1)
  {
    str[0] = 0;

    return 0;
  }

  str[len] = 0;

  return len;
}















