/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "bdf2edf.h"



UI_BDF2EDFwindow::UI_BDF2EDFwindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  recent_opendir = mainwindow->recent_opendir;
  recent_savedir = mainwindow->recent_savedir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 526);
  myobjectDialog->setMaximumSize(600, 526);
  myobjectDialog->setWindowTitle("BDF+ to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  label1 = new QLabel(myobjectDialog);
  label1->setGeometry(20, 20, 560, 25);

  label2 = new QLabel(myobjectDialog);
  label2->setGeometry(440, 196, 100, 25);
  label2->setText("High-pass filter");

  label3 = new QLabel(myobjectDialog);
  label3->setGeometry(440, 300, 100, 25);
  label3->setText("Divider");

  SignalsTablewidget = new QTableWidget(myobjectDialog);
  SignalsTablewidget->setGeometry(20, 66, 400, 380);
  SignalsTablewidget->setSelectionMode(QAbstractItemView::NoSelection);
  SignalsTablewidget->setColumnCount(3);
  SignalsTablewidget->setColumnWidth(0, 140);
  SignalsTablewidget->setColumnWidth(1, 100);
  SignalsTablewidget->setColumnWidth(2, 100);

  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "HighPassFilter";
  horizontallabels += "Divider";
  SignalsTablewidget->setHorizontalHeaderLabels(horizontallabels);

  spinBox1 = new QDoubleSpinBox(myobjectDialog);
  spinBox1->setGeometry(440, 232, 100, 25);
  spinBox1->setDecimals(3);
  spinBox1->setSuffix(" Hz");
  spinBox1->setRange(0.001, 100.0);
  spinBox1->setValue(0.1);

  spinBox2 = new QDoubleSpinBox(myobjectDialog);
  spinBox2->setGeometry(440, 336, 100, 25);
  spinBox2->setDecimals(3);
  spinBox2->setRange(1.0, 256.0);
  spinBox2->setValue(1.0);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 476, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 476, 100, 25);
  pushButton2->setText("Close");

  pushButton3 = new QPushButton(myobjectDialog);
  pushButton3->setGeometry(200, 476, 100, 25);
  pushButton3->setText("Convert");
  pushButton3->setEnabled(false);

  pushButton4 = new QPushButton(myobjectDialog);
  pushButton4->setGeometry(440, 66, 140, 25);
  pushButton4->setText("Select all signals");
  pushButton4->setEnabled(false);

  pushButton5 = new QPushButton(myobjectDialog);
  pushButton5->setGeometry(440, 118, 140, 25);
  pushButton5->setText("Deselect all signals");
  pushButton5->setEnabled(false);

  QObject::connect(pushButton1,    SIGNAL(clicked()),            this,           SLOT(SelectFileButton()));
  QObject::connect(pushButton2,    SIGNAL(clicked()),            myobjectDialog, SLOT(close()));
  QObject::connect(pushButton3,    SIGNAL(clicked()),            this,           SLOT(StartConversion()));
  QObject::connect(pushButton4,    SIGNAL(clicked()),            this,           SLOT(Select_all_signals()));
  QObject::connect(pushButton5,    SIGNAL(clicked()),            this,           SLOT(Deselect_all_signals()));
  QObject::connect(spinBox1,       SIGNAL(valueChanged(double)), this,           SLOT(spinbox1_changed(double)));
  QObject::connect(spinBox2,       SIGNAL(valueChanged(double)), this,           SLOT(spinbox2_changed(double)));
  QObject::connect(myobjectDialog, SIGNAL(destroyed()),          this,           SLOT(free_edfheader()));

  edfhdr = NULL;
  inputfile = NULL;
  outputfile = NULL;

  inputpath[0] = 0;

  myobjectDialog->exec();
}


void UI_BDF2EDFwindow::free_edfheader()
{
  if(edfhdr != NULL)
  {
    free(edfhdr->edfparam);
    free(edfhdr);
    edfhdr = NULL;
  }
}


void UI_BDF2EDFwindow::Select_all_signals()
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Checked);
    }
  }
}



void UI_BDF2EDFwindow::Deselect_all_signals()
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Unchecked);
    }
  }
}



void UI_BDF2EDFwindow::spinbox1_changed(double value)
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setValue(value);
    }
  }
}



