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



#include "export_annotations.h"


UI_ExportAnnotationswindow::UI_ExportAnnotationswindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  ExportAnnotsDialog = new QDialog;

  ExportAnnotsDialog->setMinimumSize(800, 570);
  ExportAnnotsDialog->setMaximumSize(800, 570);
  ExportAnnotsDialog->setWindowTitle("Export annotations");
  ExportAnnotsDialog->setModal(true);
  ExportAnnotsDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(ExportAnnotsDialog);
  filelist->setGeometry(10, 10, 780, 75);
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  formatGroupBox = new QGroupBox("output format", ExportAnnotsDialog);
  formatGroupBox->setGeometry(10, 120, 200, 135);

  CSVRadioButton = new QRadioButton("ASCII/CSV");

  EDFplusRadioButton = new QRadioButton("EDFplus");

  XMLRadioButton = new QRadioButton("XML");

  formatVBoxLayout = new QVBoxLayout;
  formatVBoxLayout->addWidget(CSVRadioButton);
  formatVBoxLayout->addWidget(EDFplusRadioButton);
  formatVBoxLayout->addWidget(XMLRadioButton);
  formatGroupBox->setLayout(formatVBoxLayout);

  fileGroupBox = new QGroupBox("export annotations", ExportAnnotsDialog);
  fileGroupBox->setGeometry(320, 120, 200, 90);

  selectRadioButton = new QRadioButton("from selected file");
  selectRadioButton->setChecked(true);

  mergeRadioButton = new QRadioButton("from all files (merge)");

  fileVBoxLayout = new QVBoxLayout;
  fileVBoxLayout->addWidget(selectRadioButton);
  fileVBoxLayout->addWidget(mergeRadioButton);
  fileGroupBox->setLayout(fileVBoxLayout);

  asciiSettingsGroupBox = new QGroupBox("ASCII settings", ExportAnnotsDialog);
  asciiSettingsGroupBox->setGeometry(10, 270, 300, 290);

  separatorLabel = new QLabel(ExportAnnotsDialog);
  separatorLabel->setText("separator:");

  separatorBox = new QComboBox(ExportAnnotsDialog);
  separatorBox->addItem("comma");
  separatorBox->addItem("tab");
  separatorBox->addItem("semicolon");

  asciiSecondsRadioButton = new QRadioButton("seconds relative to start of file");
  asciiSecondsRadioButton->setChecked(true);

  asciiISOtimeRadioButton = new QRadioButton("ISO timestamp hh:mm:ss");

  asciiISOtimedateRadioButton = new QRadioButton("ISO datetimestamp yyyy-mm-ddThh:mm:ss");

  asciiISOtimeFractionRadioButton = new QRadioButton("ISO timestamp hh:mm:ss.xxx");

  asciiISOtimedateFractionRadioButton = new QRadioButton("ISO datetimestamp yyyy-mm-ddThh:mm:ss.xxx");

  durationCheckBox = new QCheckBox("include duration", ExportAnnotsDialog);
  durationCheckBox->setTristate(false);
  durationCheckBox->setCheckState(Qt::Unchecked);

  asciiSettingsHBoxLayout = new QHBoxLayout;
  asciiSettingsHBoxLayout->addWidget(separatorLabel);
  asciiSettingsHBoxLayout->addWidget(separatorBox);

  asciiSettingsVBoxLayout = new QVBoxLayout;
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout);
  asciiSettingsVBoxLayout->addWidget(asciiSecondsRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimeFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(asciiISOtimedateFractionRadioButton);
  asciiSettingsVBoxLayout->addWidget(durationCheckBox);
  asciiSettingsGroupBox->setLayout(asciiSettingsVBoxLayout);

  ExportButton = new QPushButton(ExportAnnotsDialog);
  ExportButton->setGeometry(500, 530, 100, 25);
  ExportButton->setText("Export");

  CloseButton = new QPushButton(ExportAnnotsDialog);
  CloseButton->setGeometry(690, 530, 100, 25);
  CloseButton->setText("Close");

  separatorBox->setCurrentIndex(mainwindow->export_annotations_var->separator);

  switch(mainwindow->export_annotations_var->format)
  {
    case 0 : EDFplusRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 8 : XMLRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(false);
             break;

    case 1 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiSecondsRadioButton->setChecked(true);
             break;

    case 2 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeRadioButton->setChecked(true);
             break;

    case 3 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateRadioButton->setChecked(true);
             break;

    case 4 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimeFractionRadioButton->setChecked(true);
             break;

    case 5 : CSVRadioButton->setChecked(true);
             asciiSettingsGroupBox->setEnabled(true);
             asciiISOtimedateFractionRadioButton->setChecked(true);
             break;
  }

  if(mainwindow->export_annotations_var->duration)
  {
    durationCheckBox->setChecked(true);
  }
  else
  {
    durationCheckBox->setChecked(false);
  }

  QObject::connect(CloseButton,    SIGNAL(clicked()),     ExportAnnotsDialog, SLOT(close()));
  QObject::connect(ExportButton,   SIGNAL(clicked()),     this,               SLOT(ExportButtonClicked()));
  QObject::connect(CSVRadioButton, SIGNAL(toggled(bool)), this,               SLOT(outputformatRadioButtonClicked(bool)));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  ExportAnnotsDialog->exec();
}


