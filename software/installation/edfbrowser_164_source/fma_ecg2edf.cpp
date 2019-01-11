/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2015, 2016, 2017, 2018 Teunis van Beelen
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

/*
***************************************************************************
*
* This is an FM decoder using up-sampling and zero-crossing detection.
* It expects the source signal to be sampled at the traditional samplerates
* used for pc sound cards: 8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200 or 96000 Hz
* with 16-bit resolution.
*
* The carrier centerfrequency of the FM signal is 1900Hz.
* Frequency deviation is 100Hz per milli-Volt.
* This technic is used to upload an ECG recording via POTS (Plain Old Telephone Service).
*
***************************************************************************
*/



#include "fma_ecg2edf.h"



UI_FMaudio2EDFwindow::UI_FMaudio2EDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 300);
  myobjectDialog->setMaximumSize(600, 300);
  myobjectDialog->setWindowTitle("FM Audio ECG to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  PatientnameLabel = new QLabel(myobjectDialog);
  PatientnameLabel->setGeometry(20, 20, 140, 25);
  PatientnameLabel->setText("Subject");

  RecordingLabel = new QLabel(myobjectDialog);
  RecordingLabel->setGeometry(20, 55, 140, 25);
  RecordingLabel->setText("Recording");

  DatetimeLabel = new QLabel(myobjectDialog);
  DatetimeLabel->setGeometry(20, 90, 140, 25);
  DatetimeLabel->setText("Startdate and time");

  PatientnameLineEdit = new QLineEdit(myobjectDialog);
  PatientnameLineEdit->setGeometry(160, 20, 420, 25);
  PatientnameLineEdit->setMaxLength(80);

  RecordingLineEdit = new QLineEdit(myobjectDialog);
  RecordingLineEdit->setGeometry(160, 55, 420, 25);
  RecordingLineEdit->setMaxLength(80);

  StartDatetimeedit = new QDateTimeEdit(myobjectDialog);
  StartDatetimeedit->setGeometry(160, 90, 140, 25);
  StartDatetimeedit->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
  StartDatetimeedit->setDateTime(QDateTime::currentDateTime());

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 250, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 250, 100, 25);
  pushButton2->setText("Close");

  helpButton = new QPushButton(myobjectDialog);
  helpButton->setGeometry(240, 250, 100, 25);
  helpButton->setText("Help");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));
  QObject::connect(helpButton,  SIGNAL(clicked()), this,           SLOT(helpbuttonpressed()));

  myobjectDialog->exec();
}