void UI_BDF2EDFwindow::spinbox2_changed(double value)
{
  int i;

  if(edfhdr==NULL)
  {
    return;
  }

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setValue(value);
    }
  }
}



void UI_BDF2EDFwindow::SelectFileButton()
{
  int i;

  char txt_string[2048];

  if(edfhdr!=NULL)
  {
    label1->setText("");

    SignalsTablewidget->setRowCount(0);

    free_edfheader();

    inputfile = NULL;
    outputfile = NULL;

    inputpath[0] = 0;

    pushButton3->setEnabled(false);
    pushButton4->setEnabled(false);
    pushButton5->setEnabled(false);
  }

  strcpy(inputpath, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());

  if(!strcmp(inputpath, ""))
  {
    return;
  }

  get_directory_from_path(recent_opendir, inputpath, MAX_PATH_LENGTH);

  inputfile = fopeno(inputpath, "rb");
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.", inputpath);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(txt_string));
    messagewindow.exec();
    return;
  }

/***************** check if the file is valid ******************************/

  EDFfileCheck EDFfilechecker;

  edfhdr = EDFfilechecker.check_edf_file(inputfile, txt_string);
  if(edfhdr==NULL)
  {
    fclose(inputfile);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
    messagewindow.exec();
    return;
  }

  if(!edfhdr->bdf)
  {
    fclose(inputfile);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not a BDF file.");
    messagewindow.exec();
    free_edfheader();
    return;
  }

/***************** load signalproperties ******************************/

  label1->setText(QString::fromLocal8Bit(inputpath));

  SignalsTablewidget->setRowCount(edfhdr->edfsignals);

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    SignalsTablewidget->setRowHeight(i, 25);

    SignalsTablewidget->setCellWidget(i, 0, new QCheckBox(edfhdr->edfparam[i].label));
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setTristate(false);
    ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setCheckState(Qt::Checked);

    if(!edfhdr->edfparam[i].annotation)
    {
      SignalsTablewidget->setCellWidget(i, 1, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setDecimals(3);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setSuffix(" Hz");
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setRange(0.001, 100.0);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->setValue(spinBox1->value());

      SignalsTablewidget->setCellWidget(i, 2, new QDoubleSpinBox);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setDecimals(3);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setRange(1.0, 256.0);
      ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->setValue(spinBox2->value());
    }
    else
    {
      ((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->setEnabled(false);
    }
  }

  pushButton3->setEnabled(true);
  pushButton4->setEnabled(true);
  pushButton5->setEnabled(true);
}



void UI_BDF2EDFwindow::StartConversion()
{
  int i, j, k,
      datrecs,
      new_edfsignals,
      datarecords,
      len,
      progress_steps;

  char *readbuf,
       scratchpad[256];

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  union {
          signed short one_short;
          unsigned char two_bytes[2];
        } var2;



  pushButton3->setEnabled(false);
  pushButton4->setEnabled(false);
  pushButton5->setEnabled(false);

  if(edfhdr==NULL)
  {
    return;
  }

  if(edfhdr->edfsignals>MAXSIGNALS)
  {
    return;
  }

  new_edfsignals = 0;

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    if(!edfhdr->edfparam[i].annotation)
    {
      if(((QCheckBox *)(SignalsTablewidget->cellWidget(i, 0)))->checkState()==Qt::Checked)
      {
        signalslist[new_edfsignals] = i;

        annotlist[new_edfsignals] = 0;

        filterlist[new_edfsignals] = create_filter(0,
                                                  ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 1)))->value(),
                                                  1.0 / (edfhdr->data_record_duration / edfhdr->edfparam[i].smp_per_record));

        dividerlist[new_edfsignals] = ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(i, 2)))->value();

        new_edfsignals++;
      }
    }
    else
    {
      signalslist[new_edfsignals] = i;

      annotlist[new_edfsignals] = 1;

      filterlist[new_edfsignals] = create_filter(0, 0.01,
                                                1.0 / (edfhdr->data_record_duration / edfhdr->edfparam[i].smp_per_record));
      dividerlist[new_edfsignals] = 1.0;

      new_edfsignals++;
    }
  }

  datarecords = edfhdr->datarecords;

  QProgressDialog progress("Converting...", "Abort", 0, datarecords, myobjectDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  if(!new_edfsignals)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You must select at least one signal.");
    messagewindow.exec();
    goto END_1;
  }

  readbuf = (char *)malloc(edfhdr->recordsize);
  if(readbuf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error, (readbuf).");
    messagewindow.exec();
    goto END_2;
  }

