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



#include "biosemi2bdfplus.h"






UI_BIOSEMI2BDFPLUSwindow::UI_BIOSEMI2BDFPLUSwindow(QWidget *w_parent)
{
  int i;

  char str[128];


  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = mainwindow->recent_opendir;
  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 630);
  myobjectDialog->setMaximumSize(600, 630);
  myobjectDialog->setWindowTitle("Biosemi to BDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  label2 = new QLabel(myobjectDialog);
  label2->setGeometry(20, 20, 200, 25);
  label2->setText("Trigger Input descriptions:");

  label3 = new QLabel(myobjectDialog);
  label3->setGeometry(290, 350, 200, 25);
  label3->setText("Annotations will be generated at");

  label4 = new QLabel(myobjectDialog);
  label4->setGeometry(290, 440, 200, 25);
  label4->setText("of trigger input.");

  for(i=0; i<16; i++)
  {
    label1[i] = new QLabel(myobjectDialog);
    label1[i]->setGeometry(20, 50 + (i * 30), 20, 25);
    sprintf(str, "%i", i + 1);
    label1[i]->setText(str);

    lineEdit1[i] = new QLineEdit(myobjectDialog);
    lineEdit1[i]->setGeometry(70, 50 + (i * 30), 120, 25);
    sprintf(str, "Trigger Input %i", i + 1);
    lineEdit1[i]->setText(str);
    lineEdit1[i]->setMaxLength(16);
  }

  radioButton1 = new QRadioButton(myobjectDialog);
  radioButton1->setGeometry(290, 385, 150, 20);
  radioButton1->setText("rising edge");
  radioButton1->setChecked(true);

  radioButton2 = new QRadioButton(myobjectDialog);
  radioButton2->setGeometry(290, 415, 150, 20);
  radioButton2->setText("falling edge");

  checkBox1 = new QCheckBox(myobjectDialog);
  checkBox1->setGeometry(290, 500, 200, 20);
  checkBox1->setText("measure event duration");
  checkBox1->setTristate(false);
  checkBox1->setChecked(false);

  selectButton = new QPushButton(myobjectDialog);
  selectButton->setGeometry(20, 584, 100, 25);
  selectButton->setText("Select File");

  closeButton = new QPushButton(myobjectDialog);
  closeButton->setGeometry(480, 584, 100, 25);
  closeButton->setText("Close");

  QObject::connect(selectButton,   SIGNAL(clicked()),   this,           SLOT(SelectFileButton()));
  QObject::connect(closeButton,    SIGNAL(clicked()),   myobjectDialog, SLOT(close()));

  inputpath[0] = 0;
  outputpath[0] = 0;

  myobjectDialog->exec();
}