void UI_ExportAnnotationswindow::outputformatRadioButtonClicked(bool checked)
{
  if(checked == true)
  {
    asciiSettingsGroupBox->setEnabled(true);
  }
  else
  {
    asciiSettingsGroupBox->setEnabled(false);
  }
}



void UI_ExportAnnotationswindow::ExportButtonClicked()
{
  int i, j, n,
      len,
      csv_format=0,
      hdl,
      annot_cnt,
      temp,
      include_duration;

  char path[MAX_PATH_LENGTH],
       str[1024],
       separator;

  FILE *annotationfile=NULL;

  struct annotation_list *annot_list;

  struct annotationblock *annot;

  struct edfhdrblock *hdr;

  struct date_time_struct tm;


  if(CSVRadioButton->isChecked() == true)
  {
    if(asciiSecondsRadioButton->isChecked() == true)
    {
      csv_format = 1;
    }

    if(asciiISOtimeRadioButton->isChecked() == true)
    {
      csv_format = 2;
    }

    if(asciiISOtimedateRadioButton->isChecked() == true)
    {
      csv_format = 3;
    }

    if(asciiISOtimeFractionRadioButton->isChecked() == true)
    {
      csv_format = 4;
    }

    if(asciiISOtimedateFractionRadioButton->isChecked() == true)
    {
      csv_format = 5;
    }
  }

  if(EDFplusRadioButton->isChecked() == true)
  {
    csv_format = 0;
  }

  if(XMLRadioButton->isChecked() == true)
  {
    csv_format = 8;
  }

  mainwindow->export_annotations_var->format = csv_format;

  if(separatorBox->currentIndex() == 0)
  {
    separator = ',';
    mainwindow->export_annotations_var->separator = 0;
  }
  else if(separatorBox->currentIndex() == 1)
  {
    separator = '\t';
    mainwindow->export_annotations_var->separator = 1;
  }
  else
  {
    separator = ';';
    mainwindow->export_annotations_var->separator = 2;
  }

  if(durationCheckBox->checkState() == Qt::Checked)
  {
    include_duration = 1;
    mainwindow->export_annotations_var->duration = 1;
  }
  else
  {
    include_duration = 0;
    mainwindow->export_annotations_var->duration = 0;
  }

  if(!mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  if(filelist->count() < 1)
  {
    ExportAnnotsDialog->close();
    return;
  }

  ExportButton->setEnabled(false);
  CloseButton->setEnabled(false);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  if(i==mainwindow->files_open)
  {
    ExportAnnotsDialog->close();
    return;
  }

  n = i;

  if(mergeRadioButton->isChecked() == true)
  {
    n = mainwindow->sel_viewtime;
  }

  hdr = mainwindow->edfheaderlist[n];

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[n]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  if((csv_format > 0) && (csv_format < 6))
  {
    strcat(path, "_annotations.txt");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT *.csv *.CSV)").toLocal8Bit().data());
  }

  if(csv_format == 8)
  {
    strcat(path, "_annotations.xml");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "XML files (*.xml *.XML)").toLocal8Bit().data());
  }

  if(csv_format == 0)
  {
    strcat(path, "_annotations.edf");

    strcpy(path, QFileDialog::getSaveFileName(0, "Output file", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());
  }

  if(!strcmp(path, ""))
  {
    ExportAnnotsDialog->close();
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Export annotations", "Error, selected file is in use.");
    messagewindow.exec();
    return;
  }

  annot_list = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[n]->annot_list);

  if(mergeRadioButton->isChecked() == true)
  {
    for(i=0; i < mainwindow->files_open; i++)
    {
      if(i != mainwindow->sel_viewtime)
      {
        annot_cnt = edfplus_annotation_size(&mainwindow->edfheaderlist[i]->annot_list);

        for(j=0; j < annot_cnt; j++)
        {
          edfplus_annotation_add_item(annot_list, *(edfplus_annotation_get_item(&mainwindow->edfheaderlist[i]->annot_list, j)));

          annot = edfplus_annotation_get_item(annot_list, edfplus_annotation_size(annot_list) - 1);

          annot->onset -= (mainwindow->edfheaderlist[i]->viewtime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime);
        }
      }
    }

    edfplus_annotation_sort(annot_list, NULL);
  }

  annot_cnt = edfplus_annotation_size(annot_list);