/////////////////////////// write header ////////////////////////////////////////

  outputpath[0] = 0;

  if(recent_savedir[0]!=0)
  {
    strcpy(outputpath, recent_savedir);
    strcat(outputpath, "/");
  }
  len = strlen(outputpath);
  get_filename_from_path(outputpath + len, inputpath, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(outputpath);

  strcat(outputpath, ".edf");

  strcpy(outputpath, QFileDialog::getSaveFileName(0, "Select outputfile", QString::fromLocal8Bit(outputpath), "EDF files (*.edf *.EDF)").toLocal8Bit().data());

  if(!strcmp(outputpath, ""))
  {
    goto END_2;
  }

  get_directory_from_path(recent_savedir, outputpath, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(outputpath))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, selected file is in use.");
    messagewindow.exec();
    goto END_2;
  }

  outputfile = fopeno(outputpath, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open outputfile for writing.");
    messagewindow.exec();
    goto END_2;
  }

  fprintf(outputfile, "0       ");
  fseeko(inputfile, 8LL, SEEK_SET);
  if(fread(scratchpad, 176, 1, inputfile)!=1)
  {
    showpopupmessage("Error", "Read error (1).");
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Read error (1).");
    messagewindow.exec();
    goto END_3;
  }
  if(fwrite(scratchpad, 176, 1, outputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Write error (1).");
    messagewindow.exec();
    goto END_3;
  }
  fprintf(outputfile, "%-8i", new_edfsignals * 256 + 256);
  if(edfhdr->bdfplus)
  {
    if(edfhdr->discontinuous)
    {
      fprintf(outputfile, "EDF+D");
    }
    else
    {
      fprintf(outputfile, "EDF+C");
    }
    for(i=0; i<39; i++)
    {
      fputc(' ', outputfile);
    }
  }
  else
  {
    for(i=0; i<44; i++)
    {
      fputc(' ', outputfile);
    }
  }
  fprintf(outputfile, "%-8i", datarecords);
  snprintf(scratchpad, 256, "%f", edfhdr->data_record_duration);
  convert_trailing_zeros_to_spaces(scratchpad);
  if(scratchpad[7]=='.')
  {
    scratchpad[7] = ' ';
  }
  scratchpad[8] = 0;

  fprintf(outputfile, "%s", scratchpad);
  fprintf(outputfile, "%-4i", new_edfsignals);

  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "EDF Annotations ");
    }
    else
    {
      fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].label);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].transducer);
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "%s", edfhdr->edfparam[signalslist[i]].physdimension);
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "-1      ");
    }
    else
    {
      snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].bitvalue * -32768.0 * dividerlist[i]);
      convert_trailing_zeros_to_spaces(scratchpad);
      if(scratchpad[7]=='.')
      {
        scratchpad[7] = ' ';
      }
      scratchpad[8] = 0;
      fprintf(outputfile, "%s", scratchpad);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      fprintf(outputfile, "1       ");
    }
    else
    {
      snprintf(scratchpad, 256, "%f", edfhdr->edfparam[signalslist[i]].bitvalue * 32767.0 * dividerlist[i]);
      convert_trailing_zeros_to_spaces(scratchpad);
      if(scratchpad[7]=='.')
      {
        scratchpad[7] = ' ';
      }
      scratchpad[8] = 0;
      fprintf(outputfile, "%s", scratchpad);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "-32768  ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    fprintf(outputfile, "32767   ");
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      for(j=0; j<80; j++)
      {
        fputc(' ', outputfile);
      }
    }
    else
    {
      snprintf(scratchpad, 256, "HP:%f", ((QDoubleSpinBox *)(SignalsTablewidget->cellWidget(signalslist[i], 1)))->value());
      remove_trailing_zeros(scratchpad);
      strcat(scratchpad, "Hz ");

      strcat(scratchpad, edfhdr->edfparam[signalslist[i]].prefilter);

      for(j=strlen(scratchpad); j<200; j++)
      {
        scratchpad[j] = ' ';
      }

      scratchpad[200] = 0;

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad + j, "No filtering", 12))
        {
          for(k=j; k<(j+12); k++)
          {
            scratchpad[k] = ' ';
          }
        }
      }

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad + j, "None", 4))
        {
          for(k=j; k<(j+4); k++)
          {
            scratchpad[k] = ' ';
          }
        }
      }

      for(j=0; j<80; j++)
      {
        if(!strncmp(scratchpad + j, "HP: DC;", 7))
        {
          for(k=j; k<(j+7); k++)
          {
            scratchpad[k] = ' ';
          }
        }
      }

      scratchpad[80] = 0;

      fprintf(outputfile, "%s", scratchpad);
    }
  }
  for(i=0; i<new_edfsignals; i++)
  {
    if(annotlist[i])
    {
      if(edfhdr->edfparam[signalslist[i]].smp_per_record % 2)
      {
        fprintf(outputfile, "%-8i", ((edfhdr->edfparam[signalslist[i]].smp_per_record * 15) / 10) + 1);
      }
      else
      {
        fprintf(outputfile, "%-8i", (edfhdr->edfparam[signalslist[i]].smp_per_record * 15) / 10);
      }
    }
    else
    {
      fprintf(outputfile, "%-8i", edfhdr->edfparam[signalslist[i]].smp_per_record);
    }
  }
  for(i=0; i<(new_edfsignals * 32); i++)
  {
   fputc(' ', outputfile);
  }

