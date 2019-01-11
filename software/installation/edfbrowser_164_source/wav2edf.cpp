/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "wav2edf.h"



UI_WAV2EDFwindow::UI_WAV2EDFwindow(char *recent_dir, char *save_dir)
{
  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 300);
  myobjectDialog->setMaximumSize(600, 300);
  myobjectDialog->setWindowTitle("Wave to EDF converter");
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

  PhysDimLabel = new QLabel(myobjectDialog);
  PhysDimLabel->setGeometry(20, 125, 140, 25);
  PhysDimLabel->setText("Physical dimension");

  PhysMaxLabel = new QLabel(myobjectDialog);
  PhysMaxLabel->setGeometry(20, 160, 140, 25);
  PhysMaxLabel->setText("Physical maximum");

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

  PhysDimLineEdit = new QLineEdit(myobjectDialog);
  PhysDimLineEdit->setGeometry(160, 125, 140, 25);
  PhysDimLineEdit->setMaxLength(8);
  PhysDimLineEdit->setText("mV");

  PhysMaxSpinBox = new QDoubleSpinBox(myobjectDialog);
  PhysMaxSpinBox->setGeometry(160, 160, 140, 25);
  PhysMaxSpinBox->setDecimals(5);
  PhysMaxSpinBox->setRange(1.0, 1000000.0);
  PhysMaxSpinBox->setValue(2000.0);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 250, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 250, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_WAV2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL;

  int i, j,
      edfsignals,
      sf,
      resolution,
      edf_hdl,
      readbufsize,
      *writebuf,
      bytes_per_sample,
      sf_divider;

unsigned int fmt_chunk_offset,
             data_chunk_offset,
             tmp;

  char path[MAX_PATH_LENGTH],
       outputfilename[MAX_PATH_LENGTH],
       scratchpad[512],
       *readbuf;

  long long blocks,
            leftover,
            progress_steps,
            k;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;


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
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Channels < 1");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(edfsignals > MAXSIGNALS)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Channels > MAXSIGNALS");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  sf = *((unsigned int *)(scratchpad + 12));

  if(sf < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency < 1");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(sf > 500000)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency > 500000");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  resolution = *((unsigned short *)(scratchpad + 22));

  if(resolution < 8)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Resolution < 8 bit");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(resolution > 24)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Resolution > 24");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if((resolution != 8) && (resolution != 16)  && (resolution != 24))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Resolution (bitdepth) must be 8, 16 or 24 bit.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  bytes_per_sample = 2;

  if(resolution > 16)
  {
    bytes_per_sample = 3;
  }

  if(resolution < 9)
  {
    bytes_per_sample = 1;
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

  sf_divider = 1;

  if((sf % 10) == 0)
  {
    sf_divider = 10;

    sf /= 10;
  }

  blocks = (long long)(*((int *)(scratchpad + 4)));

  blocks /= (sf * edfsignals * bytes_per_sample);

  fseeko(inputfile, 0LL, SEEK_END);

  leftover = ftello(inputfile) - (long long)data_chunk_offset - 8LL;

  leftover /= (sf * edfsignals * bytes_per_sample);

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

  readbufsize = bytes_per_sample * sf * edfsignals;

  readbuf = (char *)malloc(readbufsize);
  if(readbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (readbuf).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  writebuf = (int *)malloc(sf * edfsignals * sizeof(int));
  if(writebuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (writebuf).");
    messagewindow.exec();
    free(readbuf);
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

//  printf("resolution is %i    edfsignals is %i    sf is %i    blocks is %lli\n", resolution, edfsignals, sf, blocks);

/***************** create a new EDF file *****************************************/

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);
  remove_extension_from_filename(outputfilename);
  if(resolution > 16)
  {
    strcat(outputfilename, ".bdf");
  }
  else
  {
    strcat(outputfilename, ".edf");
  }

  path[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(path, recent_savedir);
    strcat(path, "/");
  }
  strcat(path, outputfilename);

  if(resolution > 16)
  {
    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());
  }
  else
  {
    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());
  }

  if(!strcmp(path, ""))
  {
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  if(resolution > 16)
  {
    edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_BDFPLUS, edfsignals);
  }
  else
  {
    edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, edfsignals);
  }

  if(edf_hdl < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open output file for writing.");
    messagewindow.exec();
    fclose(inputfile);
    free(readbuf);
    free(writebuf);
    enable_widgets(true);
    return;
  }

  for(i=0; i<edfsignals; i++)
  {
    edf_set_samplefrequency(edf_hdl, i, sf);
  }

  if(sf_divider != 1)
  {
    edf_set_datarecord_duration(edf_hdl, 100000 / sf_divider);
  }

  for(i=0; i<edfsignals; i++)
  {
    if(resolution > 16)
    {
      edf_set_digital_maximum(edf_hdl, i, 8388607);
    }
    else
    {
      if(resolution < 9)
      {
        edf_set_digital_maximum(edf_hdl, i, 127);
      }
      else
      {
        edf_set_digital_maximum(edf_hdl, i, 32767);
      }
    }
  }

  for(i=0; i<edfsignals; i++)
  {
    if(resolution > 16)
    {
      edf_set_digital_minimum(edf_hdl, i, -8388608);
    }
    else
    {
      if(resolution < 9)
      {
        edf_set_digital_minimum(edf_hdl, i, -128);
      }
      else
      {
        edf_set_digital_minimum(edf_hdl, i, -32768);
      }
    }
  }

  for(i=0; i<edfsignals; i++)
  {
    edf_set_physical_maximum(edf_hdl, i, PhysMaxSpinBox->value());
  }

  for(i=0; i<edfsignals; i++)
  {
    edf_set_physical_minimum(edf_hdl, i, PhysMaxSpinBox->value() * -1.0);
  }

  for(i=0; i<edfsignals; i++)
  {
    edf_set_physical_dimension(edf_hdl, i, PhysDimLineEdit->text().toLatin1().data());
  }

  for(i=0; i<edfsignals; i++)
  {
    sprintf(scratchpad, "channel %i", i + 1);

    edf_set_label(edf_hdl, i, scratchpad);
  }

  edf_set_patientname(edf_hdl, PatientnameLineEdit->text().toLatin1().data());

  edf_set_recording_additional(edf_hdl, RecordingLineEdit->text().toLatin1().data());

  edf_set_startdatetime(edf_hdl, StartDatetimeedit->date().year(), StartDatetimeedit->date().month(), StartDatetimeedit->date().day(),
                        StartDatetimeedit->time().hour(), StartDatetimeedit->time().minute(), StartDatetimeedit->time().second());

  edfwrite_annotation_latin1(edf_hdl, 0LL, -1, "Recording starts");