void UI_BIOSEMI2BDFPLUSwindow::SelectFileButton()
{
  int i, j, k,
      error,
      hdl_in,
      hdl_out,
      edfsignals,
      status_signal=0,
      status_samples_in_datarecord=0,
      rising_edge,
      set_duration,
      status[24],
      totalSamplesInDatarecord,
      *buf,
      buf_offset[EDFLIB_MAXSIGNALS],
      sf,
      new_sf,
      samplerate_divider;

  char str[2048],
       triggerlabel[24][64],
       outputfilename[MAX_PATH_LENGTH];

  long long datarecords,
            status_sample_duration,
            trigger_cnt,
            progress_steps;

  struct edf_hdr_struct hdr;

  struct annotation_list annot_list;

  memset(&annot_list, 0, sizeof(struct annotation_list));

  struct annotationblock annotation;

  memset(&annotation, 0, sizeof(struct annotationblock));

  struct annotationblock *annot_ptr=NULL;


  for(i=0; i<16; i++)
  {
    if(!lineEdit1[i]->text().length())
    {
      sprintf(str, "Trigger Input label %i is empty!", i + 1);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      return;
    }
  }

  for(i=0; i<16; i++)
  {
    for(j=0; j<16; j++)
    {
      if(i != j)
      {
        if(!strcmp(lineEdit1[i]->text().toLocal8Bit().data(), lineEdit1[j]->text().toLocal8Bit().data()))
        {
          sprintf(str, "Trigger Input labels %i and %i are the same!", i + 1, j + 1);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
          messagewindow.exec();
          return;
        }
      }
    }
  }

  str[0] = 0;

  strcpy(inputpath, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());

  if(!strcmp(inputpath, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, inputpath, MAX_PATH_LENGTH);

  error = edfopen_file_readonly(inputpath, &hdr, EDFLIB_DO_NOT_READ_ANNOTATIONS);

  if(error < 0)
  {
    error = hdr.filetype;

    switch(error)
    {
      case EDFLIB_MALLOC_ERROR                : strcpy(str, "EDFlib: malloc error.");
                                                break;
      case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : strcpy(str, "EDFlib: no such file or directory.");
                                                break;
      case EDFLIB_FILE_CONTAINS_FORMAT_ERRORS : strcpy(str, "EDFlib: file contains format errors.\nOpen the file in EDFbrowser to get more info.");
                                                break;
      case EDFLIB_MAXFILES_REACHED            : strcpy(str, "EDFlib: maximum amount of files reached.");
                                                break;
      case EDFLIB_FILE_READ_ERROR             : strcpy(str, "EDFlib: a file read error occurred.");
                                                break;
      case EDFLIB_FILE_ALREADY_OPENED         : strcpy(str, "EDFlib: file is already opened.");
                                                break;
      case EDFLIB_FILETYPE_ERROR              : strcpy(str, "EDFlib: filetype error.");
                                                break;
      case EDFLIB_FILE_WRITE_ERROR            : strcpy(str, "EDFlib: file write error.");
                                                break;
      case EDFLIB_NUMBER_OF_SIGNALS_INVALID   : strcpy(str, "EDFlib: invalid number of signals.");
                                                break;
      case EDFLIB_FILE_IS_DISCONTINUOUS       : strcpy(str, "EDFlib: file is discontinuous.");
                                                break;
      case EDFLIB_INVALID_READ_ANNOTS_VALUE   : strcpy(str, "EDFlib: invalid read annotations argument.");
                                                break;
      default                                 : strcpy(str, "EDFlib: unknown error.");
    }

    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();

    return;
  }

  hdl_in = hdr.handle;

/////////////////// check file /////////////////////////////////////////////

  if(hdr.filetype == EDFLIB_FILETYPE_BDFPLUS)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Selected file is already a BDF-plus file.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  if(hdr.filetype != EDFLIB_FILETYPE_BDF)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Selected file is not a BDF file.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  if(hdr.datarecord_duration != EDFLIB_TIME_DIMENSION)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Datarecord duration of inputfile must be 1 second.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  edfsignals = hdr.edfsignals;

  if(edfsignals < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There are no signals in the selected file.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  sf = hdr.signalparam[0].smp_in_datarecord;

  for(i=1; i<edfsignals; i++)
  {
    if(hdr.signalparam[i].smp_in_datarecord != sf)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "All signals must have the same samplefrequency.");
      messagewindow.exec();
      edfclose_file(hdl_in);
      return;
    }
  }

  error = 1;

  switch(sf)
  {
    case 16384 : error = 0;
                 break;
    case  8192 : error = 0;
                 break;
    case  4096 : error = 0;
                 break;
    case  2048 : error = 0;
                 break;
    case  1024 : error = 0;
                 break;
    case   512 : error = 0;
                 break;
    case   256 : error = 0;
                 break;
    case   128 : error = 0;
                 break;
    case    64 : error = 0;
                 break;
    case    32 : error = 0;
                 break;
  }

  if(error)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency must be 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64 or 32 Hz.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  for(i=0; i<edfsignals; i++)
  {
    if(!(strcmp(hdr.signalparam[i].label, "Status          ")))
    {
      status_signal = i;

      break;
    }
  }

  if(i == edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There is no Status signal in the selected file.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  totalSamplesInDatarecord = 0;

  for(i=0; i<edfsignals; i++)
  {
    buf_offset[i] = totalSamplesInDatarecord;

    if(i == status_signal)
    {
      status_samples_in_datarecord = hdr.signalparam[i].smp_in_datarecord;
    }

    totalSamplesInDatarecord += hdr.signalparam[i].smp_in_datarecord;
  }

  status_sample_duration = EDFLIB_TIME_DIMENSION / (long long)status_samples_in_datarecord;

  for(i=0; i<16; i++)
  {
    strcpy(&triggerlabel[i][0], lineEdit1[i]->text().toUtf8().data());
    triggerlabel[i][16] = 0;
  }

  strcpy(&triggerlabel[16][0], "new epoch");

  if(radioButton1->isChecked() == true)
  {
    rising_edge = 1;

    for(i=0; i<16; i++)
    {
      status[i] = 1;
    }
  }
  else
  {
    rising_edge = 0;

    for(i=0; i<16; i++)
    {
      status[i] = 0;
    }
  }

  if(checkBox1->isChecked() == true)
  {
    set_duration = 1;
  }
  else
  {
    set_duration = 0;
  }

  for(i=16; i<24; i++)
  {
    status[i] = 1;
  }

  strcpy(outputfilename, inputpath);
  remove_extension_from_filename(outputfilename);
  strcat(outputfilename, "_+.bdf");

  outputpath[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(outputpath, recent_savedir);
    strcat(outputpath, "/");
  }
  strcat(outputpath, outputfilename);

  strcpy(outputpath, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(outputpath), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());

  if(!strcmp(outputpath, ""))
  {
    edfclose_file(hdl_in);
    return;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Outputfile is already opened in EDFbrowser.\nClose the file and try again.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  if(!(strcmp(inputpath, outputpath)))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Inputfile and outputfile are the same.");
    messagewindow.exec();
    edfclose_file(hdl_in);
    return;
  }

  hdl_out = edfopen_file_writeonly(outputpath, EDFLIB_FILETYPE_BDFPLUS, edfsignals);

  if(hdl_out < 0)
  {
    switch(hdl_out)
    {
      case EDFLIB_MALLOC_ERROR                : strcpy(str, "EDFlib: malloc error.");
                                                break;
      case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY   : strcpy(str, "EDFlib: no such file or directory.");
                                                break;
      case EDFLIB_MAXFILES_REACHED            : strcpy(str, "EDFlib: maximum amount of files reached.");
                                                break;
      case EDFLIB_FILE_READ_ERROR             : strcpy(str, "EDFlib: a file read error occurred.");
                                                break;
      case EDFLIB_FILE_ALREADY_OPENED         : strcpy(str, "EDFlib: file is already opened.");
                                                break;
      case EDFLIB_FILETYPE_ERROR              : strcpy(str, "EDFlib: filetype error.");
                                                break;
      case EDFLIB_FILE_WRITE_ERROR            : strcpy(str, "EDFlib: file write error.");
                                                break;
      case EDFLIB_NUMBER_OF_SIGNALS_INVALID   : strcpy(str, "EDFlib: invalid number of signals.");
                                                break;
      default                                 : strcpy(str, "EDFlib: unknown error.");
    }

    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();

    edfclose_file(hdl_in);
    return;
  }

/////////////////// copy header /////////////////////////////////////////////

  for(i=0; i<edfsignals; i++)
  {
    edf_set_samplefrequency(hdl_out, i, hdr.signalparam[i].smp_in_datarecord);
    edf_set_physical_maximum(hdl_out, i, hdr.signalparam[i].phys_max);
    edf_set_physical_minimum(hdl_out, i, hdr.signalparam[i].phys_min);
    edf_set_digital_maximum(hdl_out, i, hdr.signalparam[i].dig_max);
    edf_set_digital_minimum(hdl_out, i, hdr.signalparam[i].dig_min);
    edf_set_label(hdl_out, i, hdr.signalparam[i].label);
    edf_set_prefilter(hdl_out, i, hdr.signalparam[i].prefilter);
    edf_set_transducer(hdl_out, i, hdr.signalparam[i].transducer);
    edf_set_physical_dimension(hdl_out, i, hdr.signalparam[i].physdimension);
  }

  edf_set_startdatetime(hdl_out, hdr.startdate_year, hdr.startdate_month, hdr.startdate_day, hdr.starttime_hour, hdr.starttime_minute, hdr.starttime_second);
  edf_set_patientname(hdl_out, hdr.patient);
  edf_set_recording_additional(hdl_out, hdr.recording);

/////////////////// collect triggers /////////////////////////////////////////////

  buf = (int *)malloc(sizeof(int) * status_samples_in_datarecord);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (buf).");
    messagewindow.exec();
    edfclose_file(hdl_in);
    edfclose_file(hdl_out);
    return;
  }

  QProgressDialog progress("Collecting triggers...", "Abort", 0, (int)hdr.datarecords_in_file);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = hdr.datarecords_in_file / 100LL;
  if(progress_steps < 1LL)
  {
    progress_steps = 1LL;
  }

  trigger_cnt = 0;

  for(datarecords = 0LL; datarecords < hdr.datarecords_in_file; datarecords++)
  {
    if(trigger_cnt >= ((hdr.datarecords_in_file * 32) - 2))
    {
      break;
    }

    if(trigger_cnt >= 100000)
    {
      break;
    }

    if(!(datarecords % progress_steps))
    {
      progress.setValue((int)datarecords);

      qApp->processEvents();

      if(progress.wasCanceled())
      {
        edfclose_file(hdl_in);
        edfclose_file(hdl_out);
        free(buf);
        edfplus_annotation_empty_list(&annot_list);
        return;
      }
    }

    if(edfread_digital_samples(hdl_in, status_signal, status_samples_in_datarecord, buf) < 0)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred during the collection of triggers.");
      messagewindow.exec();
      edfclose_file(hdl_in);
      edfclose_file(hdl_out);
      free(buf);
      edfplus_annotation_empty_list(&annot_list);
      return;
    }

    for(i=0; i<status_samples_in_datarecord; i++)
    {
      for(j=0; j<17; j++)
      {
        if(((buf[i] & (1 << j)) && !status[j]) || (!(buf[i] & (1 << j)) && status[j]))  // rising or falling edge detected
        {
          if(status[j])  // falling edge detected
          {
            if((!rising_edge) && (j < 16))
            {
              annotation.onset = (datarecords * EDFLIB_TIME_DIMENSION) + ((long long)i * status_sample_duration);
              annotation.onset += hdr.starttime_subsecond;
              strcpy(annotation.annotation, triggerlabel[j]);
              if(edfplus_annotation_add_item(&annot_list, annotation))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (annotation).");
                messagewindow.exec();
                edfclose_file(hdl_in);
                edfclose_file(hdl_out);
                free(buf);
                edfplus_annotation_empty_list(&annot_list);
                return;
              }

              trigger_cnt++;
            }
            else
            {
              if(set_duration)
              {
                k = edfplus_annotation_size(&annot_list);
                for(; k>0; k--)
                {
                  annot_ptr = edfplus_annotation_get_item(&annot_list, k - 1);
                  if(annot_ptr == NULL)
                  {
                    break;
                  }
                  if(!strcmp(annot_ptr->annotation, triggerlabel[j]))
                  {
                    sprintf(str, "%.4f", (double)((datarecords * EDFLIB_TIME_DIMENSION) + ((long long)i * status_sample_duration) - annot_ptr->onset) / (double)EDFLIB_TIME_DIMENSION);
                    str[15] = 0;
                    strcpy(annot_ptr->duration, str);
                    annot_ptr->long_duration = edfplus_annotation_get_long_from_number(str);
                    break;
                  }
                }
              }
            }

            status[j] = 0;
          }
          else  // rising edge detected
          {
            if(rising_edge || (j == 16))
            {
              annotation.onset = (datarecords * EDFLIB_TIME_DIMENSION) + ((long long)i * status_sample_duration);
              annotation.onset += hdr.starttime_subsecond;
              strcpy(annotation.annotation, triggerlabel[j]);
              if(edfplus_annotation_add_item(&annot_list, annotation))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (annotation).");
                messagewindow.exec();
                edfclose_file(hdl_in);
                edfclose_file(hdl_out);
                free(buf);
                edfplus_annotation_empty_list(&annot_list);
                return;
              }

              trigger_cnt++;
            }
            else
            {
              if(set_duration)
              {
                k = edfplus_annotation_size(&annot_list);
                for(; k>0; k--)
                {
                  annot_ptr = edfplus_annotation_get_item(&annot_list, k - 1);
                  if(annot_ptr == NULL)
                  {
                    break;
                  }
                  if(!strcmp(annot_ptr->annotation, triggerlabel[j]))
                  {
                    sprintf(str, "%.4f", (double)((datarecords * EDFLIB_TIME_DIMENSION) + ((long long)i * status_sample_duration) - annot_ptr->onset) / (double)EDFLIB_TIME_DIMENSION);
                    str[15] = 0;
                    strcpy(annot_ptr->duration, str);
                    break;
                  }
                }
              }
            }

            status[j] = 1;
          }
        }
      }
    }
  }

  edfwrite_annotation_latin1(hdl_out, 0LL, -1LL, "Recording starts");

  j = edfplus_annotation_size(&annot_list);

  for(i=0; i<j; i++)
  {
    annot_ptr = edfplus_annotation_get_item(&annot_list, i);

    if(annot_ptr->duration[0] == 0)
    {
      edfwrite_annotation_utf8(hdl_out, annot_ptr->onset / 1000LL, -1LL, annot_ptr->annotation);
    }
    else
    {
      edfwrite_annotation_utf8(hdl_out, annot_ptr->onset / 1000LL, (long long)(atof(annot_ptr->duration) * 10000.0), annot_ptr->annotation);
    }
  }

  free(buf);

  edfwrite_annotation_latin1(hdl_out, hdr.datarecords_in_file * 10000LL, -1LL, "Recording ends");

