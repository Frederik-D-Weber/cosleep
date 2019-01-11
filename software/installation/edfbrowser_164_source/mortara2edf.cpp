/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 Teunis van Beelen
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



#include "mortara2edf.h"




UI_MortaraEDFwindow::UI_MortaraEDFwindow(QWidget *w_parent, char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("Mortara ECG XML to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  textEdit1 = new QTextEdit(myobjectDialog);
  textEdit1->setGeometry(20, 20, 560, 380);
  textEdit1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  sprintf(txt_string, "Mortara ECG XML to EDF converter\n");
  textEdit1->append(txt_string);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 430, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 430, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}



void UI_MortaraEDFwindow::SelectFileButton()
{
  int i, j, err, len, buf_len, char_encoding, edf_hdl=-99, datrecs;

  char path[MAX_PATH_LENGTH],
       scratchpad[4096];

  struct xml_handle *xml_hdl;

  for(i=0; i<MORTARA_MAX_CHNS; i++)
  {
    chan_data_in[i] = NULL;
    chan_data_out[i] = NULL;
  }

///////////////////////////////////////// OPEN THE XML FILE ///////////////////////

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

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "ECG"))
  {
    textEdit1->append("Error, can not find root element \"ECG\"\n");
    goto OUT_EXIT;
  }

  for(i=0; i<(MORTARA_MAX_CHNS + 1); i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "CHANNEL", i))
    {
      break;
    }

    xml_go_up(xml_hdl);
  }

  chan_cnt = i;

  if(chan_cnt < 1)
  {
    textEdit1->append("Error, can not find element \"CHANNEL\"\n");
    goto OUT_EXIT;
  }

  if(chan_cnt > MORTARA_MAX_CHNS)
  {
    textEdit1->append("Error, too many channels\n");
    goto OUT_EXIT;
  }