///////////////////////////// start conversion //////////////////////////////////////

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  fseeko(inputfile, (long long)(edfhdr->hdrsize), SEEK_SET);

  for(datrecs=0; datrecs<datarecords; datrecs++)
  {
    if(!(datrecs%progress_steps))
    {
      progress.setValue(datrecs);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        goto END_3;
      }
    }

    if(fread(readbuf, edfhdr->recordsize, 1, inputfile) != 1)
    {
      progress.reset();
      showpopupmessage("Error", "Read error (2).");
      goto END_3;
    }

    for(i=0; i<new_edfsignals; i++)
    {
      if(annotlist[i])
      {
        if(fwrite(readbuf + edfhdr->edfparam[signalslist[i]].buf_offset, edfhdr->edfparam[signalslist[i]].smp_per_record * 3, 1, outputfile)!=1)
        {
          progress.reset();
          showpopupmessage("Error", "Write error (2).");
          goto END_3;
        }

        if(edfhdr->edfparam[signalslist[i]].smp_per_record % 2)
        {
          if(fputc(0, outputfile)==EOF)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (3).");
            goto END_3;
          }
        }
      }
      else
      {
        for(j=0; j<edfhdr->edfparam[signalslist[i]].smp_per_record; j++)
        {
          var.two[0] = *((unsigned short *)(readbuf + edfhdr->edfparam[signalslist[i]].buf_offset + (j * 3)));

          var.four[2] = *((unsigned char *)(readbuf + edfhdr->edfparam[signalslist[i]].buf_offset + (j * 3) + 2));

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          var.one_signed += edfhdr->edfparam[signalslist[i]].offset;

          var.one_signed = first_order_filter(var.one_signed, filterlist[i]);

          var.one_signed /= dividerlist[i];

          if(var.one_signed>32767)  var.one_signed = 32767;

          if(var.one_signed<-32768)  var.one_signed = -32768;

          var2.one_short = var.one_signed;

          fputc(var2.two_bytes[0], outputfile);
          if(fputc(var2.two_bytes[1], outputfile)==EOF)
          {
            progress.reset();
            showpopupmessage("Error", "Write error (4).");
            goto END_3;
          }
        }
      }
    }
  }

  progress.reset();
  showpopupmessage("Ready", "Done.");

END_3:

  fclose(outputfile);
  outputfile = NULL;

END_2:

  free(readbuf);

END_1:

  for(i=0; i<new_edfsignals; i++)
  {
    free(filterlist[i]);
  }

  fclose(inputfile);
  inputfile = NULL;

  inputpath[0] = 0;
  outputpath[0] = 0;

  free_edfheader();

  label1->setText("");

  SignalsTablewidget->setRowCount(0);
}


void UI_BDF2EDFwindow::showpopupmessage(const char *str1, const char *str2)
{
  QMessageBox messagewindow(QMessageBox::NoIcon, str1, str2);
  messagewindow.exec();
}

















