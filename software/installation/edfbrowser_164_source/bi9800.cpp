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



#include "bi9800.h"



UI_BI98002EDFwindow::UI_BI98002EDFwindow(char *recent_dir, char *save_dir)
{
  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(300, 75);
  myobjectDialog->setMaximumSize(300, 75);
  myobjectDialog->setWindowTitle("BI9800 to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 30, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(180, 30, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_BI98002EDFwindow::SelectFileButton()
{
  int i, j, n,
      tmp,
      end_of_file,
      samplefreq=0,
      chns = 3,
      hdl,
      samplesize=1,
      bufsize,
      *buf2,
      datablocks,
      blocks_written,
      checked_modelnumber=0,
      checked_samplerate=0,
      checked_recordhours=0,
      checked_recorddate=0,
      checked_recordtime=0,
      startdate_year=0,
      startdate_month=0,
      startdate_day=0,
      starttime_hour=0,
      starttime_minute=0,
      starttime_second=0,
      progress_steps;

  char path[MAX_PATH_LENGTH],
       outputfilename[MAX_PATH_LENGTH],
       str[2048],
       str2[128],
       *buf1,
       tmp2,
       modelnumber_str[32];

  FILE *dcmfile,
       *evtfile;



  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "DCM files (*.dcm *.DCM)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);

  dcmfile = fopeno(path, "rb");
  if(dcmfile==NULL)
  {
    snprintf(str, 2048, "Can not open file %s for reading.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str));
    messagewindow.exec();
    return;
  }

  remove_extension_from_filename(path);
  strcat(path, ".EVT");
  evtfile = fopeno(path, "rb");
  if(evtfile==NULL)
  {
    remove_extension_from_filename(path);
    strcat(path, ".evt");
    evtfile = fopeno(path, "rb");
    if(evtfile==NULL)
    {
      snprintf(str, 2048, "Can not open file %s for reading.", path);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      fclose(dcmfile);
      return;
    }
  }

/***************** check if the file is valid ******************************/

  for(end_of_file=0; end_of_file == 0; )
  {
    for(i=0; i<256; i++)
    {
      tmp = fgetc(evtfile);

      if(tmp == '\n')
      {
        break;
      }

      if(tmp == EOF)
      {
        end_of_file = 1;

        break;
      }

      str[i] = tmp;
    }

    str[i] = 0;

    if(!(strncmp(str, "Sampling Rate=", 14)))
    {
      samplefreq = atoi(str + 14);
      switch(samplefreq)
      {
        case  128 : break;
        case  256 : break;
        case  512 : break;
        case 1024 : break;

        default : QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown samplerate.");
                  messagewindow.exec();
                  fclose(dcmfile);
                  fclose(evtfile);
                  return;
      }

      checked_samplerate = 1;
    }

    if(!(strncmp(str, "Model number=", 13)))
    {
      strncpy(modelnumber_str, str + 13, 8);
      modelnumber_str[8] = 0;

      if(strcmp(modelnumber_str, "TM SD01G") &&
         strcmp(modelnumber_str, "SD SD02G"))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Wrong modelnumber.");
        messagewindow.exec();
        fclose(dcmfile);
        fclose(evtfile);
        return;
      }

      checked_modelnumber = 1;
    }

    if(!(strncmp(str, "Record Date=", 12)))
    {
      strncpy(str2, str + 12, 10);
      str2[10] = 0;

      startdate_year = atoi(str2);
      startdate_month = atoi(str2 + 5);
      startdate_day = atoi(str2 + 8);

      if((startdate_year < 1970) || (startdate_year > 3000) ||
         (startdate_month < 1)   || (startdate_month > 12)  ||
         (startdate_day < 1)     || (startdate_day > 31))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Wrong record date.");
        messagewindow.exec();
        fclose(dcmfile);
        fclose(evtfile);
        return;
      }

      checked_recorddate = 1;
    }

    if(!(strncmp(str, "Record Time=", 12)))
    {
      strncpy(str2, str + 12, 10);
      str2[8] = 0;

      starttime_hour = atoi(str2);
      starttime_minute = atoi(str2 + 3);
      starttime_second = atoi(str2 + 6);

      if((starttime_hour < 0)  || (starttime_hour > 23)   ||
        (starttime_minute < 0) || (starttime_minute > 59) ||
        (starttime_second < 0) || (starttime_second > 59))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Wrong recordtime.");
        messagewindow.exec();
        fclose(dcmfile);
        fclose(evtfile);
        return;
      }

      checked_recordtime = 1;
    }

    if(!(strncmp(str, "Record Hours=", 13)))
    {
      strncpy(str2, str + 13, 10);
      str2[2] = 0;

      if((atoi(str2) != 24) && (atoi(str2) != 48))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Wrong record hours.");
        messagewindow.exec();
        fclose(dcmfile);
        fclose(evtfile);
        return;
      }

      checked_recordhours = 1;
    }
  }

  if(!checked_modelnumber ||
     !checked_samplerate  ||
     !checked_recordhours ||
     !checked_recorddate  ||
     !checked_recordtime)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Missing line.");
    messagewindow.exec();
    fclose(dcmfile);
    fclose(evtfile);
    return;
  }