///////////////////////////// CSV (text) export //////////////////////////////////////

  if((csv_format > 0) && (csv_format < 6))
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open annotationfile for writing.");
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      return;
    }

    if(include_duration)
    {
      fprintf(annotationfile, "Onset%cDuration%cAnnotation\n", separator, separator);
    }
    else
    {
      fprintf(annotationfile, "Onset%cAnnotation\n", separator);
    }

    for(j=0; j<annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }
      strncpy(str, annot->annotation, 1024);
      str[1023] = 0;
      utf8_to_latin1(str);

      len = strlen(str);
      for(i=0; i<len; i++)
      {
        if((((unsigned char *)str)[i] < 32) || (((unsigned char *)str)[i] == ','))
        {
          str[i] = '.';
        }
      }

      if(csv_format == 1)
      {
        if(include_duration)
        {
          fprintf(annotationfile, "%+.7f%c%s%c%s\n", (double)(annot->onset - hdr->starttime_offset) / TIME_DIMENSION, separator, annot->duration, separator, str);
        }
        else
        {
          fprintf(annotationfile, "%+.7f%c%s\n", (double)(annot->onset - hdr->starttime_offset) / TIME_DIMENSION, separator, str);
        }
      }

      if(csv_format == 2)
      {
        temp = annot->onset % TIME_DIMENSION;

        if(temp < 0)
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);
        }
        else
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%02i:%02i:%02i%c%s%c%s\n", tm.hour, tm.minute, tm.second, separator, annot->duration, separator, str);
        }
        else
        {
          fprintf(annotationfile, "%02i:%02i:%02i%c%s\n", tm.hour, tm.minute, tm.second, separator, str);
        }
      }

      if(csv_format == 3)
      {
        temp = annot->onset % TIME_DIMENSION;

        if(temp < 0)
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);
        }
        else
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i%c%s%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, separator, annot->duration, separator, str);
        }
        else
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, separator, str);
        }
      }

      if(csv_format == 4)
      {
        temp = annot->onset % TIME_DIMENSION;

        if(temp < 0)
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);

          temp = TIME_DIMENSION + temp;
        }
        else
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%02i:%02i:%02i.%07i%c%s%c%s\n", tm.hour, tm.minute, tm.second, temp, separator, annot->duration, separator, str);
        }
        else
        {
          fprintf(annotationfile, "%02i:%02i:%02i.%07i%c%s\n", tm.hour, tm.minute, tm.second, temp, separator, str);
        }
      }

      if(csv_format == 5)
      {
        temp = annot->onset % TIME_DIMENSION;

        if(temp < 0)
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);

          temp = TIME_DIMENSION + temp;
        }
        else
        {
          utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
        }

        if(include_duration)
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i%c%s%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, separator, annot->duration, separator, str);
        }
        else
        {
          fprintf(annotationfile, "%04i-%02i-%02iT%02i:%02i:%02i.%07i%c%s\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, separator, str);
        }
      }
    }

    fclose(annotationfile);

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// XML export //////////////////////////////////////

  if(csv_format == 8)
  {
    annotationfile = fopeno(path, "wb");
    if(annotationfile==NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open annotationfile for writing.");
      messagewindow.exec();
      ExportAnnotsDialog->close();
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      return;
    }

    fprintf(annotationfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");

    fprintf(annotationfile, "<!-- Generated by " PROGRAM_NAME " " PROGRAM_VERSION " -->\n");

    fprintf(annotationfile, "<annotationlist>\n");

    utc_to_date_time(hdr->utc_starttime, &tm);

    fprintf(annotationfile, " <recording_start_time>%04i-%02i-%02iT%02i:%02i:%02i.%07i</recording_start_time>\n",
                            tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, (int)hdr->starttime_offset);

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      temp = annot->onset % TIME_DIMENSION;

      if(temp < 0)
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION) - 1, &tm);

        temp = TIME_DIMENSION + temp;
      }
      else
      {
        utc_to_date_time(hdr->utc_starttime + (annot->onset / TIME_DIMENSION), &tm);
      }

      fprintf(annotationfile, " <annotation>\n"
                              "  <onset>%04i-%02i-%02iT%02i:%02i:%02i.%07i</onset>\n"
                              "  <duration>%s</duration>\n"
                              "  <description>",
                              tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second, temp, annot->duration);

      xml_fwrite_encode_entity(annotationfile, annot->annotation);

      fprintf(annotationfile,                     "</description>\n"
                              " </annotation>\n");
    }

    fprintf(annotationfile, "</annotationlist>\n");

    fclose(annotationfile);

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();

    return;
  }

