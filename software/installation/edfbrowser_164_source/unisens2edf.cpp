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



#include "unisens2edf.h"



#define US_DATATYPE_FLOAT_LI     0
#define US_DATATYPE_FLOAT_BI     1
#define US_DATATYPE_DOUBLE_LI    2
#define US_DATATYPE_DOUBLE_BI    3
#define US_DATATYPE_INT64_LI     4
#define US_DATATYPE_INT64_BI     5
#define US_DATATYPE_UINT64_LI    6
#define US_DATATYPE_UINT64_BI    7
#define US_DATATYPE_INT32_LI     8
#define US_DATATYPE_INT32_BI     9
#define US_DATATYPE_UINT32_LI   10
#define US_DATATYPE_UINT32_BI   11
#define US_DATATYPE_INT24_LI    12
#define US_DATATYPE_INT24_BI    13
#define US_DATATYPE_UINT24_LI   14
#define US_DATATYPE_UINT24_BI   15
#define US_DATATYPE_INT16_LI    16
#define US_DATATYPE_INT16_BI    17
#define US_DATATYPE_UINT16_LI   18
#define US_DATATYPE_UINT16_BI   19
#define US_DATATYPE_INT8_LI     20
#define US_DATATYPE_INT8_BI     21
#define US_DATATYPE_UINT8_LI    22
#define US_DATATYPE_UINT8_BI    23


#define US_SAMPLERATE_OUT_OF_RANGE  22





UI_UNISENS2EDFwindow::UI_UNISENS2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Unisens to EDF+ converter");
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
  sprintf(txt_string, "Unisens to EDF+ converter.\n");
  textEdit1->append(txt_string);

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_UNISENS2EDFwindow::SelectFileButton()
{
  int i, j, k, n,
      tmp,
      chns,
      hdl,
      err,
      *buf2,
      bufsize,
      blocks_written,
      progress_steps,
      lines;

  short tmp3;

  char path[MAX_PATH_LENGTH],
       binfilepath[MAX_PATH_LENGTH],
       outputfilename[MAX_PATH_LENGTH],
       scratchpad[2048],
       str[256],
       *buf1,
       tmp2,
       linebuf[512];

  struct xml_handle *xml_hdl;


  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "XML files (*.xml *.XML)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);

  if(xml_hdl == NULL)
  {
    sprintf(scratchpad, "Error, can not open file:\n%s\n", path);
    textEdit1->append(QString::fromLocal8Bit(scratchpad));
    return;
  }

  sprintf(scratchpad, "Processing file:\n%s", path);
  textEdit1->append(QString::fromLocal8Bit(scratchpad));

  char_encoding = xml_hdl->encoding;

  if(char_encoding == 0)  // attribute encoding not present
  {
    char_encoding = 2;  // fallback to UTF-8 because it is the default for XML
  }
  else if(char_encoding > 2)  // unknown encoding  FIX ME!!
  {
    char_encoding = 1;  // fallback to ISO-8859-1 (Latin1)
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "unisens"))
  {
    textEdit1->append("Error, can not find root element \"unisens\".\n");
    xml_close(xml_hdl);
    return;
  }

  if(xml_get_attribute_of_element(xml_hdl, "timestampStart", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"timestampStart\".\n");
    return;
  }

  if((strlen(str) < 19) || (strlen(str) > 23) || (str[4] != '-') || (str[7] != '-') || (str[10] != 'T') || (str[13] != ':') || (str[16] != ':'))
  {
    textEdit1->append("Error, can not find attribute \"timestampStart\".\n");
    return;
  }

  err = 0;

  for(i=0; i<19; i++)
  {
    if((i == 4) || (i == 7) || (i == 10) || (i == 13) || (i == 16))  continue;

    if(!(isdigit(str[i])))  err = 1;
  }

  if(strlen(str) == 23)
  {
    if(str[19] != '.')  err = 1;

    for(i=20; i<23; i++)
    {
      if(!(isdigit(str[i])))  err = 1;
    }

    starttime_fraction = atoi(str + 20);

    if((starttime_fraction < 0) || (starttime_fraction > 999))  err = 1;
  }
  else
  {
    starttime_fraction = 0;
  }

  if(err)
  {
    textEdit1->append("Error, invalid format in attribute \"timestampStart\".\n");
    return;
  }

  strncpy(str_timestampStart, str, 19);

  str_timestampStart[19] = 0;

  if(xml_get_attribute_of_element(xml_hdl, "measurementId", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"measurementId\".\n");
    return;
  }

  if(strlen(str) < 1)
  {
    textEdit1->append("Error, attribute \"measurementId\" has no data.\n");
    return;
  }

  if(char_encoding == 1)  // Latin-1
  {
    strncpy(str_measurementId, QString::fromLatin1(str).toLocal8Bit().data(), 128);
  }
  else if(char_encoding == 2)
    {
      strncpy(str_measurementId, QString::fromUtf8(str).toLocal8Bit().data(), 128);
    }
    else
    {
//      strncpy(str_measurementId, str, 128);
      strncpy(str_measurementId, QString::fromUtf8(str).toLocal8Bit().data(), 128);  // default for XML is UTF-8
    }

  str_measurementId[127] = 0;

  xml_goto_root(xml_hdl);