/////////////////// choose datarecord duration /////////////////////////////////////////////

  samplerate_divider = 1;

  i = edfplus_annotation_size(&annot_list);

  edfplus_annotation_empty_list(&annot_list);

  if(i % 2)
  {
    i++;
  }

  i += 2;

  while(i > hdr.datarecords_in_file)
  {
    samplerate_divider *= 2;

    i /= 2;

    if(samplerate_divider == 32)
    {
      break;
    }
  }

  if(samplerate_divider > 1)
  {
    for(i=0; i<edfsignals; i++)
    {
      edf_set_samplefrequency(hdl_out, i, hdr.signalparam[i].smp_in_datarecord / samplerate_divider);
    }

    if(edf_set_datarecord_duration(hdl_out, 100000 / samplerate_divider) == -1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "edf_set_datarecord_duration() returned an error.");
      messagewindow.exec();
      edfclose_file(hdl_in);
      edfclose_file(hdl_out);
      return;
    }
  }

  new_sf = sf / samplerate_divider;

/////////////////// start conversion /////////////////////////////////////////////

  buf = (int *)malloc(sizeof(int) * totalSamplesInDatarecord);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (buf).");
    messagewindow.exec();
    edfclose_file(hdl_in);
    edfclose_file(hdl_out);
    return;
  }

  edfrewind(hdl_in, status_signal);

  progress.setLabelText("Converting...");
  progress.setValue(0);

  for(datarecords = 0LL; datarecords < hdr.datarecords_in_file; datarecords++)
  {
    if(!(datarecords % progress_steps))
    {
      progress.setValue((int)datarecords);

      qApp->processEvents();

      if(progress.wasCanceled())
      {
        edfclose_file(hdl_in);
        edfclose_file(hdl_out);
        free(buf);
        return;
      }
    }

    for(i=0; i<edfsignals; i++)
    {
      if(edfread_digital_samples(hdl_in, i, hdr.signalparam[i].smp_in_datarecord, buf + buf_offset[i]) < 0)
      {
        progress.reset();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred during the conversion.");
        messagewindow.exec();
        edfclose_file(hdl_in);
        edfclose_file(hdl_out);
        free(buf);
        return;
      }
    }

    for(j=0; j<samplerate_divider; j++)
    {
      for(i=0; i<edfsignals; i++)
      {
        if(edfwrite_digital_samples(hdl_out, buf + buf_offset[i] + (j * new_sf)) < 0)
        {
          progress.reset();
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "A write error occurred during the conversion.");
          messagewindow.exec();
          edfclose_file(hdl_in);
          edfclose_file(hdl_out);
          free(buf);
          return;
        }
      }
    }
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  edfclose_file(hdl_in);
  edfclose_file(hdl_out);
  free(buf);

  QApplication::restoreOverrideCursor();

  progress.reset();

#ifdef Q_OS_WIN32
  __mingw_snprintf(str, 2048, "Done. Converted %lli input trigger events to BDF+ annotations.\n"
                      "\nBDF+ file is located at %s", trigger_cnt, outputpath);
#else
  snprintf(str, 2048, "Done. Converted %lli input trigger events to BDF+ annotations.\n"
                      "\nBDF+ file is located at %s", trigger_cnt, outputpath);
#endif

  QMessageBox messagewindow(QMessageBox::Information, "Ready", str);
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();
}


