///////////////////////////// EDFplus export //////////////////////////////////////

  if(csv_format == 0)
  {
    hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, 0);
    if(hdl < 0)
    {
      switch(hdl)
      {
        case EDFLIB_MALLOC_ERROR : strcpy(str, "EDFlib: malloc error");
                                  break;
        case EDFLIB_NO_SUCH_FILE_OR_DIRECTORY : strcpy(str, "EDFlib: no such file or directory");
                                  break;
        case EDFLIB_MAXFILES_REACHED : strcpy(str, "EDFlib: maximum files reached");
                                  break;
        case EDFLIB_FILE_ALREADY_OPENED : strcpy(str, "EDFlib: file already opened");
                                  break;
        case EDFLIB_NUMBER_OF_SIGNALS_INVALID : strcpy(str, "EDFlib: number of signals is invalid");
                                  break;
        default : strcpy(str, "EDFlib: unknown error");
                                  break;
      }

      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      edfplus_annotation_empty_list(annot_list);
      free(annot_list);
      ExportAnnotsDialog->close();
      return;
    }

    utc_to_date_time(hdr->utc_starttime, &tm);
    edf_set_startdatetime(hdl, tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    if((hdr->edfplus) || (hdr->bdfplus))
    {
      edf_set_patientname(hdl, hdr->plus_patient_name);
      edf_set_patientcode(hdl, hdr->plus_patientcode);
      if(!(strcmp(hdr->plus_gender, "Male")))
      {
        edf_set_gender(hdl, 1);
      }
      if(!(strcmp(hdr->plus_gender, "Female")))
      {
        edf_set_gender(hdl, 0);
      }
      if(hdr->plus_birthdate[0] != 0)
      {
        if(!(strncmp(hdr->plus_birthdate + 3, "jan", 3)))  n = 1;
        if(!(strncmp(hdr->plus_birthdate + 3, "feb", 3)))  n = 2;
        if(!(strncmp(hdr->plus_birthdate + 3, "mar", 3)))  n = 3;
        if(!(strncmp(hdr->plus_birthdate + 3, "apr", 3)))  n = 4;
        if(!(strncmp(hdr->plus_birthdate + 3, "may", 3)))  n = 5;
        if(!(strncmp(hdr->plus_birthdate + 3, "jun", 3)))  n = 6;
        if(!(strncmp(hdr->plus_birthdate + 3, "jul", 3)))  n = 7;
        if(!(strncmp(hdr->plus_birthdate + 3, "aug", 3)))  n = 8;
        if(!(strncmp(hdr->plus_birthdate + 3, "sep", 3)))  n = 9;
        if(!(strncmp(hdr->plus_birthdate + 3, "oct", 3)))  n = 10;
        if(!(strncmp(hdr->plus_birthdate + 3, "nov", 3)))  n = 11;
        if(!(strncmp(hdr->plus_birthdate + 3, "dec", 3)))  n = 12;
        edf_set_birthdate(hdl, atoi(hdr->plus_birthdate + 7), n, atoi(hdr->plus_birthdate));
      }
      edf_set_patient_additional(hdl, hdr->plus_patient_additional);
      edf_set_admincode(hdl, hdr->plus_admincode);
      edf_set_technician(hdl, hdr->plus_technician);
      edf_set_equipment(hdl, hdr->plus_equipment);
      edf_set_recording_additional(hdl, hdr->plus_recording_additional);
    }
    else
    {
      edf_set_patientname(hdl, hdr->patient);
      edf_set_recording_additional(hdl, hdr->recording);
    }

    int hasdot,
        decimals;

    for(j=0; j < annot_cnt; j++)
    {
      annot = edfplus_annotation_get_item(annot_list, j);
      if(annot == NULL)
      {
        break;
      }

      if(annot->duration[0] == 0)
      {
        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, -1LL, annot->annotation);
      }
      else
      {
        strcpy(str, annot->duration);

        len = strlen(str);

        hasdot = 0;

        for(i=0; i<len; i++)
        {
          if(str[i] == '.')
          {
            hasdot = 1;

            for(decimals=0; decimals<4; decimals++)
            {
              str[i] = str[i+1];

              if(str[i] == 0)
              {
                for( ; decimals<4; decimals++)
                {
                  str[i] = '0';

                  i++;
                }

                i--;
              }

              i++;
            }

            str[i] = 0;

            break;
          }
        }

        if(!hasdot)
        {
          strcat(str, "0000");
        }

        edfwrite_annotation_utf8(hdl, annot->onset / 1000LL, atoi(str), annot->annotation);
      }
    }

    if(edfclose_file(hdl) != 0)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred: edfclose_file()");
      messagewindow.exec();
    }

    edfplus_annotation_empty_list(annot_list);
    free(annot_list);

    QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
    messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
    messagewindow.exec();

    ExportAnnotsDialog->close();
  }
}















