/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#include "mit2edf.h"


#define NOTQRS  0 /* not-QRS (not a getann/putann code) */
#define NORMAL  1 /* normal beat */
#define LBBB  2 /* left bundle branch block beat */
#define RBBB  3 /* right bundle branch block beat */
#define ABERR 4 /* aberrated atrial premature beat */
#define PVC 5 /* premature ventricular contraction */
#define FUSION  6 /* fusion of ventricular and normal beat */
#define NPC 7 /* nodal (junctional) premature beat */
#define APC 8 /* atrial premature contraction */
#define SVPB  9 /* premature or ectopic supraventricular beat */
#define VESC  10  /* ventricular escape beat */
#define NESC  11  /* nodal (junctional) escape beat */
#define PACE  12  /* paced beat */
#define UNKNOWN 13  /* unclassifiable beat */
#define NOISE 14  /* signal quality change */
#define ARFCT 16  /* isolated QRS-like artifact */
#define STCH  18  /* ST change */
#define TCH 19  /* T-wave change */
#define SYSTOLE 20  /* systole */
#define DIASTOLE 21 /* diastole */
#define NOTE  22  /* comment annotation */
#define MEASURE 23  /* measurement annotation */
#define PWAVE 24  /* P-wave peak */
#define BBB 25  /* left or right bundle branch block */
#define PACESP  26  /* non-conducted pacer spike */
#define TWAVE 27  /* T-wave peak */
#define RHYTHM  28  /* rhythm change */
#define UWAVE 29  /* U-wave peak */
#define LEARN 30  /* learning */
#define FLWAV 31  /* ventricular flutter wave */
#define VFON  32  /* start of ventricular flutter/fibrillation */
#define VFOFF 33  /* end of ventricular flutter/fibrillation */
#define AESC  34  /* atrial escape beat */
#define SVESC 35  /* supraventricular escape beat */
#define LINK    36  /* link to external data (aux contains URL) */
#define NAPC  37  /* non-conducted P-wave (blocked APB) */
#define PFUS  38  /* fusion of paced and normal beat */
#define WFON  39  /* waveform onset */
#define PQ  WFON  /* PQ junction (beginning of QRS) */
#define WFOFF 40  /* waveform end */
#define JPT WFOFF /* J point (end of QRS) */
#define RONT  41  /* R-on-T premature ventricular contraction */

/* ... annotation codes between RONT+1 and ACMAX inclusive are user-defined */

#define ACMAX 49  /* value of largest valid annot code (must be < 50) */


static char annotdescrlist[42][48]=
  {"not-QRS","normal beat",
  "left bundle branch block beat", "right bundle branch block beat",
  "aberrated atrial premature beat", "premature ventricular contraction",
  "fusion of ventricular and normal beat", "nodal (junctional) premature beat",
  "atrial premature contraction", "premature or ectopic supraventricular beat",
  "ventricular escape beat", "nodal (junctional) escape beat",
  "paced beat", "unclassifiable beat",
  "signal quality change", "isolated QRS-like artifact",
  "ST change", "T-wave change",
  "systole", "diastole",
  "comment annotation", "measurement annotation",
  "P-wave peak", "left or right bundle branch block",
  "non-conducted pacer spike", "T-wave peak",
  "rhythm change", "U-wave peak",
  "learning", "ventricular flutter wave",
  "start of ventricular flutter/fibrillation", "end of ventricular flutter/fibrillation",
  "atrial escape beat", "supraventricular escape beat",
  "link to external data (aux contains URL)", "non-conducted P-wave (blocked APB)",
  "fusion of paced and normal beat", "waveform onset",
  "waveform end", "R-on-T premature ventricular contraction"};


#define ANNOT_EXT_CNT   8


static char annotextlist[ANNOT_EXT_CNT][16]=
  {
    ".ari",
    ".ecg",
    ".trigger",
    ".qrs",
    ".atr",
    ".apn",
    ".st",
    ".pwave"
  };