////////////////////////////////// GET THE LEAD PARAMETERS ////////////////////////

  for(i=0; i<chan_cnt; i++)
  {
    err = xml_goto_nth_element_inside(xml_hdl, "CHANNEL", i);
    if(err)
    {
      sprintf(scratchpad, "Error, can not find element \"CHANNEL\" number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    len = xml_get_attribute_of_element(xml_hdl, "OFFSET", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"OFFSET\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      return;
    }
    chan_offset[i] = atoi(scratchpad);
    if(chan_offset[i] < 0)
    {
      sprintf(scratchpad, "Error, value of attribute \"OFFSET\" in channel number %i is %i\n", i + 1, chan_offset[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    if(i)
    {
      if(chan_offset[i] != chan_offset[i-1])
      {
        sprintf(scratchpad, "Error, value of attribute \"OFFSET\" in channel number %i is not equal to other channels\n", i + 1);
        textEdit1->append(scratchpad);
        goto OUT_EXIT;
      }
    }

    len = xml_get_attribute_of_element(xml_hdl, "BITS", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"BITS\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_bits[i] = atoi(scratchpad);
//     if((chan_bits[i] != 8) &&
//       (chan_bits[i] != 16) &&
//       (chan_bits[i] != 32))
    if(chan_bits[i] != 16)
    {
      sprintf(scratchpad, "Error, value of attribute \"BITS\" in channel number %i is %i\n", i + 1, chan_offset[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    if(i)
    {
      if(chan_bits[i] != chan_bits[i-1])
      {
        sprintf(scratchpad, "Error, value of attribute \"BITS\" in channel number %i is not equal to other channels\n", i + 1);
        textEdit1->append(scratchpad);
        goto OUT_EXIT;
      }
    }

    len = xml_get_attribute_of_element(xml_hdl, "FORMAT", chan_format[i], 17);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"FORMAT\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_format[i][16] = 0;
    if(strcmp(chan_format[i], "SIGNED"))
    {
      sprintf(scratchpad, "Error, value of attribute \"FORMAT\" in channel number %i is %s\n", i + 1, chan_format[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    len = xml_get_attribute_of_element(xml_hdl, "UNITS_PER_MV", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"UNITS_PER_MV\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_units_per_mv[i] = atoi(scratchpad);
    if(chan_units_per_mv[i] < 1)
    {
      sprintf(scratchpad, "Error, value of attribute \"UNITS_PER_MV\" in channel number %i is %i\n", i + 1, chan_units_per_mv[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    len = xml_get_attribute_of_element(xml_hdl, "DURATION", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"DURATION\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_duration[i] = atoi(scratchpad);
    if(chan_duration[i] < 1)
    {
      sprintf(scratchpad, "Error, value of attribute \"DURATION\" in channel number %i is %i\n", i + 1, chan_duration[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    if(i)
    {
      if(chan_duration[i] != chan_duration[i-1])
      {
        sprintf(scratchpad, "Error, value of attribute \"DURATION\" in channel number %i is not equal to other channels\n", i + 1);
        textEdit1->append(scratchpad);
        goto OUT_EXIT;
      }
    }

    len = xml_get_attribute_of_element(xml_hdl, "SAMPLE_FREQ", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"SAMPLE_FREQ\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_sample_freq[i] = atoi(scratchpad);
    if(chan_sample_freq[i] < 1)
    {
      sprintf(scratchpad, "Error, value of attribute \"SAMPLE_FREQ\" in channel number %i is %i\n", i + 1, chan_sample_freq[i]);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    if(i)
    {
      if(chan_sample_freq[i] != chan_sample_freq[i-1])
      {
        sprintf(scratchpad, "Error, value of attribute \"SAMPLE_FREQ\" in channel number %i is not equal to other channels\n", i + 1);
        textEdit1->append(scratchpad);
        goto OUT_EXIT;
      }
    }

    len = xml_get_attribute_of_element(xml_hdl, "NAME", chan_name[i], 17);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"NAME\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    chan_name[i][16] = 0;

    len = xml_get_attribute_of_element(xml_hdl, "ENCODING", scratchpad, 4096);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"ENCODING\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }
    scratchpad[32] = 0;
    if(strcmp(scratchpad, "BASE64"))
    {
      sprintf(scratchpad, "Error, value of attribute \"ENCODING\" in channel number %i is %s\n", i + 1, scratchpad);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    xml_go_up(xml_hdl);
  }

  chan_sf = chan_sample_freq[0];

  for(chan_sf_div=10; chan_sf_div>0; chan_sf_div--)
  {
    if(chan_sf_div == 9)  continue;
    if(chan_sf_div == 7)  continue;
    if(chan_sf_div == 6)  continue;
    if(chan_sf_div == 3)  continue;

    if(!(chan_sf % chan_sf_div))  break;
  }

  if(chan_sf_div < 1)  chan_sf_div = 1;

  chan_sf_block = chan_sf / chan_sf_div;

////////////////////////////// GET THE LEAD DATA ////////////////////////////////////

  for(i=0; i<chan_cnt; i++)
  {
    err = xml_goto_nth_element_inside(xml_hdl, "CHANNEL", i);
    if(err)
    {
      sprintf(scratchpad, "Error, can not find element \"CHANNEL\" number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    buf_len = xml_get_attribute_of_element(xml_hdl, "DATA", NULL, 10 * 1000 * 1000);
    if(buf_len < 1)
    {
      sprintf(scratchpad, "Error, can not find attribute \"DATA\" in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    buf_len++;
    chan_data_in[i] = (char *)malloc(buf_len);
    if(chan_data_in[i] == NULL)
    {
      textEdit1->append("malloc() error for channel data in\n");
      goto OUT_EXIT;
    }
    chan_data_out[i] = (char *)malloc(buf_len);
    if(chan_data_out[i] == NULL)
    {
      textEdit1->append("malloc() error for channel data out\n");
      goto OUT_EXIT;
    }
    len = xml_get_attribute_of_element(xml_hdl, "DATA", chan_data_in[i], buf_len);
    if(len < 1)
    {
      sprintf(scratchpad, "Error, can not load channel data in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    chan_decoded_len[i] = base64_dec(chan_data_in[i], chan_data_out[i], buf_len);
    if(chan_decoded_len[i] < 1)
    {
      sprintf(scratchpad, "Error, can not decode data in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    if(chan_decoded_len[i] < 100)
    {
      sprintf(scratchpad, "Error, not enough samples in channel number %i\n", i + 1);
      textEdit1->append(scratchpad);
      goto OUT_EXIT;
    }

    if(i)
    {
      if(chan_decoded_len[i] != chan_decoded_len[i-1])
      {
        sprintf(scratchpad, "Error, number of samples in channel number %i is not equal to other channels\n", i + 1);
        textEdit1->append(scratchpad);
        goto OUT_EXIT;
      }
    }

    xml_go_up(xml_hdl);
  }

/////////////////////////////////////// GET THE START DATE AND TIME /////////////////////

    len = xml_get_attribute_of_element(xml_hdl, "ACQUISITION_TIME_XML", start_date_time, 64);
    if(len < 19)
    {
      textEdit1->append("Error, can not find attribute \"ACQUISITION_TIME_XML\"\n");
      goto OUT_EXIT;
    }
    start_date_time[4] = 0;
    start_date_time[7] = 0;
    start_date_time[10] = 0;
    start_date_time[13] = 0;
    start_date_time[16] = 0;
    start_date_time[19] = 0;

    err = 0;

    if((start_date_time[0] < '0') || (start_date_time[0] > '9'))  err = 1;
    if((start_date_time[1] < '0') || (start_date_time[1] > '9'))  err = 1;
    if((start_date_time[2] < '0') || (start_date_time[2] > '9'))  err = 1;
    if((start_date_time[3] < '0') || (start_date_time[3] > '9'))  err = 1;
    if((start_date_time[5] < '0') || (start_date_time[5] > '9'))  err = 1;
    if((start_date_time[6] < '0') || (start_date_time[6] > '9'))  err = 1;
    if((start_date_time[8] < '0') || (start_date_time[8] > '9'))  err = 1;
    if((start_date_time[9] < '0') || (start_date_time[8] > '9'))  err = 1;
    if((start_date_time[11] < '0') || (start_date_time[11] > '9'))  err = 1;
    if((start_date_time[12] < '0') || (start_date_time[12] > '9'))  err = 1;
    if((start_date_time[14] < '0') || (start_date_time[14] > '9'))  err = 1;
    if((start_date_time[15] < '0') || (start_date_time[15] > '9'))  err = 1;
    if((start_date_time[17] < '0') || (start_date_time[17] > '9'))  err = 1;
    if((start_date_time[18] < '0') || (start_date_time[18] > '9'))  err = 1;

    if((atoi(start_date_time) < 1985) || (atoi(start_date_time) > 2099)) err = 1;
    if((atoi(start_date_time + 5) < 1) || (atoi(start_date_time + 5) > 12)) err = 1;
    if((atoi(start_date_time + 8) < 1) || (atoi(start_date_time + 8) > 31)) err = 1;
    if(atoi(start_date_time + 11) > 23) err = 1;
    if(atoi(start_date_time + 14) > 59) err = 1;
    if(atoi(start_date_time + 17) > 59) err = 1;

    if(err)
    {
      textEdit1->append("Error, malformed attribute \"ACQUISITION_TIME\"\n");
      goto OUT_EXIT;
    }

/////////////////////////////////////// GET SUBJECT ///////////////////////////////

    subject_name[0] = 0;

    err = xml_goto_nth_element_inside(xml_hdl, "SUBJECT", 0);
    if(err)
    {
      textEdit1->append("Warning, subject name not present");
    }
    else
    {
      len = xml_get_attribute_of_element(xml_hdl, "FIRST_NAME", subject_name, 128);
      if(len < 1)
      {
        textEdit1->append("Warning, subjects' first name not present");
      }
      else
      {
        strcat(subject_name, " ");
      }

      len = xml_get_attribute_of_element(xml_hdl, "LAST_NAME", subject_name + strlen(subject_name), 128);
      if(len < 1)
      {
        textEdit1->append("Warning, subjects' last name not present");
      }

      if(char_encoding == 2)
      {
        utf8_to_latin1(subject_name);
      }

      len = xml_get_attribute_of_element(xml_hdl, "GENDER", scratchpad, 128);
      if(len < 1)
      {
        textEdit1->append("Warning, subjects' gender not present");
      }
      if(scratchpad[0] == 'M')
      {
        subject_gender = 1;
      }
      else if(scratchpad[0] == 'F')
        {
          subject_gender = 0;
        }
        else
        {
          subject_gender = -1;
        }

      xml_go_up(xml_hdl);
    }

/////////////////////////////////////// GET DEVICE ///////////////////////////////

    device_name[0] = 0;

    err = xml_goto_nth_element_inside(xml_hdl, "SOURCE", 0);
    if(err)
    {
      textEdit1->append("Warning, source/device info not present");
    }
    else
    {
      len = xml_get_attribute_of_element(xml_hdl, "MODEL", device_name, 128);
      if(len < 1)
      {
        textEdit1->append("Warning, model name not present");
      }

      if(char_encoding == 2)
      {
        utf8_to_latin1(device_name);
      }

      xml_go_up(xml_hdl);
    }

/////////////////////////////////////// STORE TO EDF ///////////////////////////////

  remove_extension_from_filename(path);
  strcat(path, ".edf");

  strcpy(path, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    goto OUT_EXIT;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, chan_cnt);
  if(edf_hdl < 0)
  {
    textEdit1->append("Error, can not open EDF file for writing\n");
    goto OUT_EXIT;
  }

  for(i=0; i<chan_cnt; i++)
  {
    if(edf_set_samplefrequency(edf_hdl, i, chan_sf_block))
    {
      textEdit1->append("Error, edf_set_samplefrequency()\n");
      goto OUT_EXIT;
    }
  }

  for(i=0; i<chan_cnt; i++)
  {
    if(chan_units_per_mv[i] > 327)
    {
      if(edf_set_physical_maximum(edf_hdl, i, 32767000.0 / chan_units_per_mv[i]))
      {
        textEdit1->append("Error, edf_set_physical_maximum()\n");
        goto OUT_EXIT;
      }

      if(edf_set_physical_minimum(edf_hdl, i, -32768000.0 / chan_units_per_mv[i]))
      {
        textEdit1->append("Error, edf_set_physical_minimum()\n");
        goto OUT_EXIT;
      }

      if(edf_set_physical_dimension(edf_hdl, i, "uV"))
      {
        textEdit1->append("Error, edf_set_physical_dimension()\n");
        goto OUT_EXIT;
      }
    }
    else
    {
      if(edf_set_physical_maximum(edf_hdl, i, 32767.0 / chan_units_per_mv[i]))
      {
        textEdit1->append("Error, edf_set_physical_maximum()\n");
        goto OUT_EXIT;
      }

      if(edf_set_physical_minimum(edf_hdl, i, -32768.0 / chan_units_per_mv[i]))
      {
        textEdit1->append("Error, edf_set_physical_minimum()\n");
        goto OUT_EXIT;
      }

      if(edf_set_physical_dimension(edf_hdl, i, "mV"))
      {
        textEdit1->append("Error, edf_set_physical_dimension()\n");
        goto OUT_EXIT;
      }
    }

    if(edf_set_digital_maximum(edf_hdl, i, 32767))
    {
      textEdit1->append("Error, edf_set_digital_maximum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_digital_minimum(edf_hdl, i, -32768))
    {
      textEdit1->append("Error, edf_set_digital_minimum()\n");
      goto OUT_EXIT;
    }

    if(edf_set_label(edf_hdl, i, chan_name[i]))
    {
      textEdit1->append("Error, edf_set_label()\n");
      goto OUT_EXIT;
    }
  }

  if(edf_set_startdatetime(edf_hdl, atoi(start_date_time), atoi(start_date_time + 5), atoi(start_date_time + 8),
                           atoi(start_date_time + 11), atoi(start_date_time + 14), atoi(start_date_time + 17)))
  {
    textEdit1->append("Error, edf_set_startdatetime()\n");
    goto OUT_EXIT;
  }

  if(strlen(subject_name))
  {
    if(edf_set_patientname(edf_hdl, subject_name))
    {
      textEdit1->append("Error, edf_set_patientname()\n");
      goto OUT_EXIT;
    }
  }

  if(subject_gender >= 0)
  {
    if(edf_set_gender(edf_hdl, subject_gender))
    {
      textEdit1->append("Error, edf_set_gender()\n");
      goto OUT_EXIT;
    }
  }

  if(strlen(device_name))
  {
    if(edf_set_equipment(edf_hdl, device_name))
    {
      textEdit1->append("Error, edf_set_equipment()\n");
      goto OUT_EXIT;
    }
  }

  if(chan_sf_div == 1)
  {
    if(edf_set_number_of_annotation_signals(edf_hdl, 2))
    {
      textEdit1->append("Error: edf_set_number_of_annotation_signals()\n");
      goto OUT_EXIT;
    }
  }
  else
  {
    if(edf_set_datarecord_duration(edf_hdl, 100000 / chan_sf_div))
    {
      textEdit1->append("Error: edf_set_datarecord_duration()\n");
      goto OUT_EXIT;
    }
  }

/////////////////// Start conversion //////////////////////////////////////////

  datrecs = (chan_decoded_len[0] / chan_sf_block) / 2;

  for(j=0; j<datrecs; j++)
  {
    for(i=0; i<chan_cnt; i++)
    {
      if(edfwrite_digital_short_samples(edf_hdl, ((signed short *)(chan_data_out[i])) + (j * chan_sf_block)))
      {
        textEdit1->append("Error, edfwrite_digital_short_samples()\n");
        goto OUT_EXIT;
      }
    }
  }

  textEdit1->append("Done\n");

OUT_EXIT:

  if(edf_hdl >= 0)  edfclose_file(edf_hdl);

  for(i=0; i<MORTARA_MAX_CHNS; i++)
  {
    free(chan_data_in[i]);
    free(chan_data_out[i]);
  }

  xml_close(xml_hdl);
}


void UI_MortaraEDFwindow::enable_widgets(bool toggle)
{
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}





















