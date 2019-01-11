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




#include "biox2edf.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "edflib.h"


#define EDF_CHNS   3

#define MAX_POSSIBLE_SF  256


#define READ_BLOCK_SZ   (256 * 4)


UI_BIOX2EDFwindow::UI_BIOX2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Biox CB-1305-C to EDF converter");
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
  sprintf(txt_string, "Biox CB-1305-C to EDF converter.\n");
  textEdit1->append(txt_string);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_BIOX2EDFwindow::SelectFileButton()
{
  int i, j,
      hdl,
      chns=3,
      sf,
      wr_buf[MAX_POSSIBLE_SF * EDF_CHNS],
      blocks;

  char str[512],
       rd_buf[READ_BLOCK_SZ],
       txt_string[2048],
       edf_filename[MAX_PATH_LENGTH],
       data_filename[MAX_PATH_LENGTH];

  unsigned char tmp_bits;

  FILE *inputfile=NULL;

  long long file_sz;


  pushButton1->setEnabled(false);

  strcpy(data_filename, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "Biox CB-1305-C data files (*.dat *.DAT)").toLocal8Bit().data());

  if(!strcmp(data_filename, ""))
  {
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_opendir, data_filename, MAX_PATH_LENGTH);

  inputfile = fopeno(data_filename, "rb");
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.\n", data_filename);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    pushButton1->setEnabled(true);
    return;
  }

  snprintf(txt_string, 2048, "Read file: %s", data_filename);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  fseeko(inputfile, 0x00, SEEK_END);

  file_sz = ftello(inputfile);

  if(file_sz < 2048)
  {
    textEdit1->append("File is smaller than 2048 bytes\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 0x0100, SEEK_SET);

  if(fread(str, 17, 1, inputfile) != 1)
  {
    textEdit1->append("Can not read ID string from file\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(strncmp(str, "CB-1305-C SD-ECG3", 17))
  {
    textEdit1->append("Wrong ID string in file\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 0x0132, SEEK_SET);

  if(fread(str, 4, 1, inputfile) != 1)
  {
    textEdit1->append("Can not read parameter (samplefrequency) from file\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

  sf = *((unsigned short *)str);

#pragma GCC diagnostic warning "-Wstrict-aliasing"

  if((sf != 128) && (sf != 256))
  {
    snprintf(txt_string, 2048, "Invalid parameter (samplefrequency) in file: %iHz\n", sf);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  edf_filename[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(edf_filename, recent_savedir);
    strcat(edf_filename, "/");
  }
  strcat(edf_filename, "biox_ecg.edf");

  strcpy(edf_filename, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(edf_filename), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(edf_filename, ""))
  {
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  get_directory_from_path(recent_savedir, edf_filename, MAX_PATH_LENGTH);

  hdl = edfopen_file_writeonly(edf_filename, EDFLIB_FILETYPE_EDFPLUS, chns);

  if(hdl < 0)
  {
    textEdit1->append("Error: can not open EDF file for writing\n");
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_samplefrequency(hdl, i, sf))
    {
      textEdit1->append("Error: edf_set_samplefrequency()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_digital_maximum(hdl, i, 511))
    {
      textEdit1->append("Error: edf_set_digital_maximum()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_digital_minimum(hdl, i, -512))
    {
      textEdit1->append("Error: edf_set_digital_minimum()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_maximum(hdl, i, 5000))
    {
      textEdit1->append("Error: edf_set_physical_maximum()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_minimum(hdl, i, -5000))
    {
      textEdit1->append("Error: edf_set_physical_minimum()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    if(edf_set_physical_dimension(hdl, i, "uV"))
    {
      textEdit1->append("Error: edf_set_physical_dimension()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<chns; i++)
  {
    sprintf(str, "chan. %i", i + 1);

    if(edf_set_label(hdl, i, str))
    {
      textEdit1->append("Error: edf_set_label()\n");
      edfclose_file(hdl);
      fclose(inputfile);
      pushButton1->setEnabled(true);
      return;
    }
  }

  if(edf_set_equipment(hdl, "Biox CB-1305-C SD-ECG3"))
  {
    textEdit1->append("Error: edf_set_equipment()\n");
    edfclose_file(hdl);
    fclose(inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  fseeko(inputfile, 0x0200, SEEK_SET);

  file_sz -= 0x0200;

  file_sz /= READ_BLOCK_SZ;

  QProgressDialog progress("Converting ECG data ...", "Abort", 0, file_sz);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  j = 0;

  for(blocks=0; ; blocks++)
  {
    if(!(blocks % 10))
    {
      progress.setValue(blocks);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        textEdit1->append("Conversion aborted by user.\n");
        edfclose_file(hdl);
        fclose(inputfile);
        pushButton1->setEnabled(true);
        return;
      }
    }

    if(fread(rd_buf, READ_BLOCK_SZ, 1, inputfile) != 1)
    {
      break;
    }

    if(*((int *)(rd_buf + 1020)) == 0)
    {
      break;
    }

    for(i=0; i<(READ_BLOCK_SZ / 4); i++)
    {
      if((i == 127) || (i == 255))  continue;

      tmp_bits = rd_buf[i * 4 + 3];

      rd_buf[i * 4] += 128;

      wr_buf[j] = rd_buf[i * 4];

      wr_buf[j] <<= 2;

      rd_buf[i * 4 + 1] += 128;

      wr_buf[sf + j] = rd_buf[i * 4 + 1];

      wr_buf[sf + j] <<= 2;

      rd_buf[i * 4 + 2] += 128;

      wr_buf[sf * 2 + j] = rd_buf[i * 4 + 2];

      wr_buf[sf * 2 + j] <<= 2;

      wr_buf[j] += (tmp_bits >> 6) & 0b00000011;

      wr_buf[sf + j] += (tmp_bits >> 4) & 0b00000011;

      wr_buf[sf * 2 + j] += (tmp_bits >> 2) & 0b00000011;

      if(++j == sf)
      {
        j = 0;

        if(edf_blockwrite_digital_samples(hdl, wr_buf))
        {
          progress.reset();
          textEdit1->append("Error: a write error occurred during conversion\n");
          edfclose_file(hdl);
          fclose(inputfile);
          pushButton1->setEnabled(true);
          return;
        }
      }
    }
  }

  progress.reset();

  fclose(inputfile);

  edfclose_file(hdl);

  textEdit1->append("Ready.\n");

  pushButton1->setEnabled(true);
}



