void UI_FMaudio2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL;

  int i,
      edfsignals,
      resolution,
      sf,
      df,
      uf,
      dsf,
      usf,
      hdl_out,
      readbufsize,
      *buf2=NULL, *buf3=NULL, pos=0, smpls=0,
      freq=608000, freq_min=448000, freq_max=768000;

  unsigned int fmt_chunk_offset,
               data_chunk_offset,
               tmp;

  char path[MAX_PATH_LENGTH],
       outputfilename[MAX_PATH_LENGTH],
       scratchpad[512],
       *buf1=NULL;

  long long blocks,
            leftover,
            progress_steps,
            k;


  hpf44=NULL;
  lpf9a=NULL;
  lpf9b=NULL;
  lpf9c=NULL;
  lpf9d=NULL;
  lpf9e=NULL;
  lpf200a=NULL;
  lpf200b=NULL;
  lpf200c=NULL;
  lpf200d=NULL;
  lpf200e=NULL;

  enable_widgets(false);

  if(!(strlen(PatientnameLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a subject name.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  if(!(strlen(RecordingLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a recording description.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "Text files (*.wav *.WAV)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    enable_widgets(true);
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

/***************** check if the wavefile is valid ******************************/

  rewind(inputfile);

  if(fread(scratchpad, 256, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading from inputfile.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if((strncmp(scratchpad, "RIFF", 4)) || (strncmp(scratchpad + 8, "WAVE", 4)))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not a Wave file.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  fmt_chunk_offset = 12;

  while(1)
  {
    fseeko(inputfile, (long long)fmt_chunk_offset, SEEK_SET);

    if(fread(scratchpad, 256, 1, inputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find fmt chunk.");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(strncmp(scratchpad, "fmt ", 4) == 0)
    {
      break;
    }

    tmp = *((unsigned int *)(scratchpad + 4));
    if(tmp < 2)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find fmt chunk.");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(tmp & 1)
    {
      tmp++;
    }

    fmt_chunk_offset += (tmp + 8);
  }

  if(*((signed short *)(scratchpad + 8)) != 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File contains compressed data.\nCan not convert compressed data.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  edfsignals = *((unsigned short *)(scratchpad + 10));

  if(edfsignals < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Number of channels in wave file is < 1");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(edfsignals > 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Number of channels in wave file is > 1\n"
                                                              "Can only handle mono recordings.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  sf = *((unsigned int *)(scratchpad + 12));

  switch(sf)
  {
    case 96000 : dsf = 384;
                 usf = 2;
                 break;
    case 88200 : dsf = 882;
                 usf = 5;
                 break;
    case 48000 : dsf = 384;
                 usf = 4;
                 break;
    case 44100 : dsf = 441;
                 usf = 5;
                 break;
    case 32000 : dsf = 384;
                 usf = 6;
                 break;
    case 22050 : dsf = 441;
                 usf = 10;
                 break;
    case 16000 : dsf = 384;
                 usf = 12;
                 break;
    case 11025 : dsf = 441;
                 usf = 20;
                 break;
    case  8000 : dsf = 384;
                 usf = 24;
                 break;
    default    : dsf =  0;
                 usf = 1;
                 break;
  }

  if(!dsf)
  {
    sprintf(scratchpad, "error: samplefrequency is %iHz.\n"
           "expected: 8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200 or 96000 Hz\n", sf);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", scratchpad);
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

//  usf *= 2;  // Select 1000Hz instead of 500Hz for the new EDF file,
               // not used because it doesn't improve signal quality

                  // sf:   samplefrequency of the source file
  uf = sf * usf;  // uf:   upsampling frequency (192KHz or 220.5KHz)
                  // usf:  upsampling factor
  df = uf / dsf;  // df:   downsampling frequency (500Hz, samplefrequency of the new EDF file)
                  // dsf:  downsampling factor

  resolution = *((unsigned short *)(scratchpad + 22));

  if(resolution != 16)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Resolution (bitdepth) must be 16 bit.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

///////////////////////////////////////////////////////////////////////////////

  data_chunk_offset = 12;

  while(1)
  {
    fseeko(inputfile, (long long)data_chunk_offset, SEEK_SET);

    if(fread(scratchpad, 256, 1, inputfile)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find data chunk.");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(strncmp(scratchpad, "data", 4) == 0)
    {
      break;
    }

    tmp = *((unsigned int *)(scratchpad + 4));
    if(tmp < 2)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find data chunk.");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(tmp & 1)
    {
      tmp++;
    }

    data_chunk_offset += (tmp + 8);
  }

///////////////////////////////////////////////////////////////////////////////

  blocks = (long long)(*((int *)(scratchpad + 4)));

  blocks /= (sf * edfsignals * 2);

  fseeko(inputfile, 0LL, SEEK_END);

  leftover = ftello(inputfile) - (long long)data_chunk_offset - 8LL;

  leftover /= (sf * edfsignals * 2);

  if(blocks > leftover)
  {
    blocks = leftover;
  }

  if(blocks < 1LL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Not enough data in file.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  readbufsize = 2 * sf * edfsignals;

  buf1 = (char *)malloc(readbufsize);
  if(buf1 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (buf1).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  buf2 = (int *)malloc(uf * sizeof(int));
  if(buf2 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (buf2).");
    messagewindow.exec();
    fclose(inputfile);
    free(buf1);
    enable_widgets(true);
    return;
  }

  buf3 = (int *)malloc(df * sizeof(int));
  if(buf3 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (buf3).");
    messagewindow.exec();
    fclose(inputfile);
    free(buf1);
    free(buf2);
    enable_widgets(true);
    return;
  }

  if(allocateFilters(uf, usf, dsf) == false)
  {
    fclose(inputfile);
    free(buf1);
    free(buf2);
    free(buf3);
    deleteFilters();
    enable_widgets(true);
    return;
  }

/***************** create a new EDF file *****************************************/

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
    free(buf1);
    free(buf2);
    free(buf3);
    deleteFilters();
    enable_widgets(true);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  hdl_out = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, edfsignals);

  if(hdl_out < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open output file for writing.");
    messagewindow.exec();
    fclose(inputfile);
    free(buf1);
    free(buf2);
    free(buf3);
    deleteFilters();
    enable_widgets(true);
    return;
  }

  edf_set_samplefrequency(hdl_out, 0, df);
  edf_set_physical_maximum(hdl_out, 0, 8192);
  edf_set_physical_minimum(hdl_out, 0, -8192);
  edf_set_digital_maximum(hdl_out, 0, 32767);
  edf_set_digital_minimum(hdl_out, 0, -32768);
  edf_set_label(hdl_out, 0, "ECG");
  edf_set_physical_dimension(hdl_out, 0, "uV");

  edf_set_patientname(hdl_out, PatientnameLineEdit->text().toLatin1().data());

  edf_set_recording_additional(hdl_out, RecordingLineEdit->text().toLatin1().data());

  edf_set_startdatetime(hdl_out, StartDatetimeedit->date().year(), StartDatetimeedit->date().month(), StartDatetimeedit->date().day(),
                        StartDatetimeedit->time().hour(), StartDatetimeedit->time().minute(), StartDatetimeedit->time().second());

  edfwrite_annotation_latin1(hdl_out, 0LL, -1, "Recording starts");

/***************** start conversion **************************************/

  fseeko(inputfile, (long long)data_chunk_offset + 8LL, SEEK_SET);

  QProgressDialog progress("Converting a Wave file ...", "Abort", 0, (int)blocks, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(0);

  progress_steps = blocks / 100LL;
  if(progress_steps < 1LL)
  {
    progress_steps = 1LL;
  }

  for(k=0LL; k<blocks; k++)
  {
    if(!(k%progress_steps))
    {
      progress.setValue((int)k);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        edfclose_file(hdl_out);
        fclose(inputfile);
        free(buf1);
        free(buf2);
        free(buf3);
        deleteFilters();
        enable_widgets(true);
        return;
      }
    }

    if(fread(buf1, readbufsize, 1, inputfile) != 1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred during conversion.");
      messagewindow.exec();
      edfclose_file(hdl_out);
      fclose(inputfile);
      free(buf1);
      free(buf2);
      free(buf3);
      deleteFilters();
      enable_widgets(true);
      return;
    }

    for(i=0; i<uf; i++)
    {
      if(i % usf)
      {
        buf2[i] = run_ravg_filter(0, lpf9a);
      }
      else
      {
        buf2[i] = run_ravg_filter((*(((signed short *)buf1) + (i / usf))) * (usf * 2), lpf9a);
      }
      buf2[i] = run_ravg_filter(buf2[i], lpf9b);
      buf2[i] = run_ravg_filter(buf2[i], lpf9c);
      buf2[i] = run_ravg_filter(buf2[i], lpf9d);
      buf2[i] = run_ravg_filter(buf2[i], lpf9e);
      buf2[i] = run_ravg_filter(buf2[i], hpf44);

      smpls++;

      if((buf2[i] >= 0) && (!pos))
      {
        pos = 1;

        freq = (uf * 320) / smpls;

        if(freq < freq_min)
        {
          freq = freq_min;
        }
        else if(freq > freq_max)
          {
            freq = freq_max;
          }

        smpls = 0;
      }
      else if((buf2[i] < 0) && pos)
        {
          pos = 0;
        }

      buf2[i] = freq - 608000;
      buf2[i] = run_ravg_filter(buf2[i], lpf200a);
      buf2[i] = run_ravg_filter(buf2[i], lpf200b);
      buf2[i] = run_ravg_filter(buf2[i], lpf200c);
      buf2[i] = run_ravg_filter(buf2[i], lpf200d);
      buf2[i] = run_ravg_filter(buf2[i], lpf200e);

      if(!(i % dsf))
      {
        buf3[i / dsf] = buf2[i] / 8;
      }
    }

    if(edf_blockwrite_digital_samples(hdl_out, buf3))
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A write error occurred during conversion.");
      messagewindow.exec();
      edfclose_file(hdl_out);
      fclose(inputfile);
      free(buf1);
      free(buf2);
      free(buf3);
      deleteFilters();
      enable_widgets(true);
      return;
    }
  }

  progress.reset();

  edfwrite_annotation_latin1(hdl_out, blocks * 10000LL, -1, "Recording ends");

  edfclose_file(hdl_out);

  fclose(inputfile);

  free(buf1);
  free(buf2);
  free(buf3);

  deleteFilters();

  QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();

  enable_widgets(true);
}


void UI_FMaudio2EDFwindow::enable_widgets(bool toggle)
{
  StartDatetimeedit->setEnabled(toggle);
  PatientnameLineEdit->setEnabled(toggle);
  RecordingLineEdit->setEnabled(toggle);
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}


void UI_FMaudio2EDFwindow::deleteFilters(void)
{
  free_ravg_filter(lpf9a);
  free_ravg_filter(lpf9b);
  free_ravg_filter(lpf9c);
  free_ravg_filter(lpf9d);
  free_ravg_filter(lpf9e);
  free_ravg_filter(hpf44);
  free_ravg_filter(lpf200a);
  free_ravg_filter(lpf200b);
  free_ravg_filter(lpf200c);
  free_ravg_filter(lpf200d);
  free_ravg_filter(lpf200e);
}


bool UI_FMaudio2EDFwindow::allocateFilters(int uf, int usf, int dsf)
{
  lpf9a = create_ravg_filter(1, usf * 2);
  if(lpf9a == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf9b = create_ravg_filter(1, usf * 2);
  if(lpf9b == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf9c = create_ravg_filter(1, usf * 2);
  if(lpf9c == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf9d = create_ravg_filter(1, usf * 2);
  if(lpf9d == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf9e = create_ravg_filter(1, usf * 2);
  if(lpf9e == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  hpf44 = create_ravg_filter(0, uf / 700);
  if(hpf44 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf200a = create_ravg_filter(1, dsf * 2);
  if(lpf200a == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf200b = create_ravg_filter(1, dsf * 2);
  if(lpf200b == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf200c = create_ravg_filter(1, dsf * 2);
  if(lpf200c == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf200d = create_ravg_filter(1, dsf * 2);
  if(lpf200d == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  lpf200e = create_ravg_filter(1, dsf * 2);
  if(lpf200e == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (ravg filter).");
    messagewindow.exec();
    return false;
  }

  return true;
}


void UI_FMaudio2EDFwindow::helpbuttonpressed()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html#FMaudio_to_EDF"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strcpy(p_path, "file:///");
  strcat(p_path, mainwindow->specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data());
  strcat(p_path, "\\EDFbrowser\\manual.html#FMaudio_to_EDF");
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}



