/***************** start conversion **************************************/

  fseeko(inputfile, (long long)data_chunk_offset + 8LL, SEEK_SET);

  QProgressDialog progress("Converting a Wave file ...", "Abort", 0, (int)blocks, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

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
        edfclose_file(edf_hdl);
        fclose(inputfile);
        free(readbuf);
        free(writebuf);
        enable_widgets(true);
        return;
      }
    }

    if(fread(readbuf, readbufsize, 1, inputfile)!=1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read error occurred during conversion.");
      messagewindow.exec();
      edfclose_file(edf_hdl);
      fclose(inputfile);
      free(readbuf);
      free(writebuf);
      enable_widgets(true);
      return;
    }

    if(bytes_per_sample == 1)
    {
      for(i=0; i<sf; i++)
      {
        for(j=0; j<edfsignals; j++)
        {
          writebuf[i + (j * sf)] = (signed char)(*(readbuf + (i * edfsignals) + j) + 128);
        }
      }
    }

    if(bytes_per_sample == 2)
    {
      for(i=0; i<sf; i++)
      {
        for(j=0; j<edfsignals; j++)
        {
          writebuf[i + (j * sf)] = *(((signed short *)readbuf) + (i * edfsignals) + j);
        }
      }
    }

    if(bytes_per_sample == 3)
    {
      for(i=0; i<sf; i++)
      {
        for(j=0; j<edfsignals; j++)
        {
          var.two[0] = *((unsigned short *)(readbuf + (i * edfsignals) + (j * 3)));

          var.four[2] = *((unsigned char *)(readbuf + (i * edfsignals * 3) + (j * 3) + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          writebuf[i + (j * sf)] = var.one_signed;
        }
      }
    }

    if(edf_blockwrite_digital_samples(edf_hdl, writebuf))
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A write error occurred during conversion.");
      messagewindow.exec();
      edfclose_file(edf_hdl);
      fclose(inputfile);
      free(readbuf);
      free(writebuf);
      enable_widgets(true);
      return;
    }
  }

  progress.reset();

  edfwrite_annotation_latin1(edf_hdl, (blocks * 10000LL) / sf_divider, -1, "Recording ends");

  edfclose_file(edf_hdl);

  fclose(inputfile);

  QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();

  free(readbuf);
  free(writebuf);

  enable_widgets(true);
}


void UI_WAV2EDFwindow::enable_widgets(bool toggle)
{
  StartDatetimeedit->setEnabled(toggle);
  PatientnameLineEdit->setEnabled(toggle);
  RecordingLineEdit->setEnabled(toggle);
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}