////////////////////////// read parameters from XML-file //////////////////////////////////////////

  total_edf_signals = 0;

  bdf = 0;

  int skip = 0;

  sf_less_1 = 0;

  for(file_cnt=0; file_cnt<MAXFILES; file_cnt++)
  {
    csv_enc[file_cnt] = 0;

    big_endian[file_cnt] = 0;

    nedval_enc[file_cnt] = 0;

    for(i=0; i<MAXSIGNALS; i++)
    {
      nedval_value[file_cnt][i] = 0;

      nedval_value2[file_cnt][i] = 0;
    }

    nedval_smpl[file_cnt] = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalEntry", file_cnt + skip))
    {
      break;
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "csvFileFormat", 0))
    {
      csv_enc[file_cnt] = 1;

      xml_go_up(xml_hdl);
    }

    tmp = get_signalparameters_from_BIN_attributes(xml_hdl, file_cnt);
    if(tmp)
    {
      if(tmp == US_SAMPLERATE_OUT_OF_RANGE)
      {
        xml_go_up(xml_hdl);

        file_cnt--;

        skip++;

        continue;
      }
      else
      {
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }
    }

    if(csv_enc[file_cnt])
    {
      if(xml_goto_nth_element_inside(xml_hdl, "csvFileFormat", 0))
      {
        textEdit1->append("Error, can not find element \"csvFileFormat\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(xml_get_attribute_of_element(xml_hdl, "separator", str, 255) < 0)
      {
        textEdit1->append("Error, can not find attribute \"separator\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(strlen(str) != 1)
      {
        textEdit1->append("Error, value for \"separator\" must be one character.\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      csv_sep[file_cnt] = str[0];

      if(xml_get_attribute_of_element(xml_hdl, "decimalSeparator", str, 255) < 0)
      {
        csv_dec_sep[file_cnt] = '.';  // if attribute decimalSeparator is not present, fall back to a dot
      }
      else
      {
        if(strlen(str) != 1)
        {
          textEdit1->append("Error, value for \"decimalSeparator\" must be one character.\n");
          xml_close(xml_hdl);
          for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
          return;
        }

        csv_dec_sep[file_cnt] = str[0];

        if(csv_sep[file_cnt] == csv_dec_sep[file_cnt])
        {
          textEdit1->append("Error, attribute \"decimalSeparator\" and \"separator\" have equal values.\n");
          xml_close(xml_hdl);
          for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
          return;
        }

      }

      xml_go_up(xml_hdl);
    }
    else
    {
      if(xml_goto_nth_element_inside(xml_hdl, "binFileFormat", 0))
      {
        textEdit1->append("Error, can not find element \"binFileFormat\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(xml_get_attribute_of_element(xml_hdl, "endianess", str, 255) < 0)
      {
        textEdit1->append("Error, can not find attribute \"endianess\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(strlen(str) < 1)
      {
        textEdit1->append("Error, can not find attribute \"endianess\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(strcmp(str, "LITTLE") && strcmp(str, "BIG"))
      {
        textEdit1->append("Error, attribute \"endianess\" has an unknown value.\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(!(strcmp(str, "LITTLE")))
      {
        big_endian[file_cnt] = 0;
      }
      else
      {
        big_endian[file_cnt] = 1;

        datatype[file_cnt]++;
      }

      xml_go_up(xml_hdl);
    }

    edf_signal_start[file_cnt] = total_edf_signals;

    for(chns=0; chns<MAXSIGNALS; chns++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "channel", chns))
      {
        break;
      }

      if(xml_get_attribute_of_element(xml_hdl, "name", str, 255) < 0)
      {
        textEdit1->append("Error, can not find attribute \"name\" in element \"channel\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(char_encoding == 1)  // Latin-1
      {
        strncpy(signallabel[total_edf_signals], str, 16);
      }
      else if(char_encoding == 2)
        {
          strncpy(signallabel[total_edf_signals], QString::fromUtf8(str).toLatin1().data(), 16);
        }
        else
        {
//          strncpy(signallabel[total_edf_signals], str, 16);
          strncpy(signallabel[total_edf_signals], QString::fromUtf8(str).toLatin1().data(), 16);  // default for XML is UTF-8
        }

      signallabel[total_edf_signals][16] = 0;

      total_edf_signals++;

      xml_go_up(xml_hdl);
    }

    edf_signals[file_cnt] = chns;

    if(chns < 1)
    {
      textEdit1->append("Error, no signals in element \"signalEntry\" or in element \"csvFileFormat\".\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    if(chns >= MAXSIGNALS)
    {
      textEdit1->append("Error, too many signals.\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    get_directory_from_path(binfilepath, path, MAX_PATH_LENGTH);

    strcat(binfilepath, "/");
    strcat(binfilepath, binfilename[file_cnt]);

    binfile[file_cnt] = fopeno(binfilepath, "rb");
    if(binfile[file_cnt] == NULL)
    {
      sprintf(scratchpad, "Error, can not open file:\n%s\n", binfilepath);
      textEdit1->append(QString::fromLocal8Bit(scratchpad));
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    xml_go_up(xml_hdl);
  }

////////////////////// read parameters for valuesEntry type files ///////////////////////////

  int value_entry_file_cnt = 0;

  skip = 0;

  for(; file_cnt<MAXFILES; file_cnt++)
  {
    csv_enc[file_cnt] = 0;

    big_endian[file_cnt] = 0;

    nedval_enc[file_cnt] = 0;

    for(i=0; i<MAXSIGNALS; i++)
    {
      nedval_value[file_cnt][i] = 0;

      nedval_value2[file_cnt][i] = 0;
    }

    nedval_smpl[file_cnt] = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "valuesEntry", value_entry_file_cnt + skip))
    {
      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "csvFileFormat", 0))
    {
      xml_go_up(xml_hdl);

      file_cnt--;

      skip++;

      continue;
    }

    csv_enc[file_cnt] = 1;

    nedval_enc[file_cnt] = 1;

    xml_go_up(xml_hdl);

    tmp = get_signalparameters_from_BIN_attributes(xml_hdl, file_cnt);
    if(tmp)
    {
      if(tmp == US_SAMPLERATE_OUT_OF_RANGE)
      {
        xml_go_up(xml_hdl);

        file_cnt--;

        skip++;

        continue;
      }
      else
      {
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }
    }

    if(xml_goto_nth_element_inside(xml_hdl, "csvFileFormat", 0))
    {
      textEdit1->append("Error, can not find element \"csvFileFormat\".\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    if(xml_get_attribute_of_element(xml_hdl, "separator", str, 255) < 0)
    {
      textEdit1->append("Error, can not find attribute \"separator\".\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    if(strlen(str) != 1)
    {
      textEdit1->append("Error, value for \"separator\" must be one character.\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    csv_sep[file_cnt] = str[0];

    if(xml_get_attribute_of_element(xml_hdl, "decimalSeparator", str, 255) < 0)
    {
      csv_dec_sep[file_cnt] = '.';  // if attribute decimalSeparator is not present, fall back to a dot
    }
    else
    {
      if(strlen(str) != 1)
      {
        textEdit1->append("Error, value for \"decimalSeparator\" must be one character.\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      csv_dec_sep[file_cnt] = str[0];

      if(csv_sep[file_cnt] == csv_dec_sep[file_cnt])
      {
        textEdit1->append("Error, attribute \"decimalSeparator\" and \"separator\" have equal values.\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }
    }

    xml_go_up(xml_hdl);

    edf_signal_start[file_cnt] = total_edf_signals;

    for(chns=0; chns<MAXSIGNALS; chns++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "channel", chns))
      {
        break;
      }

      if(xml_get_attribute_of_element(xml_hdl, "name", str, 255) < 0)
      {
        textEdit1->append("Error, can not find attribute \"name\" in element \"channel\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(char_encoding == 1)  // Latin-1
      {
        strncpy(signallabel[total_edf_signals], str, 16);
      }
      else if(char_encoding == 2)
        {
          strncpy(signallabel[total_edf_signals], QString::fromUtf8(str).toLatin1().data(), 16);
        }
        else
        {
//          strncpy(signallabel[total_edf_signals], str, 16);
          strncpy(signallabel[total_edf_signals], QString::fromUtf8(str).toLatin1().data(), 16);  // default for XML is UTF-8
        }

      signallabel[total_edf_signals][16] = 0;

      total_edf_signals++;

      xml_go_up(xml_hdl);
    }

    edf_signals[file_cnt] = chns;

    if(chns < 1)
    {
      textEdit1->append("Error, no signals in element \"valuesEntry\".\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    if(chns >= MAXSIGNALS)
    {
      textEdit1->append("Error, too many signals.\n");
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    get_directory_from_path(binfilepath, path, MAX_PATH_LENGTH);

    strcat(binfilepath, "/");
    strcat(binfilepath, binfilename[file_cnt]);

    binfile[file_cnt] = fopeno(binfilepath, "rb");
    if(binfile[file_cnt] == NULL)
    {
      sprintf(scratchpad, "Error, can not open binary file:\n%s", binfilepath);
      textEdit1->append(QString::fromLocal8Bit(scratchpad));
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    xml_go_up(xml_hdl);

    value_entry_file_cnt++;
  }

///////////////////////////////////////////////////////////////////////////////////////////////

  if(file_cnt < 1)
  {
    textEdit1->append("Error, can not find element \"signalEntry\" or element \"csvFileFormat\".\n");
    xml_close(xml_hdl);
    return;
  }

  if(total_edf_signals > (MAXSIGNALS - 1))
  {
    textEdit1->append("Error, too many signals (limit is 511).\n");
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    xml_close(xml_hdl);
    return;
  }

  sf_divider = 1;

  if(sf_less_1 == 0)
  {
    sf_divider = 10;

    for(i=0; i<file_cnt; i++)
    {
      if(sf[i] % 10)
      {
        sf_divider = 1;

        break;
      }
    }

    if(sf_divider == 1)
    {
      sf_divider = 8;

      for(i=0; i<file_cnt; i++)
      {
        if(sf[i] % 8)
        {
          sf_divider = 1;

          break;
        }
      }
    }

    if(sf_divider == 1)
    {
      sf_divider = 5;

      for(i=0; i<file_cnt; i++)
      {
        if(sf[i] % 5)
        {
          sf_divider = 1;

          break;
        }
      }
    }

    if(sf_divider == 1)
    {
      sf_divider = 4;

      for(i=0; i<file_cnt; i++)
      {
        if(sf[i] % 4)
        {
          sf_divider = 1;

          break;
        }
      }
    }

    if(sf_divider == 1)
    {
      sf_divider = 2;

      for(i=0; i<file_cnt; i++)
      {
        if(sf[i] % 2)
        {
          sf_divider = 1;

          break;
        }
      }
    }
  }

  max_datablocks = 0;

  for(k=0; k<file_cnt; k++)
  {
    sf[k] /= sf_divider;

    if(csv_enc[k])
    {
      if(nedval_enc[k])
      {
        tmp = 0;

        while(fgets(linebuf, 32, binfile[k]) != NULL)
        {
          tmp = atoi(linebuf);
        }

        if(sf_inv[k])  // samplefrequency lower than 1 Hz
        {
          datablocks[k] = (tmp * sf_inv[k]) + 1;
        }
        else
        {
          datablocks[k] = (tmp / sf[k]) + 1;
        }
      }
      else
      {
        lines = 0;

        while(1)
        {
          tmp = fgetc(binfile[k]);

          if(tmp == EOF)  break;

          if(tmp == '\n')  lines++;
        }

        if(sf_inv[k])  // samplefrequency lower than 1 Hz
        {
          datablocks[k] = lines * sf_inv[k];
        }
        else
        {
          datablocks[k] = lines / sf[k];
        }
      }
    }
    else
    {
      fseeko(binfile[k], 0LL, SEEK_END);

      if(sf_inv[k])  // samplefrequency lower than 1 Hz
      {
        datablocks[k] = (ftello(binfile[k]) / (edf_signals[k] * samplesize[k])) * sf_inv[k];
      }
      else
      {
        datablocks[k] = ftello(binfile[k]) / (edf_signals[k] * samplesize[k] * sf[k]);
      }
    }

    fseeko(binfile[k], 0LL, SEEK_SET);

    if(datablocks[k] > max_datablocks)
    {
      max_datablocks = datablocks[k];
    }
  }

////////////////////////////////////// Check for and count the triggers ////////////////////////////////////////////////////////////

  total_annotations = 0;

  xml_goto_root(xml_hdl);

  trig_file_cnt = 0;

  for(k=0; k<MAXFILES; k++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "eventEntry", k))
    {
      break;
    }

    if(get_signalparameters_from_EVT_attributes(xml_hdl, k))
    {
      xml_close(xml_hdl);
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      return;
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "csvFileFormat", 0))
    {
      if(xml_get_attribute_of_element(xml_hdl, "separator", str, 255) < 0)
      {
        textEdit1->append("Error, can not find attribute \"separator\".\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      if(strlen(str) != 1)
      {
        textEdit1->append("Error, value for \"separator\" must be one character.\n");
        xml_close(xml_hdl);
        for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
        return;
      }

      csv_sep[k] = str[0];

      if(xml_get_attribute_of_element(xml_hdl, "decimalSeparator", str, 255) < 0)
      {
        csv_dec_sep[k] = '.';  // if attribute decimalSeparator is not present, fall back to a dot
      }
      else
      {
        if(strlen(str) != 1)
        {
          textEdit1->append("Error, value for \"decimalSeparator\" must be one character.\n");
          xml_close(xml_hdl);
          for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
          return;
        }

        csv_dec_sep[k] = str[0];

        if(csv_sep[k] == csv_dec_sep[k])
        {
          textEdit1->append("Error, attribute \"decimalSeparator\" and \"separator\" have equal values.\n");
          xml_close(xml_hdl);
          for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
          return;
        }

        xml_go_up(xml_hdl);
      }

      trig_file_cnt++;
    }

    xml_go_up(xml_hdl);
  }

  if(count_events_from_csv_files(trig_file_cnt, path, &total_annotations))
  {
    xml_close(xml_hdl);
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    return;
  }

////////////////////// Initialize EDF-file ////////////////////////////////////////////////////////

  outputfilename[0] = 0;
  if(recent_savedir[0]!=0)
  {
    strcpy(outputfilename, recent_savedir);
    strcat(outputfilename, "/");
  }
  strcat(outputfilename, str_measurementId);
  remove_extension_from_filename(outputfilename);
  if(bdf == 1)
  {
    strcat(outputfilename, ".bdf");

    strcpy(outputfilename, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(outputfilename), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());

    if(!strcmp(outputfilename, ""))
    {
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      xml_close(xml_hdl);
      return;
    }

    get_directory_from_path(recent_savedir, outputfilename, MAX_PATH_LENGTH);

    hdl = edfopen_file_writeonly(outputfilename, EDFLIB_FILETYPE_BDFPLUS, total_edf_signals);
  }
  else
  {
    strcat(outputfilename, ".edf");

    strcpy(outputfilename, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(outputfilename), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

    if(!strcmp(outputfilename, ""))
    {
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      xml_close(xml_hdl);
      return;
    }

    get_directory_from_path(recent_savedir, outputfilename, MAX_PATH_LENGTH);

    hdl = edfopen_file_writeonly(outputfilename, EDFLIB_FILETYPE_EDFPLUS, total_edf_signals);
  }

  if(hdl<0)
  {
    snprintf(scratchpad, 2048, "Error, can not open file %s for writing.\n", outputfilename);
    textEdit1->append(QString::fromLocal8Bit(scratchpad));
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    xml_close(xml_hdl);
    return;
  }

  err = 0;

  for(k=0; k<file_cnt; k++)
  {
    for(i=0; i<edf_signals[k]; i++)
    {
      j = edf_signal_start[k] + i;

      if(edf_set_samplefrequency(hdl, j, sf[k]))  err = 1;

      if(edf_set_digital_maximum(hdl, j, digmax[k]))  err = 1;

      if(edf_set_digital_minimum(hdl, j, digmin[k]))  err = 1;

      if(edf_set_physical_maximum(hdl, j, physmax[k]))  err = 1;

      if(edf_set_physical_minimum(hdl, j, physmin[k]))  err = 1;

      if(edf_set_physical_dimension(hdl, j, physdim[k]))  err = 1;

      if(edf_set_label(hdl, j, signallabel[i]))  err = 1;
    }
  }

  for(i=0; i<total_edf_signals; i++)
  {
    if(edf_set_label(hdl, i, signallabel[i]))  err = 1;
  }

  if(edf_set_startdatetime(hdl, atoi(str_timestampStart), atoi(str_timestampStart + 5),
                          atoi(str_timestampStart + 8), atoi(str_timestampStart + 11),
                          atoi(str_timestampStart + 14), atoi(str_timestampStart + 17)))  err = 1;

  if(edf_set_datarecord_duration(hdl, 100000 / sf_divider))  err = 1;

  if(total_annotations > max_datablocks)
  {
    tmp = (total_annotations / max_datablocks) + 1;

    if(tmp > 64)  tmp = 64;

    if(edf_set_number_of_annotation_signals(hdl, tmp))  err = 1;
  }

  if(err)
  {
    textEdit1->append("An error occurred during initializing the EDF-header.\n");
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    xml_close(xml_hdl);
    edfclose_file(hdl);
    return;
  }

////////////////////// Prepare data-conversion ////////////////////////////////////////////////////

  bufsize = 0;

  for(k=0; k<file_cnt; k++)   // bufsize is expressed in bytes
  {
    buf1_offset[k] = bufsize;

    buf1_freadsize[k] = (edf_signals[k] * samplesize[k] * sf[k]);

    bufsize += buf1_freadsize[k];
  }

  buf1 = (char *)malloc(bufsize);
  if(buf1 == NULL)
  {
    textEdit1->append("Malloc error (buf1)\n");
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    xml_close(xml_hdl);
    edfclose_file(hdl);
    return;
  }

  j = 0;

  for(k=0; k<file_cnt; k++)  // calculate the total number of samples in one datarecord
  {
    buf2_offset[k] = j;

    j += (edf_signals[k] * sf[k]);
  }

  buf2 = (int *)malloc(j * sizeof(int));
  if(buf2 == NULL)
  {
    textEdit1->append("Malloc error (buf2)\n");
    for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
    xml_close(xml_hdl);
    edfclose_file(hdl);
    free(buf1);
    return;
  }

////////////////////// Start data-conversion //////////////////////////////////////////////////////

  QProgressDialog progress("Converting binary file(s)...", "Cancel", 0, max_datablocks, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = max_datablocks / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  int sf_t,
      signals_t,
      *buf2_t;

  char *buf1_t,
       *ptr,
       dec_sep,
       sep;

  long long adcz;

  unsigned long long ul_tmp;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
          float flp;
        } var;

  union {
          unsigned long long lone;
          signed long long lone_signed;
          double dflp;
        } var2;

// for(i=0; i<file_cnt; i++)
// {
//   printf("\nfile: %i\n"
//          "sf = %i\n"
//          "sf_inv = %i\n"
//          "datablocks = %i\n"
//          "csv_enc = %i\n"
//          "nedval_enc = %i\n"
//          "adczero = %lli\n"
//          "baseline = %lli\n"
//          "lsbvalue = %.20f\n",
//          i,
//          sf[i],
//          sf_inv[i],
//          datablocks[i],
//          csv_enc[i],
//          nedval_enc[i],
//          adczero[i],
//          baseline[i],
//          lsbval[i]);
// }

  for(k=0; k<file_cnt; k++)
  {
    if(csv_enc[k] && nedval_enc[k])
    {
      sf_t = sf[k];

      signals_t = edf_signals[k];

      dec_sep = csv_dec_sep[k];

      sep = csv_sep[k];

      if(fgets(linebuf, 512, binfile[k]) != NULL)
      {
        if(dec_sep != '.')
        {
          for(ptr=linebuf; *ptr!=0; ptr++)
          {
            if(*ptr == dec_sep)  *ptr = '.';
          }
        }

        nedval_smpl[k] = atoi(linebuf);

        ptr=linebuf;

        for(i=0; i<signals_t; i++)
        {
          for(; *ptr!=0; ptr++)
          {
            if(*ptr == sep)
            {
              ptr++;

              break;
            }
          }

          nedval_value[k][i] = nearbyint(atof(ptr));

          nedval_value2[k][i] = 0;
        }
      }
    }
  }

  for(blocks_written=0; blocks_written<max_datablocks; blocks_written++)
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

    for(k=0; k<file_cnt; k++)
    {
      sf_t = sf[k];

      signals_t = edf_signals[k];

      buf1_t = buf1 + buf1_offset[k];

      buf2_t = buf2 + buf2_offset[k];

      dec_sep = csv_dec_sep[k];

      sep = csv_sep[k];

      adcz = adczero[k];

      if(blocks_written < datablocks[k])
      {
        if((sf_inv[k] == 0) || (!(blocks_written % sf_inv[k])))
        {
          if(csv_enc[k])
          {
            if(nedval_enc[k])
            {
              for(i=0; i<sf_t; i++)
              {
                if(((blocks_written * sf_t) + i) == nedval_smpl[k])
                {
                  if(fgets(linebuf, 512, binfile[k]) != NULL)
                  {
                    if(dec_sep != '.')
                    {
                      for(ptr=linebuf; *ptr!=0; ptr++)
                      {
                        if(*ptr == dec_sep)  *ptr = '.';
                      }
                    }

                    nedval_smpl[k] = atoi(linebuf);

                    ptr=linebuf;

                    for(j=0; j<signals_t; j++)
                    {
                      for(; *ptr!=0; ptr++)
                      {
                        if(*ptr == sep)
                        {
                          ptr++;

                          break;
                        }
                      }

                      nedval_value2[k][j] = nedval_value[k][j];

                      nedval_value[k][j] = nearbyint(atof(ptr));
                    }
                  }
                  else
                  {
                    for(j=0; j<signals_t; j++)
                    {
                      nedval_value2[k][j] = nedval_value[k][j];

                      nedval_smpl[k] = 0;
                    }
                  }
                }

                for(j=0; j<signals_t; j++)
                {
                  buf2_t[(j * sf_t) + i] = nedval_value2[k][j] - adcz;
                }
              }
            }
            else
            {
              if(fgets(linebuf, 512, binfile[k]) == NULL)
              {
                progress.reset();
                textEdit1->append("A read error occurred during the conversion.\n");
                for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
                xml_close(xml_hdl);
                edfclose_file(hdl);
                free(buf1);
                free(buf2);
                return;
              }

              if(dec_sep != '.')
              {
                for(ptr=linebuf; *ptr!=0; ptr++)
                {
                  if(*ptr == dec_sep)  *ptr = '.';
                }
              }

              ptr = linebuf;

              for(j=0; j<signals_t; j++)
              {
                if((*ptr == 0) || (*ptr == '\n'))  break;

                buf2_t[j] = nearbyint(atof(ptr)) - adcz;

                while(*(++ptr) != 0)
                {
                  if(*ptr == '\n')  break;

                  if(*ptr == sep)
                  {
                    ptr++;

                    break;
                  }
                }
              }
            }
          }
          else
          {
            n = fread(buf1_t, buf1_freadsize[k], 1, binfile[k]);
            if(n != 1)
            {
              progress.reset();
              textEdit1->append("A read error occurred during the conversion.\n");
              for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
              xml_close(xml_hdl);
              edfclose_file(hdl);
              free(buf1);
              free(buf2);
              return;
            }

            if(datatype[k] == US_DATATYPE_INT16_LI)
            {
              for(i=0; i<sf_t; i++)
              {
                for(j=0; j<signals_t; j++)
                {
                  buf2_t[(j * sf_t) + i] = *(((signed short *)buf1_t) + (i * signals_t) + j) - adcz;
                }
              }
            }
            else if(datatype[k] == US_DATATYPE_UINT16_LI)
              {
                for(i=0; i<sf_t; i++)
                {
                  for(j=0; j<signals_t; j++)
                  {
                    buf2_t[(j * sf_t) + i] = *(((unsigned short *)buf1_t) + (i * signals_t) + j) - adcz;
                  }
                }
              }
              else if(datatype[k] == US_DATATYPE_INT24_LI)
                {
                  for(i=0; i<sf_t; i++)
                  {
                    for(j=0; j<signals_t; j++)
                    {
                      var.four[0] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3));
                      var.four[1] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 1);
                      var.four[2] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 2);

                      if(var.four[2] & 0x80)
                      {
                        var.four[3] = 0xff;
                      }
                      else
                      {
                        var.four[3] = 0x00;
                      }

                      buf2_t[(j * sf_t) + i] = var.one_signed - adcz;
                    }
                  }
                }
                else if(datatype[k] == US_DATATYPE_UINT24_LI)
                  {
                    for(i=0; i<sf_t; i++)
                    {
                      for(j=0; j<signals_t; j++)
                      {
                        var.four[0] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3));
                        var.four[1] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 1);
                        var.four[2] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 2);
                        var.four[3] = 0;

                        buf2_t[(j * sf_t) + i] = var.one - adcz;
                      }
                    }
                  }
                  else if((datatype[k] == US_DATATYPE_INT8_LI) || (datatype[k] == US_DATATYPE_INT8_BI))
                    {
                      for(i=0; i<sf_t; i++)
                      {
                        for(j=0; j<signals_t; j++)
                        {
                          buf2_t[(j * sf_t) + i] = *(((signed char *)buf1_t) + (i * signals_t) + j) - adcz;
                        }
                      }
                    }
                    else if((datatype[k] == US_DATATYPE_UINT8_LI) || (datatype[k] == US_DATATYPE_UINT8_BI))
                      {
                        for(i=0; i<sf_t; i++)
                        {
                          for(j=0; j<signals_t; j++)
                          {
                            tmp2 = *(((unsigned char *)buf1_t) + (i * signals_t) + j) - adcz;

                            buf2_t[(j * sf_t) + i] = tmp2;
                          }
                        }
                      }
                      else if(datatype[k] == US_DATATYPE_INT16_BI)
                        {
                          for(i=0; i<sf_t; i++)
                          {
                            for(j=0; j<signals_t; j++)
                            {
                              tmp3 = *(((signed short *)buf1_t) + (i * signals_t) + j);

                              buf2_t[(j * sf_t) + i] = ((signed short)((((unsigned short)tmp3 & 0xFF00) >> 8) |
                                                                       (((unsigned short)tmp3 & 0x00FF) << 8))
                                                       ) - adcz;
                            }
                          }
                        }
                        else if(datatype[k] == US_DATATYPE_UINT16_BI)
                          {
                            for(i=0; i<sf_t; i++)
                            {
                              for(j=0; j<signals_t; j++)
                              {
                                tmp3 = *(((signed short *)buf1_t) + (i * signals_t) + j);

                                buf2_t[(j * sf_t) + i] = ((unsigned short)((((unsigned short)tmp3 & 0xFF00) >> 8) |
                                                                           (((unsigned short)tmp3 & 0x00FF) << 8))
                                                         )  - adcz;
                              }
                            }
                          }
                          else if(datatype[k] == US_DATATYPE_INT24_BI)
                            {
                              for(i=0; i<sf_t; i++)
                              {
                                for(j=0; j<signals_t; j++)
                                {
                                  var.four[2] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3));
                                  var.four[1] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 1);
                                  var.four[0] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 2);

                                  if(var.four[2] & 0x80)
                                  {
                                    var.four[3] = 0xff;
                                  }
                                  else
                                  {
                                    var.four[3] = 0x00;
                                  }

                                  buf2_t[(j * sf_t) + i] = var.one_signed - adcz;
                                }
                              }
                            }
                            else if(datatype[k] == US_DATATYPE_UINT24_BI)
                              {
                                for(i=0; i<sf_t; i++)
                                {
                                  for(j=0; j<signals_t; j++)
                                  {
                                    var.four[3] = 0;
                                    var.four[2] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3));
                                    var.four[1] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 1);
                                    var.four[0] = *((unsigned char *)buf1_t + (((i * signals_t) + j) * 3) + 2);

                                    buf2_t[(j * sf_t) + i] = var.one - adcz;
                                  }
                                }
                              }
                              else if(datatype[k] == US_DATATYPE_INT32_LI)
                                {
                                  for(i=0; i<sf_t; i++)
                                  {
                                    for(j=0; j<signals_t; j++)
                                    {
                                      buf2_t[(j * sf_t) + i] = *(((signed int *)buf1_t) + (i * signals_t) + j) - adcz;
                                    }
                                  }
                                }
                                else if(datatype[k] == US_DATATYPE_UINT32_LI)
                                  {
                                    for(i=0; i<sf_t; i++)
                                    {
                                      for(j=0; j<signals_t; j++)
                                      {
                                        buf2_t[(j * sf_t) + i] = *(((unsigned int *)buf1_t) + (i * signals_t) + j) - adcz;
                                      }
                                    }
                                  }
                                  else if(datatype[k] == US_DATATYPE_INT32_BI)
                                    {
                                      for(i=0; i<sf_t; i++)
                                      {
                                        for(j=0; j<signals_t; j++)
                                        {
                                            tmp = *(((signed int *)buf1_t) + (i * signals_t) + j);

                                            buf2_t[(j * sf_t) + i] = ((signed int)((((unsigned int)tmp & 0xFF000000) >> 24) |
                                                                                   (((unsigned int)tmp & 0x00FF0000) >>  8) |
                                                                                   (((unsigned int)tmp & 0x0000FF00) <<  8) |
                                                                                   (((unsigned int)tmp & 0x000000FF) << 24))
                                                                     ) - adcz;
                                        }
                                      }
                                    }
                                    else if(datatype[k] == US_DATATYPE_UINT32_BI)
                                      {
                                        for(i=0; i<sf_t; i++)
                                        {
                                          for(j=0; j<signals_t; j++)
                                          {
                                            tmp = *(((signed int *)buf1_t) + (i * signals_t) + j);

                                            buf2_t[(j * sf_t) + i] = ((unsigned int)((((unsigned int)tmp & 0xFF000000) >> 24) |
                                                                                     (((unsigned int)tmp & 0x00FF0000) >>  8) |
                                                                                     (((unsigned int)tmp & 0x0000FF00) <<  8) |
                                                                                     (((unsigned int)tmp & 0x000000FF) << 24))
                                                                     ) - adcz;
                                          }
                                        }
                                      }
                                      else if(datatype[k] == US_DATATYPE_FLOAT_LI)
                                        {
                                          for(i=0; i<sf_t; i++)
                                          {
                                            for(j=0; j<signals_t; j++)
                                            {
                                              buf2_t[(j * sf_t) + i] = (*(((float *)buf1_t) + (i * signals_t) + j) - adcz);
                                            }
                                          }
                                        }
                                        else if(datatype[k] == US_DATATYPE_FLOAT_BI)
                                          {
                                            for(i=0; i<sf_t; i++)
                                            {
                                              for(j=0; j<signals_t; j++)
                                              {
                                                tmp = *(((signed int *)buf1_t) + (i * signals_t) + j);

                                                var.one = ((((unsigned int)tmp & 0xFF000000) >> 24) |
                                                           (((unsigned int)tmp & 0x00FF0000) >>  8) |
                                                           (((unsigned int)tmp & 0x0000FF00) <<  8) |
                                                           (((unsigned int)tmp & 0x000000FF) << 24));

                                                buf2_t[(j * sf_t) + i] = var.flp - adcz;
                                              }
                                            }
                                          }
                                          else if(datatype[k] == US_DATATYPE_DOUBLE_LI)
                                            {
                                              for(i=0; i<sf_t; i++)
                                              {
                                                for(j=0; j<signals_t; j++)
                                                {
                                                  buf2_t[(j * sf_t) + i] = (*(((double *)buf1_t) + (i * signals_t) + j) - adcz);
                                                }
                                              }
                                            }
                                            else if(datatype[k] == US_DATATYPE_DOUBLE_BI)
                                              {
                                                for(i=0; i<sf_t; i++)
                                                {
                                                  for(j=0; j<signals_t; j++)
                                                  {
                                                    ul_tmp = *(((unsigned long long *)buf1_t) + (i * signals_t) + j);

                                                    var2.lone = (((ul_tmp & 0xFF00000000000000) >> 56) |
                                                                 ((ul_tmp & 0x00FF000000000000) >> 40) |
                                                                 ((ul_tmp & 0x0000FF0000000000) >> 24) |
                                                                 ((ul_tmp & 0x000000FF00000000) >>  8) |
                                                                 ((ul_tmp & 0x00000000FF000000) <<  8) |
                                                                 ((ul_tmp & 0x0000000000FF0000) << 24) |
                                                                 ((ul_tmp & 0x000000000000FF00) << 40) |
                                                                 ((ul_tmp & 0x00000000000000FF) << 56));

                                                    buf2_t[(j * sf_t) + i] = var2.dflp - adcz;
                                                  }
                                                }
                                              }
                                              else if(datatype[k] == US_DATATYPE_INT64_LI)
                                                {
                                                  for(i=0; i<sf_t; i++)
                                                  {
                                                    for(j=0; j<signals_t; j++)
                                                    {
                                                      buf2_t[(j * sf_t) + i] = *(((signed long long *)buf1_t) + (i * signals_t) + j) - adcz;
                                                    }
                                                  }
                                                }
                                                else if(datatype[k] == US_DATATYPE_UINT64_LI)
                                                  {
                                                    for(i=0; i<sf_t; i++)
                                                    {
                                                      for(j=0; j<signals_t; j++)
                                                      {
                                                        buf2_t[(j * sf_t) + i] = *(((unsigned long long *)buf1_t) + (i * signals_t) + j) - adcz;
                                                      }
                                                    }
                                                  }
                                                  else if(datatype[k] == US_DATATYPE_INT64_BI)
                                                    {
                                                      for(i=0; i<sf_t; i++)
                                                      {
                                                        for(j=0; j<signals_t; j++)
                                                        {
                                                          ul_tmp = *(((unsigned long long *)buf1_t) + (i * signals_t) + j);

                                                          var2.lone = ((signed long long)(
                                                            ((ul_tmp & 0xFF00000000000000) >> 56) |
                                                            ((ul_tmp & 0x00FF000000000000) >> 40) |
                                                            ((ul_tmp & 0x0000FF0000000000) >> 24) |
                                                            ((ul_tmp & 0x000000FF00000000) >>  8) |
                                                            ((ul_tmp & 0x00000000FF000000) <<  8) |
                                                            ((ul_tmp & 0x0000000000FF0000) << 24) |
                                                            ((ul_tmp & 0x000000000000FF00) << 40) |
                                                            ((ul_tmp & 0x00000000000000FF) << 56))
                                                            );

                                                          buf2_t[(j * sf_t) + i] = var2.lone_signed - adcz;
                                                        }
                                                      }
                                                    }
                                                    else if(datatype[k] == US_DATATYPE_UINT64_BI)
                                                      {
                                                        for(i=0; i<sf_t; i++)
                                                        {
                                                          for(j=0; j<signals_t; j++)
                                                          {
                                                            ul_tmp = *(((unsigned long long *)buf1_t) + (i * signals_t) + j);

                                                            var2.lone = ((unsigned long long)(
                                                              ((ul_tmp & 0xFF00000000000000) >> 56) |
                                                              ((ul_tmp & 0x00FF000000000000) >> 40) |
                                                              ((ul_tmp & 0x0000FF0000000000) >> 24) |
                                                              ((ul_tmp & 0x000000FF00000000) >>  8) |
                                                              ((ul_tmp & 0x00000000FF000000) <<  8) |
                                                              ((ul_tmp & 0x0000000000FF0000) << 24) |
                                                              ((ul_tmp & 0x000000000000FF00) << 40) |
                                                              ((ul_tmp & 0x00000000000000FF) << 56))
                                                              );

                                                            buf2_t[(j * sf_t) + i] = var2.lone - adcz;
                                                          }
                                                        }
                                                      }
          }
        }
      }
      else if(blocks_written == datablocks[k])
        {
          if(!nedval_enc[k])
          {
            for(i=0; i<sf_t; i++)
            {
              for(j=0; j<signals_t; j++)
              {
                buf2_t[(j * sf_t) + i] = 0;
              }
            }
          }
          else
          {
            for(i=0; i<sf_t; i++)
            {
              for(j=0; j<signals_t; j++)
              {
                buf2_t[(j * sf_t) + i] = nedval_value2[k][j] - adcz;
              }
            }
          }
        }
    }

    if(edf_blockwrite_digital_samples(hdl, buf2))
    {
      progress.reset();
      textEdit1->append("A write error occurred.\n");
      for(i=0; i<file_cnt; i++)  fclose(binfile[i]);
      xml_close(xml_hdl);
      edfclose_file(hdl);
      free(buf1);
      free(buf2);
      return;
    }
  }

  progress.reset();

  free(buf1);
  free(buf2);
  for(i=0; i<file_cnt; i++)  fclose(binfile[i]);

////////////////////////////////////// Write triggers ////////////////////////////////////////////////////////////

  if(get_events_from_csv_files(trig_file_cnt, hdl, path))
  {
    edfclose_file(hdl);

    xml_close(xml_hdl);

    return;
  }


  xml_close(xml_hdl);

  edfclose_file(hdl);

  textEdit1->append("Done\n");
}


int UI_UNISENS2EDFwindow::get_events_from_csv_files(int max_files, int edf_hdl, const char *path)
{
  int i, k,
      len,
      len2,
      progress_steps,
      triggers_written=0;

  char scratchpad[2048],
       csvfilepath[MAX_PATH_LENGTH],
       linebuf[1024],
       *ptr,
       sep,
       annotation[64];

  long long onset;

  FILE *csvfile;


  QProgressDialog progress("Converting triggers ...", "Cancel", 0, total_annotations, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = total_annotations / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  for(k=0; k<max_files; k++)
  {
    get_directory_from_path(csvfilepath, path, MAX_PATH_LENGTH);

    strcat(csvfilepath, "/");
    strcat(csvfilepath, evtfilename[k]);

    csvfile = fopeno(csvfilepath, "rb");
    if(csvfile == NULL)
    {
      progress.reset();
      sprintf(scratchpad, "Error, can not open csv file:\n%s\n", evtfilename[k]);
      textEdit1->append(QString::fromLocal8Bit(scratchpad));
      return 1;
    }

    sep = csv_sep[k];

    while(1)
    {
      if(!(triggers_written % progress_steps))
      {
        progress.setValue(triggers_written);

        qApp->processEvents();

        if(progress.wasCanceled() == true)
        {
          fclose(csvfile);

          return 1;
        }
      }

      ptr = fgets(linebuf, 1024, csvfile);

      if(ptr == NULL)  break;

      len = strlen(linebuf);

      if(len < 3)  continue;

      for(i=0; i<len; i++)
      {
        if((*ptr == '\n') || (*ptr == '\r') || (*ptr == 0))
        {
          break;
        }

        if(*ptr == sep)
        {
          if((i < (len - 2)) && (i > 0))
          {
            *ptr = 0;

            onset = atoi(linebuf);

            strncpy(annotation, ++ptr, 46);

            annotation[45] = 0;

            len2 = strlen(annotation);

            if(annotation[len2 - 1] == '\n')
            {
              annotation[--len2] = 0;
            }

            if(annotation[len2 - 1] == '\r')
            {
              annotation[--len2] = 0;
            }

            if(annotation[len2 - 1] == sep)
            {
              annotation[--len2] = 0;
            }

            remove_leading_spaces(annotation);

            remove_trailing_spaces(annotation);

            onset *= 10000LL;

            onset /= evt_sf[k];

            if(char_encoding == 1)
            {
              if(edfwrite_annotation_latin1(edf_hdl, onset, -1LL, annotation))
              {
                progress.reset();
                textEdit1->append("An error occurred while writing the events to the EDF-file.\n");
                return 1;
              }
            }

            if(char_encoding == 2)
            {
              if(edfwrite_annotation_utf8(edf_hdl, onset, -1LL, annotation))
              {
                progress.reset();
                textEdit1->append("An error occurred while writing the events to the EDF-file.\n");
                return 1;
              }
            }

            triggers_written++;

            break;
          }
          else
          {
            break;
          }
        }

        ptr++;
      }
    }

    fclose(csvfile);
  }

  progress.reset();

  return 0;
}


int UI_UNISENS2EDFwindow::count_events_from_csv_files(int max_files, const char *path, int *result)
{
  int evt_cnt=0, k;

  char scratchpad[2048],
       csvfilepath[MAX_PATH_LENGTH],
       linebuf[1024],
       *ptr;

  FILE *csvfile;


  *result = 0;

  for(k=0; k<max_files; k++)
  {
    get_directory_from_path(csvfilepath, path, MAX_PATH_LENGTH);

    strcat(csvfilepath, "/");
    strcat(csvfilepath, evtfilename[k]);

    csvfile = fopeno(csvfilepath, "rb");
    if(csvfile == NULL)
    {
      sprintf(scratchpad, "Error, can not open csv file:\n%s\n", evtfilename[k]);
      textEdit1->append(QString::fromLocal8Bit(scratchpad));
      return 1;
    }

    while(1)
    {
      ptr = fgets(linebuf, 1024, csvfile);

      if(ptr == NULL)  break;

      evt_cnt++;
    }

    fclose(csvfile);
  }

  *result = evt_cnt;

  return 0;
}


int UI_UNISENS2EDFwindow::get_signalparameters_from_BIN_attributes(struct xml_handle *xml_hdl, int file_nr)
{
  char str[256],
       scratchpad[2048];

  double d_tmp;


  if(xml_get_attribute_of_element(xml_hdl, "id", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"id\".\n");
    return 1;
  }

  if(strlen(str) < 1)
  {
    textEdit1->append("Error, attribute \"id\" has no value.\n");
    return 1;
  }

  strncpy(binfilename[file_nr], str, MAX_PATH_LENGTH - 1);

  if(xml_get_attribute_of_element(xml_hdl, "adcResolution", str, 255) < 0)
  {
    adcres[file_nr] = 0;
  }
  else if(strlen(str) < 1)
    {
      adcres[file_nr] = 0;
    }
    else adcres[file_nr] = atoi(str);

  if(xml_get_attribute_of_element(xml_hdl, "unit", str, 255) < 0)
  {
    strcpy(physdim[file_nr], "X");
  }
  else if(strlen(str) < 1)
    {
      strcpy(physdim[file_nr], "X");
    }
    else if(char_encoding == 1)  // Latin-1
      {
        strncpy(physdim[file_nr], str, 8);
      }
      else if(char_encoding == 2)
        {
          strncpy(physdim[file_nr], QString::fromUtf8(str).toLatin1().data(), 8);
        }
        else
        {
//          strncpy(physdim[file_nr], str, 8);
          strncpy(physdim[file_nr], QString::fromUtf8(str).toLatin1().data(), 8);  // default for XML is UTF-8
        }

  physdim[file_nr][8] = 0;

  remove_leading_spaces(physdim[file_nr]);

  remove_trailing_spaces(physdim[file_nr]);

  if(xml_get_attribute_of_element(xml_hdl, "sampleRate", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"sampleRate\".\n");
    return 1;
  }

  if(strlen(str) < 1)
  {
    textEdit1->append("Error, attribute \"sampleRate\" has no value.");
    return 1;
  }

  sf_inv[file_nr] = 0;

  sf[file_nr] = nearbyint(atof(str));

  if((sf[file_nr] < 1) || (sf[file_nr] > 1000000))
  {
    if(sf[file_nr] < 1)
    {
      d_tmp = atof(str);

      if(d_tmp >= 0.001)
      {
        sf_inv[file_nr] = nearbyint(1.0 / d_tmp);

        sf[file_nr] = 1;

        sf_less_1 = 1;
      }
      else
      {
        return US_SAMPLERATE_OUT_OF_RANGE;
      }
    }
    else
    {
      return US_SAMPLERATE_OUT_OF_RANGE;
    }
  }

  if(xml_get_attribute_of_element(xml_hdl, "baseline", str, 255) < 0)
  {
    baseline[file_nr] = 0LL;
  }
  else
  {
    if(strlen(str) < 1)
    {
      textEdit1->append("Error, attribute \"baseline\" has no value.\n");
      return 1;
    }

    baseline[file_nr] = strtoll(str, NULL, 0);
  }

  if(xml_get_attribute_of_element(xml_hdl, "adcZero", str, 255) < 0)
  {
    adczero[file_nr] = 0LL;
  }
  else
  {
    if(strlen(str) < 1)
    {
      textEdit1->append("Error, attribute \"adcZero\" has no value.\n");
      return 1;
    }

    adczero[file_nr] = strtoll(str, NULL, 0);
  }

  if(xml_get_attribute_of_element(xml_hdl, "lsbValue", str, 255) < 0)
  {
    lsbval[file_nr] = 1.0;
  }
  else if(strlen(str) < 1)
    {
      lsbval[file_nr] = 1.0;
    }
    else lsbval[file_nr] = atof(str);

  if((lsbval[file_nr] < -1000000.0) || (lsbval[file_nr] > 1000000.0))
  {
    textEdit1->append("Error, attribute \"lsbValue\" is out of range.\n");
    return 1;
  }

  physmax[file_nr] = lsbval[file_nr];

  physmin[file_nr] = lsbval[file_nr];

  if(xml_get_attribute_of_element(xml_hdl, "dataType", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"dataType\".\n");
    return 1;
  }

  if(csv_enc[file_nr])
  {
    if(!strcmp(str, "double"))
    {
      datatype[file_nr] = US_DATATYPE_DOUBLE_LI;
      bdf = 1;
      straightbinary[file_nr] = 0;
      samplesize[file_nr] = 8;
      physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
      physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
      digmax[file_nr] = 8388607;
      digmin[file_nr] = -8388608;
    }
    else if(!strcmp(str, "float"))
      {
        datatype[file_nr] = US_DATATYPE_FLOAT_LI;
        bdf = 1;
        straightbinary[file_nr] = 0;
        samplesize[file_nr] = 4;
        physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
        physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
        digmax[file_nr] = 8388607;
        digmin[file_nr] = -8388608;
      }
      else if(!strcmp(str, "int32"))
        {
          datatype[file_nr] = US_DATATYPE_INT32_LI;
          bdf = 1;
          straightbinary[file_nr] = 0;
          samplesize[file_nr] = 4;
          physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
          physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
          digmax[file_nr] = 8388607;
          digmin[file_nr] = -8388608;
        }
        else if(!strcmp(str, "uint32"))
          {
            datatype[file_nr] = US_DATATYPE_UINT32_LI;
            bdf = 1;
            straightbinary[file_nr] = 1;
            samplesize[file_nr] = 4;
            physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
            physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
            digmax[file_nr] = 8388607;
            digmin[file_nr] = -8388608;
          }
          else if(!strcmp(str, "int16"))
            {
              datatype[file_nr] = US_DATATYPE_INT16_LI;
              straightbinary[file_nr] = 0;
              samplesize[file_nr] = 2;
              physmax[file_nr] *= (32767LL - (baseline[file_nr] - adczero[file_nr]));
              physmin[file_nr] *= (-32768LL - (baseline[file_nr] - adczero[file_nr]));
              digmax[file_nr] = 32767;
              digmin[file_nr] = -32768;
            }
            else if(!strcmp(str, "uint16"))
              {
                datatype[file_nr] = US_DATATYPE_UINT16_LI;
                straightbinary[file_nr] = 1;
                samplesize[file_nr] = 2;
                physmax[file_nr] *= (32767LL - (baseline[file_nr] - adczero[file_nr]));
                physmin[file_nr] *= (-32768LL - (baseline[file_nr] - adczero[file_nr]));
                digmax[file_nr] = 32767;
                digmin[file_nr] = -32768;
              }
              else if(!strcmp(str, "int8"))
                {
                  datatype[file_nr] = US_DATATYPE_INT8_LI;
                  straightbinary[file_nr] = 0;
                  samplesize[file_nr] = 1;
                  physmax[file_nr] *= (127 - (baseline[file_nr] - adczero[file_nr]));
                  physmin[file_nr] *= (-128 - (baseline[file_nr] - adczero[file_nr]));
                  digmax[file_nr] = 127;
                  digmin[file_nr] = -128;
                }
                else if(!strcmp(str, "uint8"))
                  {
                    datatype[file_nr] = US_DATATYPE_UINT8_LI;
                    straightbinary[file_nr] = 1;
                    samplesize[file_nr] = 1;
                    physmax[file_nr] *= (127 - (baseline[file_nr] - adczero[file_nr]));
                    physmin[file_nr] *= (-128 - (baseline[file_nr] - adczero[file_nr]));
                    digmax[file_nr] = 127;
                    digmin[file_nr] = -128;
                  }
                  else if(!strcmp(str, "int64"))
                    {
                      datatype[file_nr] = US_DATATYPE_INT64_LI;
                      bdf = 1;
                      straightbinary[file_nr] = 0;
                      samplesize[file_nr] = 8;
                      physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                      physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                      digmax[file_nr] = 8388607;
                      digmin[file_nr] = -8388608;
                    }
                    else if(!strcmp(str, "uint64"))
                      {
                        datatype[file_nr] = US_DATATYPE_UINT64_LI;
                        bdf = 1;
                        straightbinary[file_nr] = 1;
                        samplesize[file_nr] = 8;
                        physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                        physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                        digmax[file_nr] = 8388607;
                        digmin[file_nr] = -8388608;
                      }
                      else
                      {
                        snprintf(scratchpad, 2047, "Error, unsupported combination of datatype: %s and csv file\n", str);
                        textEdit1->append(scratchpad);
                        return 1;
                      }
  }
  else
  {
    if(!strcmp(str, "uint8"))
    {
      datatype[file_nr] = US_DATATYPE_UINT8_LI;
      straightbinary[file_nr] = 1;
      samplesize[file_nr] = 1;
      physmax[file_nr] *= (127 - (baseline[file_nr] - adczero[file_nr]));
      physmin[file_nr] *= (-128 - (baseline[file_nr] - adczero[file_nr]));
      digmax[file_nr] = 127;
      digmin[file_nr] = -128;
    }
    else if(!strcmp(str, "int8"))
      {
        datatype[file_nr] = US_DATATYPE_INT8_LI;
        straightbinary[file_nr] = 0;
        samplesize[file_nr] = 1;
        physmax[file_nr] *= (127 - (baseline[file_nr] - adczero[file_nr]));
        physmin[file_nr] *= (-128 - (baseline[file_nr] - adczero[file_nr]));
        digmax[file_nr] = 127;
        digmin[file_nr] = -128;
      }
      else if(!strcmp(str, "uint16"))
        {
          datatype[file_nr] = US_DATATYPE_UINT16_LI;
          straightbinary[file_nr] = 1;
          samplesize[file_nr] = 2;
          physmax[file_nr] *= (32767LL - (baseline[file_nr] - adczero[file_nr]));
          physmin[file_nr] *= (-32768LL - (baseline[file_nr] - adczero[file_nr]));
          digmax[file_nr] = 32767;
          digmin[file_nr] = -32768;
        }
        else if(!strcmp(str, "int16"))
          {
            datatype[file_nr] = US_DATATYPE_INT16_LI;
            straightbinary[file_nr] = 0;
            samplesize[file_nr] = 2;
            physmax[file_nr] *= (32767LL - (baseline[file_nr] - adczero[file_nr]));
            physmin[file_nr] *= (-32768LL - (baseline[file_nr] - adczero[file_nr]));
            digmax[file_nr] = 32767;
            digmin[file_nr] = -32768;
          }
          else if(!strcmp(str, "uint24"))
            {
              datatype[file_nr] = US_DATATYPE_UINT24_LI;
              bdf = 1;
              straightbinary[file_nr] = 1;
              samplesize[file_nr] = 3;
              physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
              physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
              digmax[file_nr] = 8388607;
              digmin[file_nr] = -8388608;
            }
            else if(!strcmp(str, "int24"))
              {
                datatype[file_nr] = US_DATATYPE_INT24_LI;
                bdf = 1;
                straightbinary[file_nr] = 0;
                samplesize[file_nr] = 3;
                physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                digmax[file_nr] = 8388607;
                digmin[file_nr] = -8388608;
              }
              else if(!strcmp(str, "uint32"))
                {
                  datatype[file_nr] = US_DATATYPE_UINT32_LI;
                  bdf = 1;
                  straightbinary[file_nr] = 1;
                  samplesize[file_nr] = 4;
                  physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                  physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                  digmax[file_nr] = 8388607;
                  digmin[file_nr] = -8388608;
                }
                else if(!strcmp(str, "int32"))
                  {
                    datatype[file_nr] = US_DATATYPE_INT32_LI;
                    bdf = 1;
                    straightbinary[file_nr] = 0;
                    samplesize[file_nr] = 4;
                    physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                    physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                    digmax[file_nr] = 8388607;
                    digmin[file_nr] = -8388608;
                  }
                  else if(!strcmp(str, "float"))
                    {
                      datatype[file_nr] = US_DATATYPE_FLOAT_LI;
                      bdf = 1;
                      straightbinary[file_nr] = 0;
                      samplesize[file_nr] = 4;
                      physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                      physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                      digmax[file_nr] = 8388607;
                      digmin[file_nr] = -8388608;
                    }
                    else if(!strcmp(str, "double"))
                      {
                        datatype[file_nr] = US_DATATYPE_DOUBLE_LI;
                        bdf = 1;
                        straightbinary[file_nr] = 0;
                        samplesize[file_nr] = 8;
                        physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                        physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                        digmax[file_nr] = 8388607;
                        digmin[file_nr] = -8388608;
                      }
                      else if(!strcmp(str, "uint64"))
                        {
                          datatype[file_nr] = US_DATATYPE_UINT64_LI;
                          bdf = 1;
                          straightbinary[file_nr] = 1;
                          samplesize[file_nr] = 8;
                          physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                          physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                          digmax[file_nr] = 8388607;
                          digmin[file_nr] = -8388608;
                        }
                        else if(!strcmp(str, "int64"))
                          {
                            datatype[file_nr] = US_DATATYPE_INT64_LI;
                            bdf = 1;
                            straightbinary[file_nr] = 0;
                            samplesize[file_nr] = 8;
                            physmax[file_nr] *= (8388607LL - (baseline[file_nr] - adczero[file_nr]));
                            physmin[file_nr] *= (-8388608LL - (baseline[file_nr] - adczero[file_nr]));
                            digmax[file_nr] = 8388607;
                            digmin[file_nr] = -8388608;
                          }
                          else
                          {
                            snprintf(scratchpad, 2047, "Error, unsupported combination of datatype: %s and binary file\n", str);
                            textEdit1->append(scratchpad);
                            return 1;
                          }
  }

  return 0;
}


int UI_UNISENS2EDFwindow::get_signalparameters_from_EVT_attributes(struct xml_handle *xml_hdl, int file_nr)
{
  char str[256];

  if(xml_get_attribute_of_element(xml_hdl, "id", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"id\".\n");
    return 1;
  }

  if(strlen(str) < 1)
  {
    textEdit1->append("Error, attribute \"id\" has no value.\n");
    return 1;
  }

  strncpy(evtfilename[file_nr], str, MAX_PATH_LENGTH - 1);

  if(xml_get_attribute_of_element(xml_hdl, "sampleRate", str, 255) < 0)
  {
    textEdit1->append("Error, can not find attribute \"sampleRate\".\n");
    return 1;
  }

  if(strlen(str) < 1)
  {
    textEdit1->append("Error, attribute \"sampleRate\" has no value.\n");
    return 1;
  }

  evt_sf[file_nr] = atoi(str);

  if((evt_sf[file_nr] < 1) || (evt_sf[file_nr] > 1000000))
  {
    textEdit1->append("Error, attribute \"sampleRate\" is out of range.\n");
    return 1;
  }

  return 0;
}