///////////////////////////////////////////////////////////////////////////////////

  bufsize = chns * samplesize * samplefreq;

  buf1 = (char *)malloc(bufsize);
  if(buf1 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error.");
    messagewindow.exec();
    fclose(dcmfile);
    fclose(evtfile);
    return;
  }

  buf2 = (int *)malloc(bufsize * sizeof(int));
  if(buf2 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error.");
    messagewindow.exec();
    fclose(dcmfile);
    fclose(evtfile);
    free(buf1);
    return;
  }

///////////////////////////////////////////////////////////////////////////////////

  path[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(path, recent_savedir);
    strcat(path, "/");
  }
  strcat(path, outputfilename);
  remove_extension_from_filename(path);
  strcat(path, ".edf");

  strcpy(path, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    fclose(dcmfile);
    fclose(evtfile);
    free(buf1);
    free(buf2);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, chns);
  if(hdl<0)
  {
    snprintf(str, 2048, "Can not open file %s for writing.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    fclose(dcmfile);
    fclose(evtfile);
    free(buf1);
    free(buf2);
    return;
  }

  for(i=0; i<chns; i++)
  {
    edf_set_samplefrequency(hdl, i, samplefreq);
  }

  for(i=0; i<chns; i++)
  {
    edf_set_digital_maximum(hdl, i, 127);
  }

  for(i=0; i<chns; i++)
  {
    edf_set_digital_minimum(hdl, i, -128);
  }

  for(i=0; i<chns; i++)
  {
    edf_set_physical_maximum(hdl, i, 2442.307692);
  }

  for(i=0; i<chns; i++)
  {
    edf_set_physical_minimum(hdl, i, -2461.538462);
  }

  for(i=0; i<chns; i++)
  {
    edf_set_physical_dimension(hdl, i, "uV");
  }

  edf_set_label(hdl, 0, "channel 1");

  edf_set_label(hdl, 1, "channel 2");

  edf_set_label(hdl, 2, "channel 3");

  edf_set_equipment(hdl, modelnumber_str);

  edf_set_patientname(hdl, "BI9800");

  edf_set_startdatetime(hdl, startdate_year, startdate_month, startdate_day, starttime_hour, starttime_minute, starttime_second);

  fseeko(dcmfile, 0LL, SEEK_END);
  datablocks = ftello(dcmfile) / bufsize;
  fseeko(dcmfile, 0LL, SEEK_SET);

  QProgressDialog progress("Converting...", "Cancel", 0, datablocks, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datablocks / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  for(blocks_written=0; ; blocks_written++)
  {
    if(!(blocks_written % progress_steps))
    {
      progress.setValue(blocks_written);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    n = fread(buf1, bufsize, 1, dcmfile);
    if(n != 1)
    {
      break;
    }

    for(i=0; i<samplefreq; i++)
    {
      for(j=0; j<chns; j++)
      {
        tmp2 = buf1[(i * chns) + j] + 128;
        buf2[(j * samplefreq) + i] = tmp2;
      }
    }

    edf_blockwrite_digital_samples(hdl, buf2);
  }

  progress.reset();

  edfwrite_annotation_latin1(hdl, 0LL, -1LL, "Recording starts");

  fseeko(evtfile, 0LL, SEEK_SET);

  for(end_of_file=0; end_of_file == 0; )
  {
    for(i=0; i<256; i++)
    {
      tmp = fgetc(evtfile);

      if((tmp == '\n') || (tmp == '\r'))
      {
        break;
      }

      if(tmp == EOF)
      {
        end_of_file = 1;

        break;
      }

      str[i] = tmp;
    }

    str[i] = 0;

    if((isdigit(str[0])) && (isdigit(str[1])))
    {
      starttime_hour = atoi(str);
      starttime_minute = atoi(str + 3);
      starttime_second = atoi(str + 6);

      if((starttime_hour < 0)   || (starttime_hour > 23)   ||
         (starttime_minute < 0) || (starttime_minute > 59) ||
         (starttime_second < 0) || (starttime_second > 59))
      {
      }
      else
      {
        if(strlen(str) > 9)
        {
          edfwrite_annotation_latin1(hdl, (starttime_second + (starttime_minute * 60) + (starttime_hour * 3600)) * 10000, -1, str + 9);
        }
      }
    }
  }

  edfwrite_annotation_latin1(hdl, blocks_written * 10000LL, -1LL, "Recording ends");

  edfclose_file(hdl);

  fclose(dcmfile);
  fclose(evtfile);
  free(buf1);
  free(buf2);
}



