UI_MIT2EDFwindow::UI_MIT2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("MIT to EDF+ converter");
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
  sprintf(txt_string, "MIT (PhysioBank) to EDF+ converter.\n");
  textEdit1->append(txt_string);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_MIT2EDFwindow::SelectFileButton()
{
  FILE *header_inputfile=NULL,
       *data_inputfile=NULL,
       *annot_inputfile=NULL;

  int i, j, p, len, hdl, *buf;

  char header_filename[MAX_PATH_LENGTH],
       txt_string[2048],
       edf_filename[MAX_PATH_LENGTH],
       data_filename[MAX_PATH_LENGTH],
       annot_filename[MAX_PATH_LENGTH],
       filename_x[MAX_PATH_LENGTH],
       scratchpad[4096],
       *charpntr;

  unsigned char a_buf[128];

  long long filesize;

  union {
    int one;
    unsigned char four[4];
  } var;

  pushButton1->setEnabled(false);

  strcpy(header_filename, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "MIT header files (*.hea *.HEA)").toLocal8Bit().data());

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

  get_filename_from_path(filename_x, header_filename, MAX_PATH_LENGTH);

  snprintf(txt_string, 2048, "Read file: %s", filename_x);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  remove_extension_from_filename(filename_x);

  charpntr = fgets(scratchpad, 4095, header_inputfile);
  if(charpntr == NULL)
  {
    textEdit1->append("Can not read header file. (error 1)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  len = strlen(charpntr);
  if(len < 6)
  {
    textEdit1->append("Can not read header file. (error 2)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  for(i=0; i<len; i++)
  {
    if(charpntr[i] == ' ')
    {
      charpntr[i] = 0;

      break;
    }
  }

  if(i == len)
  {
    textEdit1->append("Can not read header file. (error 3)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(strcmp(charpntr, filename_x))
  {
    textEdit1->append("Can not read header file. (error 4)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  p = ++i;

  for(; i<len; i++)
  {
    if(charpntr[i] == ' ')
    {
      charpntr[i] = 0;

      break;
    }
  }

  if(i == p)
  {
    textEdit1->append("Can not read header file. (error 5)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.chns = atoi(charpntr + p);

  if(mit_hdr.chns < 1)
  {
    textEdit1->append("Error, number of signals is less than one. (error 6)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.chns > MAXSIGNALS)
  {
    textEdit1->append("Error, Too many signals in header. (error 7)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  p = ++i;

  for(; i<len; i++)
  {
    if(charpntr[i] == ' ')
    {
      charpntr[i] = 0;

      break;
    }
  }

  if(i == p)
  {
    textEdit1->append("Can not read header file. (error 8)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.sf = atoi(charpntr + p);

  if(mit_hdr.sf < 1)
  {
    textEdit1->append("Error, samplefrequency is less than 1 Hz. (error 9)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.sf > 100000)
  {
    textEdit1->append("Error, samplefrequency is more than 100000 Hz. (error 10)\n");
    fclose(header_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  mit_hdr.smp_period = 1000000000LL / mit_hdr.sf;

  strcat(filename_x, ".dat");

  for(j=0; j<mit_hdr.chns; j++)
  {
    mit_hdr.adc_gain[j] = 200.0;

    mit_hdr.adc_resolution[j] = 12;

    mit_hdr.adc_zero[j] = 0;

    mit_hdr.init_val[j] = 0;

    sprintf(mit_hdr.label[j], "chan. %i", j + 1);

    charpntr = fgets(scratchpad, 4095, header_inputfile);
    if(charpntr == NULL)
    {
      textEdit1->append("Can not read header file. (error 11)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    len = strlen(charpntr);
    if(len < 6)
    {
      textEdit1->append("Can not read header file. (error 12)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    for(i=0; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == len)
    {
      textEdit1->append("Can not read header file. (error 13)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    if(strcmp(charpntr, filename_x))
    {
      textEdit1->append("Error, filenames are different. (error 14)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == len)
    {
      textEdit1->append("Can not read header file. (error 15)\n");
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    mit_hdr.format[j] = atoi(charpntr + p);

    if((mit_hdr.format[j] != 212) &&
      (mit_hdr.format[j] != 16) &&
      (mit_hdr.format[j] != 32) &&
      (mit_hdr.format[j] != 61))
    {
      snprintf(txt_string, 2048, "Error, unsupported format: %i  (error 16)\n", mit_hdr.format[j]);
      textEdit1->append(txt_string);
      fclose(header_inputfile);
      pushButton1->setEnabled(true);
      return;
    }

    if(j>0)
    {
      if(mit_hdr.format[j] != mit_hdr.format[0])
      {
        textEdit1->append("Error, different formats in the same file. (error 17)\n");
        fclose(header_inputfile);
        pushButton1->setEnabled(true);
        return;
      }
    }

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    if(atoi(charpntr + p) != 0)
    {
      mit_hdr.adc_gain[j] = atof(charpntr + p);
    }

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    mit_hdr.adc_resolution[j] = atoi(charpntr + p);

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    mit_hdr.adc_zero[j] = atoi(charpntr + p);

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    mit_hdr.init_val[j] = atoi(charpntr + p);

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    // skip

    p = ++i;

    for(; i<len; i++)
    {
      if(charpntr[i] == ' ')
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    // skip

    p = ++i;

    for(; i<len; i++)
    {
      if((charpntr[i] == '\n') || (charpntr[i] == '\r'))
      {
        charpntr[i] = 0;

        break;
      }
    }

    if(i == p)
    {
      continue;
    }

    strncpy(mit_hdr.label[j], charpntr + p, 16);

    mit_hdr.label[j][16] = 0;
  }

  fclose(header_inputfile);

  strcpy(data_filename, header_filename);

  remove_extension_from_filename(data_filename);

  strcpy(edf_filename, data_filename);

  strcpy(annot_filename, data_filename);

  strcat(data_filename, ".dat");

  strcat(edf_filename, ".edf");

  strcat(annot_filename, ".atr");

  data_inputfile = fopeno(data_filename, "rb");
  if(data_inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.\n", data_filename);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    pushButton1->setEnabled(true);
    return;
  }

  get_filename_from_path(filename_x, data_filename, MAX_PATH_LENGTH);

  snprintf(txt_string, 2048, "Read file: %s (format: %i)", filename_x, mit_hdr.format[0]);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  remove_extension_from_filename(filename_x);

  fseeko(data_inputfile, 0LL, SEEK_END);
  filesize = ftello(data_inputfile);
  if(filesize < (mit_hdr.chns * mit_hdr.sf * 45 / 10))
  {
    textEdit1->append("Error, .dat filesize is too small.\n");
    fclose(data_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  for(mit_hdr.sf_div=10; mit_hdr.sf_div>0; mit_hdr.sf_div--)
  {
    if(mit_hdr.sf_div == 9)  continue;
    if(mit_hdr.sf_div == 7)  continue;
    if(mit_hdr.sf_div == 6)  continue;
    if(mit_hdr.sf_div == 3)  continue;

    if(!(mit_hdr.sf % mit_hdr.sf_div))  break;
  }

  if(mit_hdr.sf_div < 1)  mit_hdr.sf_div = 1;

  mit_hdr.sf_block = mit_hdr.sf / mit_hdr.sf_div;

  hdl = edfopen_file_writeonly(edf_filename, EDFLIB_FILETYPE_EDFPLUS, mit_hdr.chns);

  if(hdl<0)
  {
    snprintf(txt_string, 2048, "Can not open file %s for writing.\n", edf_filename);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    fclose(data_inputfile);
    pushButton1->setEnabled(true);
    return;
  }

  if(mit_hdr.sf_div == 1)
  {
    if(edf_set_number_of_annotation_signals(hdl, 2))
    {
      textEdit1->append("Error: edf_set_number_of_annotation_signals()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_samplefrequency(hdl, i, mit_hdr.sf_block))
    {
      textEdit1->append("Error: edf_set_samplefrequency()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_digital_minimum(hdl, i, -32768))
    {
      textEdit1->append("Error: edf_set_digital_minimum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_digital_maximum(hdl, i, 32767))
    {
      textEdit1->append("Error: edf_set_digital_maximum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_label(hdl, i, mit_hdr.label[i]))
    {
      textEdit1->append("Error: edf_set_label()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  for(i=0; i<mit_hdr.chns; i++)
  {
    if(edf_set_physical_dimension(hdl, i, "uV"))
    {
      textEdit1->append("Error: edf_set_physical_dimension()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_physical_maximum(hdl, i, (double)((32767 - mit_hdr.adc_zero[i]) * 1000) / mit_hdr.adc_gain[i]))
    {
      textEdit1->append("Error: edf_set_physical_maximum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }

    if(edf_set_physical_minimum(hdl, i, (double)((-32768 - mit_hdr.adc_zero[i]) * 1000) / mit_hdr.adc_gain[i]))
    {
      textEdit1->append("Error: edf_set_physical_minimum()\n");
      fclose(data_inputfile);
      edfclose_file(hdl);
      pushButton1->setEnabled(true);
      return;
    }
  }

  if(edf_set_datarecord_duration(hdl, 100000 / mit_hdr.sf_div))
  {
    textEdit1->append("Error: edf_set_datarecord_duration()\n");
    fclose(data_inputfile);
    edfclose_file(hdl);
    pushButton1->setEnabled(true);
    return;
  }

  buf = (int *)malloc(mit_hdr.sf_block * mit_hdr.chns * sizeof(int));
  if(buf == NULL)
  {
    textEdit1->append("Malloc() error (buf)\n");
    fclose(data_inputfile);
    edfclose_file(hdl);
    pushButton1->setEnabled(true);
    return;
  }

/////////////////// Start conversion //////////////////////////////////////////

  int k, blocks, tmp1, tmp2, l_end=0;

  fseeko(data_inputfile, 0LL, SEEK_SET);

  blocks = filesize / (mit_hdr.sf_block * mit_hdr.chns);

  QProgressDialog progress("Converting digitized signals ...", "Abort", 0, blocks);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  if(mit_hdr.format[0] == 212)
  {
    blocks *= 10;
    blocks /= 15;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          if(j % 2)
          {
            tmp1 = fgetc(data_inputfile);
            tmp2 = fgetc(data_inputfile);

            if(tmp2 == EOF)
            {
              goto OUT;
            }

            buf[j * mit_hdr.sf_block + i] = (tmp1 & 0xf0) << 4;
            buf[j * mit_hdr.sf_block + i] += tmp2;
            if(buf[j * mit_hdr.sf_block + i] & 0x800)
            {
              buf[j * mit_hdr.sf_block + i] |= 0xfffff000;
            }
          }
          else
          {
            tmp1 = fgetc(data_inputfile);
            tmp2 = fgetc(data_inputfile);

            buf[j * mit_hdr.sf_block + i] = (tmp2 & 0x0f) << 8;
            buf[j * mit_hdr.sf_block + i] += tmp1;
            if(buf[j * mit_hdr.sf_block + i] & 0x800)
            {
              buf[j * mit_hdr.sf_block + i] |= 0xfffff000;
            }

            fseeko(data_inputfile, -1LL, SEEK_CUR);
          }
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if((mit_hdr.format[0] == 16) || (mit_hdr.format[0] == 61))
  {
    if(mit_hdr.format[0] == 16)  l_end = 1;

    blocks /= 2;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          tmp1 = fgetc(data_inputfile);
          if(tmp1 == EOF)
          {
            goto OUT;
          }

          if(l_end)
          {
            tmp1 += (fgetc(data_inputfile) << 8);
          }
          else
          {
            tmp1 <<= 8;

            tmp1 += fgetc(data_inputfile);
          }

          if(tmp1 & 0x8000)
          {
            tmp1 |= 0xffff0000;
          }

          buf[j * mit_hdr.sf_block + i] = tmp1;
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

  if(mit_hdr.format[0] == 32)
  {
    blocks /= 4;

    progress.setMaximum(blocks);

    for(k=0; k<blocks; k++)
    {
      if(!(k % 100))
      {
        progress.setValue(k);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          textEdit1->append("Conversion aborted by user.\n");
          fclose(data_inputfile);
          edfclose_file(hdl);
          free(buf);
          pushButton1->setEnabled(true);
          return;
        }
      }

      for(i=0; i<mit_hdr.sf_block; i++)
      {
        for(j=0; j<mit_hdr.chns; j++)
        {
          tmp1 = fgetc(data_inputfile);
          if(tmp1 == EOF)
          {
            goto OUT;
          }

          var.four[0] = tmp1;
          var.four[1] = fgetc(data_inputfile);
          var.four[2] = fgetc(data_inputfile);
          var.four[3] = fgetc(data_inputfile);

          buf[j * mit_hdr.sf_block + i] = var.one;
        }
      }

      if(edf_blockwrite_digital_samples(hdl, buf))
      {
        progress.reset();
        textEdit1->append("A write error occurred during conversion.\n");
        fclose(data_inputfile);
        edfclose_file(hdl);
        free(buf);
        pushButton1->setEnabled(true);
        return;
      }
    }
  }

OUT:

  progress.reset();

  qApp->processEvents();

/////////////////// End conversion //////////////////////////////////////////

  fclose(data_inputfile);

  free(buf);

  int annot_code, tc, skip, total_annots=0;

  long long bytes_read;

  get_filename_from_path(filename_x, annot_filename, MAX_PATH_LENGTH);

  for(k=0; k<ANNOT_EXT_CNT; k++)
  {
    tc = 0;

    remove_extension_from_filename(annot_filename);

    strcat(annot_filename, annotextlist[k]);

    annot_inputfile = fopeno(annot_filename, "rb");
    if(annot_inputfile==NULL)
    {
      continue;
    }

    get_filename_from_path(filename_x, annot_filename, MAX_PATH_LENGTH);

    if(annot_inputfile==NULL)
    {
      snprintf(txt_string, 2048, "Can not open file %s for reading.\n"
                                 "Annotations can not be included.", filename_x);
      textEdit1->append(QString::fromLocal8Bit(txt_string));
    }
    else
    {
      snprintf(txt_string, 2048, "Read file: %s", filename_x);
      textEdit1->append(QString::fromLocal8Bit(txt_string));

      fseeko(annot_inputfile, 0LL, SEEK_END);
      filesize = ftello(annot_inputfile);

      progress.setLabelText("Converting annotations ...");
      progress.setMinimum(0);
      progress.setMaximum(filesize);

      fseeko(annot_inputfile, 0LL, SEEK_SET);

      for(bytes_read=0LL; bytes_read < filesize; bytes_read += 2LL)
      {
        if(!(bytes_read % 100))
        {
          progress.setValue(bytes_read);

          qApp->processEvents();

          if(progress.wasCanceled() == true)
          {
            textEdit1->append("Conversion aborted by user.\n");

            break;
          }
        }

        skip = 0;

        if(fread(a_buf, 2, 1, annot_inputfile) != 1)
        {
          break;
        }

  #pragma GCC diagnostic ignored "-Wstrict-aliasing"

        if(*((unsigned short *)a_buf) == 0)  // end of file
        {
          break;
        }

        annot_code = a_buf[1] >> 2;

        if(annot_code == 59)
        {
          if(fread(a_buf, 4, 1, annot_inputfile) != 1)
          {
            break;
          }

          tc += (*((unsigned short *)a_buf) << 16);

          tc += *((unsigned short *)(a_buf + 2));
        }
        else if(annot_code == 63)
          {
            skip = *((unsigned short *)a_buf) & 0x3ff;

            if(skip % 2) skip++;
          }
          else if((annot_code >= 0) && (annot_code <= ACMAX))
            {
              tc += *((unsigned short *)a_buf) & 0x3ff;

  #pragma GCC diagnostic warning "-Wstrict-aliasing"

              if(annot_code < 42)
              {
                edfwrite_annotation_latin1(hdl, ((long long)tc * mit_hdr.smp_period) / 100000LL, -1, annotdescrlist[annot_code]);
              }
              else
              {
                edfwrite_annotation_latin1(hdl, ((long long)tc * mit_hdr.smp_period) / 100000LL, -1, "user-defined");
              }

              total_annots++;
            }

        if(skip)
        {
          if(fseek(annot_inputfile, skip, SEEK_CUR) < 0)
          {
            break;
          }

          bytes_read += skip;
        }
      }

      fclose(annot_inputfile);
    }
  }

  if(total_annots)
  {
    snprintf(txt_string, 2048, "Read %i annotations.", total_annots);
    textEdit1->append(txt_string);
  }

  progress.reset();

  edfclose_file(hdl);

  textEdit1->append("Ready.\n");

  pushButton1->setEnabled(true);
}

















