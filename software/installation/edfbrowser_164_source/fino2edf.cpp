/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "fino2edf.h"



UI_FINO2EDFwindow::UI_FINO2EDFwindow(char *recent_dir, char *save_dir)
{
  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 240);
  myobjectDialog->setMaximumSize(600, 240);
  myobjectDialog->setWindowTitle("Finometer to EDF converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  PatientnameLabel = new QLabel(myobjectDialog);
  PatientnameLabel->setGeometry(20, 20, 140, 25);
  PatientnameLabel->setText("Subject name");

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
  RecordingLineEdit->setText("Finometer");

  StartDatetimeedit = new QDateTimeEdit(myobjectDialog);
  StartDatetimeedit->setGeometry(160, 90, 140, 25);
  StartDatetimeedit->setDisplayFormat("dd/MM/yyyy hh:mm:ss");
  StartDatetimeedit->setDateTime(QDateTime::currentDateTime());

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 190, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 190, 100, 25);
  pushButton2->setText("Close");

  QObject::connect(pushButton1, SIGNAL(clicked()), this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_FINO2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL,
       *outputfile=NULL;

  int i, j, k, p,
      temp,
      separator=';',
      edfsignals=0,
      ok,
      timestep,
      new_smpl_time=0,
      datarecords,
      str_start,
      column,
      line_nr;

  char txt_string[2048],
       path[512],
       outputfilename[MAX_PATH_LENGTH],
       line[2048],
       scratchpad[128],
       labels[MAX_SIGNALS][17],
       phys_dim[MAX_SIGNALS][9],
       phys_min[MAX_SIGNALS][9],
       phys_max[MAX_SIGNALS][9],
       patientname[81],
       recording[81],
       datetime[17];


  double sensitivity[MAX_SIGNALS],
         new_value[MAX_SIGNALS],
         old_value[MAX_SIGNALS];

  union{
         short two;
         char one[2];
       } var;



  for(j=0; j<MAX_SIGNALS; j++)
  {
    old_value[j] = 0.0;
  }

  enable_widgets(false);

  if(!(strlen(PatientnameLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a subjectname.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  if(!(strlen(RecordingLineEdit->text().toLatin1().data())))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Please enter a recordingdescription.");
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

  strcpy(path, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "Text files (*.txt *.TXT)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    enable_widgets(true);
    return;
  }

  get_directory_from_path(recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
    messagewindow.exec();
    enable_widgets(true);
    return;
  }

/***************** check if the txtfile is valid ******************************/

  rewind(inputfile);

  if(fread(scratchpad, 5, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading the inputfile.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(strncmp(scratchpad, "\"T\";", 4))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown data in file (1).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (2).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\n')  break;
  }

  if(fread(scratchpad, 2, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile (3).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  if(strncmp(scratchpad, "\"s\";", 4))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Unknown data in file (4).");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

/***************** collect items *****************************************/

  rewind(inputfile);

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (5).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp==separator)
    {
      break;
    }
  }

  i = 0;

  while(temp!='\n')
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (6).");
      messagewindow.exec();
      fclose(inputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\r')
    {
      continue;
    }

    if((temp==separator)||(temp=='\n'))
    {
      if(edfsignals>=MAX_SIGNALS)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many labels/signals (7).");
        messagewindow.exec();
        fclose(inputfile);
        enable_widgets(true);
        return;
      }

      line[i] = 0;

      ok = 0;

      if(!strcmp(line, "\" reSYS\""))
      {
        strcpy(labels[edfsignals], "reSYS           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" fiSYS\""))
      {
        strcpy(labels[edfsignals], "fiSYS           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" reDIA\""))
      {
        strcpy(labels[edfsignals], "reDIA           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" fiDIA\""))
      {
        strcpy(labels[edfsignals], "fiDIA           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" reMAP\""))
      {
        strcpy(labels[edfsignals], "reMAP           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" fiMAP\""))
      {
        strcpy(labels[edfsignals], "fiMAP           ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"  HR\""))
      {
        strcpy(labels[edfsignals], "HR              ");
        strcpy(phys_dim[edfsignals], "bpm     ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"   IBI\""))
      {
        strcpy(labels[edfsignals], "IBI             ");
        strcpy(phys_dim[edfsignals], "s       ");
        strcpy(phys_min[edfsignals], "-31.744 ");
        strcpy(phys_max[edfsignals], "31.744  ");
        sensitivity[edfsignals] = 1000.0;
        ok = 1;
      }

      if(!strcmp(line, "\"  SV\""))
      {
        strcpy(labels[edfsignals], "SV              ");
        strcpy(phys_dim[edfsignals], "ml      ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"    CO\""))
      {
        strcpy(labels[edfsignals], "CO              ");
        strcpy(phys_dim[edfsignals], "lpm     ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"   EJT\""))
      {
        strcpy(labels[edfsignals], "EJT             ");
        strcpy(phys_dim[edfsignals], "s       ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"    TPR\""))
      {
        strcpy(labels[edfsignals], "TPR             ");
        strcpy(phys_dim[edfsignals], "MU      ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" Artifact\""))
      {
        strcpy(labels[edfsignals], "Artifact        ");
        strcpy(phys_dim[edfsignals], "_TPSROD2");
        strcpy(phys_min[edfsignals], "0       ");
        strcpy(phys_max[edfsignals], "10000000");
        sensitivity[edfsignals] = 0.0063488;
        ok = 1;
      }

      if(!strcmp(line, "\" Zao\""))
      {
        strcpy(labels[edfsignals], "Zao             ");
        strcpy(phys_dim[edfsignals], "mMU     ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\"   Cwk\""))
      {
        strcpy(labels[edfsignals], "Cwk             ");
        strcpy(phys_dim[edfsignals], "MU      ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!strcmp(line, "\" Height\""))
      {
        strcpy(labels[edfsignals], "Height          ");
        strcpy(phys_dim[edfsignals], "mmHg    ");
        strcpy(phys_min[edfsignals], "-1024   ");
        strcpy(phys_max[edfsignals], "1024    ");
        sensitivity[edfsignals] = 31.0;
        ok = 1;
      }

      if(!ok)
      {
        snprintf(txt_string, 2048, "Found unknown label/signal: %s", line);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
        messagewindow.exec();
        fclose(inputfile);
        enable_widgets(true);
        return;
      }

      edfsignals++;

      i = 0;
    }

    if(temp==separator)
    {
      continue;
    }

    line[i++] = temp;
  }

  if(!edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There are no labels/signals.");
    messagewindow.exec();
    fclose(inputfile);
    enable_widgets(true);
    return;
  }

  p = sprintf(patientname, "%s", PatientnameLineEdit->text().toLatin1().data());
  latin1_to_ascii(patientname, p);
  for( ; p<80; p++)
  {
    patientname[p] = ' ';
  }
  patientname[80] = 0;

  p = sprintf(recording, "%s", RecordingLineEdit->text().toLatin1().data());
  latin1_to_ascii(recording, p);
  for( ; p<80; p++)
  {
    recording[p] = ' ';
  }
  recording[80] = 0;

  sprintf(datetime,
          "%02i.%02i.%02i%02i.%02i.%02i",
          StartDatetimeedit->date().day(),
          StartDatetimeedit->date().month(),
          StartDatetimeedit->date().year() % 100,
          StartDatetimeedit->time().hour(),
          StartDatetimeedit->time().minute(),
          StartDatetimeedit->time().second());
  datetime[16] = 0;


/***************** write header *****************************************/

  get_filename_from_path(outputfilename, path, MAX_PATH_LENGTH);
  remove_extension_from_filename(outputfilename);
  strcat(outputfilename, "_finometer.edf");

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
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  get_directory_from_path(recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopeno(path, "wb");
  if(outputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for writing.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
    messagewindow.exec();
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  fprintf(outputfile, "0       ");
  fprintf(outputfile, "%s", patientname);
  fprintf(outputfile, "%s", recording);
  fprintf(outputfile, "%s", datetime);
  fprintf(outputfile, "%-8i", 256 * edfsignals + 256);
  fprintf(outputfile, "                                            ");
  fprintf(outputfile, "-1      ");
  fprintf(outputfile, "0.01    ");
  fprintf(outputfile, "%-4i", edfsignals);

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", labels[i]);
  }

  for(i=0; i<(80*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_dim[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_min[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "%s", phys_max[i]);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "-31744  ");
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "31744   ");
  }

  for(i=0; i<(80*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

  for(i=0; i<edfsignals; i++)
  {
    fprintf(outputfile, "1       ");
  }

  for(i=0; i<(32*edfsignals); i++)
  {
    fputc(' ', outputfile);
  }

/***************** start conversion **************************************/

  rewind(inputfile);

  for(i=0; ; )
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Reached end of file unexpectedly (8).");
      messagewindow.exec();
      fclose(inputfile);
      fclose(outputfile);
      enable_widgets(true);
      return;
    }

    if(temp=='\n')  i++;

    if(i>1)  break;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(k=0; k<10; k++)  qApp->processEvents();

  i = 0;

  column = 0;

  datarecords = 0;

  str_start = 0;

  line_nr = 2;

  while(1)
  {
    temp = fgetc(inputfile);

    if(temp==EOF)
    {
      for(k=0; k<edfsignals; k++)
      {
        temp = (int)(new_value[k] * sensitivity[k]);

        if(temp>31744)  temp = 31744;

        if(temp<-31744)  temp = -31744;

        var.two = (short)temp;

        fputc(var.one[0], outputfile);
        fputc(var.one[1], outputfile);
      }

      datarecords++;

      break;
    }

    line[i] = temp;

  /**************************************/

    if(line[i]==',')
    {
      line[i] = '.';
    }

    if((line[i]==separator)||(line[i]=='\n'))
    {
      if(column)
      {
        new_value[column-1] = atof(line + str_start);
      }
      else
      {
        new_smpl_time = (int)(atof(line + str_start) * 100.0);
      }

      if(line[i]=='\n')
      {
        /**************************************/

        line_nr++;

        if(column!=edfsignals)
        {
          QApplication::restoreOverrideCursor();
          snprintf(txt_string, 2048, "Number of separators in line %i is wrong.", line_nr);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }

        timestep = new_smpl_time - datarecords;

        if(timestep<=0)
        {
          QApplication::restoreOverrideCursor();
          snprintf(txt_string, 2048, "Timestep <= 0 in line %i.", line_nr);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
          messagewindow.exec();
          fclose(inputfile);
          fclose(outputfile);
          enable_widgets(true);
          return;
        }

        for(j=0; j<timestep; j++)
        {
          for(k=0; k<edfsignals; k++)
          {
            temp = (int)((old_value[k] + ((new_value[k] - old_value[k]) * ((double)j / (double)timestep))) * sensitivity[k]);

            if(temp>31744)  temp = 31744;

            if(temp<-31744)  temp = -31744;

            var.two = (short)temp;

            fputc(var.one[0], outputfile);
            fputc(var.one[1], outputfile);
          }

          datarecords++;
        }

        for(j=0; j<edfsignals; j++)
        {
          old_value[j] = new_value[j];
        }

        /**************************************/

        str_start = 0;

        i = 0;

        column = 0;

        continue;
      }

      str_start = i + 1;

      column++;
    }

  /**************************************/

    i++;

    if(i>2046)
    {
      QApplication::restoreOverrideCursor();
      snprintf(txt_string, 2048, "Line %i is too long.", line_nr);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
      messagewindow.exec();
      fclose(inputfile);
      fclose(outputfile);
      enable_widgets(true);
      return;
    }
  }

  QApplication::restoreOverrideCursor();

  fseeko(outputfile, 236LL, SEEK_SET);

  fprintf(outputfile, "%-8i", datarecords);

  if(fclose(outputfile))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while closing outputfile.");
    messagewindow.exec();
    enable_widgets(true);
    fclose(inputfile);
    return;
  }

  if(fclose(inputfile))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while closing inputfile.");
    messagewindow.exec();
  }

  snprintf(txt_string, 2048, "Done, EDF file is located at %s", path);
  QMessageBox messagewindow(QMessageBox::Information, "Ready", txt_string);
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();

  enable_widgets(true);
}


void UI_FINO2EDFwindow::enable_widgets(bool toggle)
{
  StartDatetimeedit->setEnabled(toggle);
  PatientnameLineEdit->setEnabled(toggle);
  RecordingLineEdit->setEnabled(toggle);
  pushButton1->setEnabled(toggle);
  pushButton2->setEnabled(toggle);
}



