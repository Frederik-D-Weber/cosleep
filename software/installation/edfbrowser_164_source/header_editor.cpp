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



#include "header_editor.h"



UI_headerEditorWindow::UI_headerEditorWindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  setWindowTitle("EDF+/BDF+ header editor");

  edf = 0;
  bdf = 0;
  edfplus = 0;
  bdfplus = 0;
  edfsignals = 0;

  file = NULL;

  setMinimumSize(690, 560);
  setMaximumSize(690, 560);

  tabholder = new QTabWidget(this);
  tabholder->setGeometry(0, 0, 690, 490);

  tab1 = new QWidget;
  tab2 = new QWidget;

  fileNameLabel = new QLabel(tab1);
  fileNameLabel->setGeometry(10, 10, 670, 25);

  startTimeDateLabel = new QLabel(tab1);
  startTimeDateLabel->setGeometry(10, 45, 80, 25);
  startTimeDateLabel->setText("Starttime");
  startTimeDateLabel->setVisible(false);

  startTimeDate = new QDateTimeEdit(tab1);
  startTimeDate->setGeometry(100, 45, 250, 25);
  startTimeDate->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
  startTimeDate->setMinimumDate(QDate(1970, 1, 1));
  startTimeDate->setMaximumDate(QDate(2299, 12, 31));
  startTimeDate->setVisible(false);

  label1 = new QLabel(tab1);
  label1->setGeometry(10, 80, 80, 25);
  label1->setText("Subject");
  label1->setVisible(false);

  lineEdit1 = new QLineEdit(tab1);
  lineEdit1->setGeometry(100, 80, 580, 25);
  lineEdit1->setMaxLength(80);
  lineEdit1->setVisible(false);

  label2 = new QLabel(tab1);
  label2->setGeometry(10, 115, 80, 25);
  label2->setText("Recording");
  label2->setVisible(false);

  lineEdit2 = new QLineEdit(tab1);
  lineEdit2->setGeometry(100, 115, 580, 25);
  lineEdit2->setMaxLength(80);
  lineEdit2->setVisible(false);

  label3 = new QLabel(tab1);
  label3->setGeometry(10, 80, 80, 25);
  label3->setText("Subject code");
  label3->setVisible(false);

  lineEdit3 = new QLineEdit(tab1);
  lineEdit3->setGeometry(100, 80, 580, 25);
  lineEdit3->setMaxLength(80);
  lineEdit3->setVisible(false);

  label4 = new QLabel(tab1);
  label4->setGeometry(10, 115, 80, 25);
  label4->setText("Subject name");
  label4->setVisible(false);

  lineEdit4 = new QLineEdit(tab1);
  lineEdit4->setGeometry(100, 115, 580, 25);
  lineEdit4->setMaxLength(80);
  lineEdit4->setVisible(false);

  label11 = new QLabel(tab1);
  label11->setGeometry(10, 150, 80, 25);
  label11->setText("Gender");
  label11->setVisible(false);

  comboBox1 = new QComboBox(tab1);
  comboBox1->setGeometry(100, 150, 125, 25);
  comboBox1->addItem("unknown");
  comboBox1->addItem("male");
  comboBox1->addItem("female");
  comboBox1->setVisible(false);

  charsleft1Label = new QLabel(tab1);
  charsleft1Label->setGeometry(500, 150, 120, 25);
  charsleft1Label->setVisible(false);

  label12 = new QLabel(tab1);
  label12->setGeometry(10, 185, 80, 25);
  label12->setText("Birthdate");
  label12->setVisible(false);

  dateEdit1 = new QDateEdit(tab1);
  dateEdit1->setGeometry(100, 185, 125, 25);
  dateEdit1->setDisplayFormat("d MMM yyyy");
  dateEdit1->setVisible(false);

  checkBox1 = new QCheckBox("No birthdate", tab1);
  checkBox1->setGeometry(245, 185, 125, 25);
  checkBox1->setTristate(false);
  checkBox1->setCheckState(Qt::Unchecked);
  checkBox1->setVisible(false);

  label5 = new QLabel(tab1);
  label5->setGeometry(10, 220, 80, 25);
  label5->setText("Additional info");
  label5->setVisible(false);

  lineEdit5 = new QLineEdit(tab1);
  lineEdit5->setGeometry(100, 220, 580, 25);
  lineEdit5->setMaxLength(80);
  lineEdit5->setVisible(false);

  label6 = new QLabel(tab1);
  label6->setGeometry(10, 290, 80, 25);
  label6->setText("Admin. code");
  label6->setVisible(false);

  lineEdit6 = new QLineEdit(tab1);
  lineEdit6->setGeometry(100, 290, 580, 25);
  lineEdit6->setMaxLength(80);
  lineEdit6->setVisible(false);

  label7 = new QLabel(tab1);
  label7->setGeometry(10, 325, 80, 25);
  label7->setText("Technician");
  label7->setVisible(false);

  lineEdit7 = new QLineEdit(tab1);
  lineEdit7->setGeometry(100, 325, 580, 25);
  lineEdit7->setMaxLength(80);
  lineEdit7->setVisible(false);

  label8 = new QLabel(tab1);
  label8->setGeometry(10, 360, 80, 25);
  label8->setText("Device");
  label8->setVisible(false);

  lineEdit8 = new QLineEdit(tab1);
  lineEdit8->setGeometry(100, 360, 580, 25);
  lineEdit8->setMaxLength(80);
  lineEdit8->setVisible(false);

  label9 = new QLabel(tab1);
  label9->setGeometry(10, 395, 80, 25);
  label9->setText("Additional info");
  label9->setVisible(false);

  lineEdit9 = new QLineEdit(tab1);
  lineEdit9->setGeometry(100, 395, 580, 25);
  lineEdit9->setMaxLength(80);
  lineEdit9->setVisible(false);

  charsleft2Label = new QLabel(tab1);
  charsleft2Label->setGeometry(500, 430, 120, 25);
  charsleft2Label->setVisible(false);

  signallist = new QTableWidget(tab2);
  signallist->setGeometry(10, 10, 670, 430);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);
  signallist->setColumnCount(4);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setColumnWidth(0, 180);
  signallist->setColumnWidth(1, 120);
  signallist->setColumnWidth(2, 520);
  signallist->setColumnWidth(3, 520);
  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Physical dimension";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  signallist->setHorizontalHeaderLabels(horizontallabels);

  pushButton1 = new QPushButton(this);
  pushButton1->setGeometry(580, 510, 100, 25);
  pushButton1->setText("Close");

  pushButton2 = new QPushButton(this);
  pushButton2->setGeometry(200, 510, 100, 25);
  pushButton2->setText("Save");
  pushButton2->setToolTip("By clicking on \"Save\", the file will be checked for\n"
                          "errors and, if found, it will try to correct them.");

  pushButton3 = new QPushButton(this);
  pushButton3->setGeometry(10, 510, 100, 25);
  pushButton3->setText("Select file");

  helpButton = new QPushButton(this);
  helpButton->setGeometry(380, 510, 100, 25);
  helpButton->setText("Help");

  connect(pushButton1, SIGNAL(clicked()), this, SLOT(close()));
  connect(pushButton2, SIGNAL(clicked()), this, SLOT(save_hdr()));
  connect(pushButton3, SIGNAL(clicked()), this, SLOT(open_file()));
  connect(helpButton,  SIGNAL(clicked()), this, SLOT(helpbuttonpressed()));

  tabholder->addTab(tab1, "Header");
  tabholder->addTab(tab2, "Signals");

  hdr = (char *)malloc(256 * 2050);

  QMessageBox messagewindow(QMessageBox::Warning, "Warning", "Always make a backup copy of your file before using this tool!");
  messagewindow.exec();

  exec();
}


void UI_headerEditorWindow::closeEvent(QCloseEvent *cl_event)
{
  if(file != NULL)
  {
    fclose(file);
  }

  if(hdr != NULL)
  {
    free(hdr);
  }

  cl_event->accept();
}


void UI_headerEditorWindow::open_file()
{
  long long filesize;


  if(file != NULL)
  {
    fclose(file);
    file = NULL;
  }

  edf = 0;
  bdf = 0;
  edfplus = 0;
  bdfplus = 0;
  edfsignals = 0;

  disconnect(lineEdit3,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(lineEdit4,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(lineEdit5,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
  disconnect(checkBox1,   SIGNAL(stateChanged(int)),           this, SLOT(calculate_chars_left_name(int)));
  disconnect(lineEdit6,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit7,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit8,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  disconnect(lineEdit9,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));

  lineEdit3->setMaxLength(80);
  lineEdit4->setMaxLength(80);
  lineEdit5->setMaxLength(80);
  lineEdit6->setMaxLength(80);
  lineEdit7->setMaxLength(80);
  lineEdit8->setMaxLength(80);
  lineEdit9->setMaxLength(80);

  lineEdit1->clear();
  lineEdit2->clear();
  lineEdit3->clear();
  lineEdit4->clear();
  lineEdit5->clear();
  lineEdit6->clear();
  lineEdit7->clear();
  lineEdit8->clear();
  lineEdit9->clear();

  signallist->setRowCount(0);

  fileNameLabel->clear();

  if(hdr==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (hdr)");
    messagewindow.exec();
    return;
  }

  strcpy(path, QFileDialog::getOpenFileName(0, "Open file", QString::fromLocal8Bit(mainwindow->recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Selected file is in use.");
    messagewindow.exec();
    return;
  }

  file = fopeno(path, "r+b");
  if(file==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file.");
    messagewindow.exec();
    return;
  }

  rewind(file);

  if(fread(hdr, 256, 1, file) != 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not read from file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  if(!(strncmp(hdr, "0       ", 8)))  edf = 1;

  if((!(strncmp(hdr + 1, "BIOSEMI", 7))) && (((unsigned char *)hdr)[0] == 255))
  {
    bdf = 1;
  }

  if((!edf) && (!bdf))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not a valid EDF or BDF file.\n"
                                                              "Invalid version.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  edfsignals = antoi(hdr + 252, 4);

  if(edfsignals < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Invalid number of signals in header (less than 1)");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  if(edfsignals > 2049)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Number of signals in header is more than 2048.\n"
                                                              "This tool can not process more than 2048 signals.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  fseeko(file, 0LL, SEEK_END);
  filesize = ftello(file);
  if(filesize < (((edfsignals + 1LL) * 256LL) + edfsignals))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Filesize is too small.\n"
                                                              "Can not fix this file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  if((!(strncmp(hdr + 192, "EDF+C", 5))) || (!(strncmp(hdr + 192, "EDF+D", 5))))
  {
    edfplus = 1;
  }

  if((!(strncmp(hdr + 192, "BDF+C", 5))) || (!(strncmp(hdr + 192, "BDF+D", 5))))
  {
    bdfplus = 1;
  }

  read_header();
}


void UI_headerEditorWindow::read_header()
{
  int i, j, p, r;

  char scratchpad[256],
       str[256];


  if(file == NULL)
  {
    return;
  }

  fileNameLabel->setText(QString::fromLocal8Bit(path));

  rewind(file);

  if(fread(hdr, (256 * edfsignals) + 256, 1, file) != 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not read from file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }

  startTimeDate->setVisible(true);
  startTimeDateLabel->setVisible(true);

  if(edfplus || bdfplus)
  {
    startTimeDate->setMinimumDate(QDate(1970, 1, 1));
    startTimeDate->setMaximumDate(QDate(2299, 12, 31));
    startTimeDate->setDate(QDate(1970, 1, 1));
  }
  else
  {
    startTimeDate->setMinimumDate(QDate(1985, 1, 1));
    startTimeDate->setMaximumDate(QDate(2084, 12, 31));
    startTimeDate->setDate(QDate(1985, 1, 1));
  }
  startTimeDate->setTime(QTime(0, 0, 0));

  memcpy(scratchpad, hdr + 168, 8);
  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;
  r = atoi(scratchpad + 6);
  if(r < 85)
  {
    r += 2000;
  }
  else
  {
    r += 1900;
  }
  startTimeDate->setDate(QDate(r, atoi(scratchpad + 3), atoi(scratchpad)));
  startTimeDate->setMinimumDate(QDate(1985, 1, 1));
  startTimeDate->setMaximumDate(QDate(2084, 12, 31));

  memcpy(scratchpad, hdr + 176, 8);
  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;
  startTimeDate->setTime(QTime(atoi(scratchpad), atoi(scratchpad + 3), atoi(scratchpad + 6)));

  if(edfplus || bdfplus)
  {
    label1->setVisible(false);
    label2->setVisible(false);

    lineEdit1->setVisible(false);
    lineEdit2->setVisible(false);

    label3->setVisible(true);
    label4->setVisible(true);
    label5->setVisible(true);
    label6->setVisible(true);
    label7->setVisible(true);
    label8->setVisible(true);
    label9->setVisible(true);
    label11->setVisible(true);
    label12->setVisible(true);
    charsleft1Label->setVisible(true);
    charsleft2Label->setVisible(true);

    lineEdit3->setVisible(true);
    lineEdit4->setVisible(true);
    lineEdit5->setVisible(true);
    lineEdit6->setVisible(true);
    lineEdit7->setVisible(true);
    lineEdit8->setVisible(true);
    lineEdit9->setVisible(true);

    comboBox1->setVisible(true);

    checkBox1->setVisible(true);

    dateEdit1->setVisible(true);

    strncpy(scratchpad, hdr + 8, 80);
    scratchpad[80] = 0;
    strcat(scratchpad, "    ");

    p = 0;

    for(i=0; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strcpy(str, scratchpad);
      remove_trailing_spaces(str);
      lineEdit3->setText(str);
    }
    else
    {
      lineEdit3->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    comboBox1->setCurrentIndex(0);
    if(!(strncmp(scratchpad + p, "M ", 2)))
    {
      comboBox1->setCurrentIndex(1);
    }
    if(!(strncmp(scratchpad + p, "F ", 2)))
    {
      comboBox1->setCurrentIndex(2);
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    dateEdit1->setDate(QDate(1800, 1, 1));
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;

      r = 0;
      if(!strncmp(scratchpad + p + 3, "JAN", 3))  r = 1;
        else if(!strncmp(scratchpad + p + 3, "FEB", 3))  r = 2;
          else if(!strncmp(scratchpad + p + 3, "MAR", 3))  r = 3;
            else if(!strncmp(scratchpad + p + 3, "APR", 3))  r = 4;
              else if(!strncmp(scratchpad + p + 3, "MAY", 3))  r = 5;
                else if(!strncmp(scratchpad + p + 3, "JUN", 3))  r = 6;
                  else if(!strncmp(scratchpad + p + 3, "JUL", 3))  r = 7;
                    else if(!strncmp(scratchpad + p + 3, "AUG", 3))  r = 8;
                      else if(!strncmp(scratchpad + p + 3, "SEP", 3))  r = 9;
                        else if(!strncmp(scratchpad + p + 3, "OCT", 3))  r = 10;
                          else if(!strncmp(scratchpad + p + 3, "NOV", 3))  r = 11;
                            else if(!strncmp(scratchpad + p + 3, "DEC", 3))  r = 12;

      if(r)
      {
        dateEdit1->setDate(QDate(atoi(scratchpad + p + 7), r, atoi(scratchpad + p)));

        checkBox1->setCheckState(Qt::Unchecked);
      }

      p += 10;
    }
    else
    {
      checkBox1->setCheckState(Qt::Checked);
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strcpy(str, scratchpad + p);
      remove_trailing_spaces(str);
      lineEdit4->setText(str);
    }
    else
    {
      lineEdit4->clear();
    }

    p = ++i;

    scratchpad[80] = 0;
    strcpy(str, scratchpad + p);
    remove_trailing_spaces(str);
    lineEdit5->setText(str);

    strncpy(scratchpad, hdr + 88, 80);
    scratchpad[80] = 0;
    strcat(scratchpad, "    ");

    p = 10;

    for(i=10; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;

      has_startdate = 1;

      scratchpad[12] = 0;
      scratchpad[16] = 0;
      scratchpad[21] = 0;

      r = 0;
      if(!strncmp(scratchpad + 13, "JAN", 3))  r = 1;
        else if(!strncmp(scratchpad + 13, "FEB", 3))  r = 2;
          else if(!strncmp(scratchpad + 13, "MAR", 3))  r = 3;
            else if(!strncmp(scratchpad + 13, "APR", 3))  r = 4;
              else if(!strncmp(scratchpad + 13, "MAY", 3))  r = 5;
                else if(!strncmp(scratchpad + 13, "JUN", 3))  r = 6;
                  else if(!strncmp(scratchpad + 13, "JUL", 3))  r = 7;
                    else if(!strncmp(scratchpad + 13, "AUG", 3))  r = 8;
                      else if(!strncmp(scratchpad + 13, "SEP", 3))  r = 9;
                        else if(!strncmp(scratchpad + 13, "OCT", 3))  r = 10;
                          else if(!strncmp(scratchpad + 13, "NOV", 3))  r = 11;
                            else if(!strncmp(scratchpad + 13, "DEC", 3))  r = 12;

      if(r)
      {
        startTimeDate->setDate(QDate(atoi(scratchpad + 17), r, atoi(scratchpad + 10)));

        startTimeDate->setMinimumDate(QDate(1970, 1, 1));
        startTimeDate->setMaximumDate(QDate(2299, 12, 31));
      }
    }
    else
    {
      has_startdate = 0;
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strcpy(str, scratchpad + p);
      remove_trailing_spaces(str);
      lineEdit6->setText(str);
    }
    else
    {
      lineEdit6->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strcpy(str, scratchpad + p);
      remove_trailing_spaces(str);
      lineEdit7->setText(str);
    }
    else
    {
      lineEdit7->clear();
    }

    p = ++i;

    for(; i<80; i++)
    {
      if(scratchpad[i] == ' ')  break;
    }
    if(strncmp(scratchpad + p, "X ", 2))
    {
      scratchpad[i] = 0;
      for(j=p; j<i; j++)
      {
        if(scratchpad[j] == '_')  scratchpad[j] = ' ';
      }
      strcpy(str, scratchpad + p);
      remove_trailing_spaces(str);
      lineEdit8->setText(str);
    }
    else
    {
      lineEdit8->clear();
    }

    p = ++i;

    scratchpad[80] = 0;
    strcpy(str, scratchpad + p);
    remove_trailing_spaces(str);
    lineEdit9->setText(str);

    calculate_chars_left_name("");
    calculate_chars_left_recording("");

    connect(lineEdit3,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(lineEdit4,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(lineEdit5,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_name(const QString &)));
    connect(checkBox1,   SIGNAL(stateChanged(int)),           this, SLOT(calculate_chars_left_name(int)));
    connect(lineEdit6,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit7,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit8,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
    connect(lineEdit9,   SIGNAL(textEdited(const QString &)), this, SLOT(calculate_chars_left_recording(const QString &)));
  }
  else
  {
    label3->setVisible(false);
    label4->setVisible(false);
    label5->setVisible(false);
    label6->setVisible(false);
    label7->setVisible(false);
    label8->setVisible(false);
    label9->setVisible(false);
    label11->setVisible(false);
    label12->setVisible(false);
    charsleft1Label->setVisible(false);
    charsleft2Label->setVisible(false);

    lineEdit3->setVisible(false);
    lineEdit4->setVisible(false);
    lineEdit5->setVisible(false);
    lineEdit6->setVisible(false);
    lineEdit7->setVisible(false);
    lineEdit8->setVisible(false);
    lineEdit9->setVisible(false);

    comboBox1->setVisible(false);

    checkBox1->setVisible(false);

    dateEdit1->setVisible(false);

    label1->setVisible(true);
    label2->setVisible(true);

    lineEdit1->setVisible(true);
    lineEdit2->setVisible(true);

    strncpy(scratchpad, hdr + 8, 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    lineEdit1->setText(scratchpad);

    strncpy(scratchpad, hdr + 88, 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    lineEdit2->setText(scratchpad);
  }

  datrecs = antoi(hdr + 236, 8);

  signallist->setRowCount(edfsignals);

  recordsize = 0;

  for(i=0; i<edfsignals; i++)
  {
    signallist->setRowHeight(i, 25);

    strncpy(scratchpad, hdr + 256 + (i * 16), 16);
    scratchpad[16] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 0, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 0)))->setMaxLength(16);
    ((QLineEdit *)(signallist->cellWidget(i, 0)))->setText(scratchpad);

    strncpy(scratchpad, hdr + 256 + (edfsignals * 96) + (i * 8), 8);
    scratchpad[8] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 1, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 1)))->setMaxLength(8);
    ((QLineEdit *)(signallist->cellWidget(i, 1)))->setText(scratchpad);

    strncpy(scratchpad, hdr + 256 + (edfsignals * 136) + (i * 80), 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 2, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 2)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 2)))->setText(scratchpad);

    strncpy(scratchpad, hdr + 256 + (edfsignals * 16) + (i * 80), 80);
    scratchpad[80] = 0;
    remove_trailing_spaces(scratchpad);
    signallist->setCellWidget(i, 3, new QLineEdit(scratchpad));
    ((QLineEdit *)(signallist->cellWidget(i, 3)))->setMaxLength(80);
    ((QLineEdit *)(signallist->cellWidget(i, 3)))->setText(scratchpad);

    strncpy(scratchpad, hdr + 256 + (edfsignals * 216) + (i * 8), 8);
    scratchpad[8] = 0;
    if(atoi(scratchpad) < 1)
    {
      sprintf(str, "Samples per record of signal %i is less than 1.\n"
                   "Can not fix this file.", i + 1);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      fclose(file);
      file = NULL;
      return;
    }
    if(bdf)
    {
      recordsize += (atoi(scratchpad) * 3);
    }
    else
    {
      recordsize += (atoi(scratchpad) * 2);
    }

    if(edfplus)
    {
      if(!(strncmp(((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data(), "EDF Annotations", 15)))
      {
        ((QLineEdit *)(signallist->cellWidget(i, 0)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 1)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 2)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 3)))->setEnabled(false);
      }
    }

    if(bdfplus)
    {
      if(!(strncmp(((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data(), "BDF Annotations", 15)))
      {
        ((QLineEdit *)(signallist->cellWidget(i, 0)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 1)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 2)))->setEnabled(false);
        ((QLineEdit *)(signallist->cellWidget(i, 3)))->setEnabled(false);
      }
    }
  }

  if(recordsize < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Recordsize is less than 1.\n"
                                                              "Can not fix this file.");
    messagewindow.exec();
    fclose(file);
    file = NULL;
    return;
  }
}


void UI_headerEditorWindow::save_hdr()
{
  int i, j, p, len, hassign, digmin=0, digmax=0, dig_ok, dots, commas;

  char scratchpad[256],
       scratchpad2[256],
       str[256];

  long long filesize,
            l_tmp=0LL;


  if(file == NULL)
  {
    return;
  }

  sprintf(scratchpad, "%02i.%02i.%02i%02i.%02i.%02i",
          startTimeDate->date().day(),
          startTimeDate->date().month(),
          startTimeDate->date().year() % 100,
          startTimeDate->time().hour(),
          startTimeDate->time().minute(),
          startTimeDate->time().second());
  fseeko(file, 168LL, SEEK_SET);
  fprintf(file, "%s", scratchpad);

  if(edfplus || bdfplus)
  {
    if((calculate_chars_left_name("") < 0) || (calculate_chars_left_recording("") < 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many characters.");
      messagewindow.exec();
      return;
    }

    str[0] = 0;

    len = strlen(lineEdit3->text().toLatin1().data());  // patient code
    if(len)
    {
      strcpy(scratchpad, lineEdit3->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strcpy(scratchpad, "X");
      }
      strcat(scratchpad, " ");
      strcat(str, scratchpad);
    }
    else
    {
      strcat(str, "X ");
    }

    if(comboBox1->currentIndex() == 0)  // gender
    {
      strcat(str, "X ");
    }
    if(comboBox1->currentIndex() == 1)
    {
      strcat(str, "M ");
    }
    if(comboBox1->currentIndex() == 2)
    {
      strcat(str, "F ");
    }

    if(checkBox1->checkState() == Qt::Checked)  // birthdate
    {
      strcat(str, "X ");
    }
    else
    {
      i = dateEdit1->date().month();

      switch(i)
      {
        case  1 : sprintf(scratchpad, "%02i-JAN-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  2 : sprintf(scratchpad, "%02i-FEB-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  3 : sprintf(scratchpad, "%02i-MAR-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  4 : sprintf(scratchpad, "%02i-APR-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  5 : sprintf(scratchpad, "%02i-MAY-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  6 : sprintf(scratchpad, "%02i-JUN-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  7 : sprintf(scratchpad, "%02i-JUL-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  8 : sprintf(scratchpad, "%02i-AUG-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case  9 : sprintf(scratchpad, "%02i-SEP-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 10 : sprintf(scratchpad, "%02i-OCT-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 11 : sprintf(scratchpad, "%02i-NOV-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
        case 12 : sprintf(scratchpad, "%02i-DEC-%04i ", dateEdit1->date().day(), dateEdit1->date().year());
                  break;
      }

      strcat(str, scratchpad);
    }

    len = strlen(lineEdit4->text().toLatin1().data());  // patient name
    if(len)
    {
      strcpy(scratchpad, lineEdit4->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strcpy(scratchpad, "X");
      }
      strcat(scratchpad, " ");
      strcat(str, scratchpad);
    }
    else
    {
      strcat(str, "X ");
    }

    len = strlen(lineEdit5->text().toLatin1().data());  // additional info
    if(len)
    {
      strcpy(scratchpad, lineEdit5->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      strcat(str, scratchpad);
    }

    for(i=0; i<8; i++)
    {
      strcat(str, "          ");
    }

    str[80] = 0;

    fseeko(file, 8LL, SEEK_SET);
    fprintf(file, "%s", str);

    if(has_startdate)
    {
      strcpy(str, "Startdate ");

      i = startTimeDate->date().month();

      switch(i)
      {
        case  1 : sprintf(scratchpad, "%02i-JAN-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  2 : sprintf(scratchpad, "%02i-FEB-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  3 : sprintf(scratchpad, "%02i-MAR-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  4 : sprintf(scratchpad, "%02i-APR-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  5 : sprintf(scratchpad, "%02i-MAY-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  6 : sprintf(scratchpad, "%02i-JUN-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  7 : sprintf(scratchpad, "%02i-JUL-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  8 : sprintf(scratchpad, "%02i-AUG-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case  9 : sprintf(scratchpad, "%02i-SEP-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 10 : sprintf(scratchpad, "%02i-OCT-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 11 : sprintf(scratchpad, "%02i-NOV-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
        case 12 : sprintf(scratchpad, "%02i-DEC-%04i ", startTimeDate->date().day(), startTimeDate->date().year());
                  break;
      }

      strcat(str, scratchpad);
    }
    else
    {
      strcpy(str, "Startdate X ");
    }

    len = strlen(lineEdit6->text().toLatin1().data());  // administration code
    if(len)
    {
      strcpy(scratchpad, lineEdit6->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strcpy(scratchpad, "X");
      }
      strcat(scratchpad, " ");
      strcat(str, scratchpad);
    }
    else
    {
      strcat(str, "X ");
    }

    len = strlen(lineEdit7->text().toLatin1().data());  // technician
    if(len)
    {
      strcpy(scratchpad, lineEdit7->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strcpy(scratchpad, "X");
      }
      strcat(scratchpad, " ");
      strcat(str, scratchpad);
    }
    else
    {
      strcat(str, "X ");
    }

    len = strlen(lineEdit8->text().toLatin1().data());  // device
    if(len)
    {
      strcpy(scratchpad, lineEdit8->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      for(i=0; i<len; i++)
      {
        if(scratchpad[i] == ' ')  scratchpad[i] = '_';
      }
      if(!strlen(scratchpad))
      {
        strcpy(scratchpad, "X");
      }
      strcat(scratchpad, " ");
      strcat(str, scratchpad);
    }
    else
    {
      strcat(str, "X ");
    }

    len = strlen(lineEdit9->text().toLatin1().data());  // additional info
    if(len)
    {
      strcpy(scratchpad, lineEdit9->text().toLatin1().data());
      latin1_to_ascii(scratchpad, len);
      remove_trailing_spaces(scratchpad);
      strcat(str, scratchpad);
    }

    for(i=0; i<8; i++)
    {
      strcat(str, "          ");
    }

    str[80] = 0;

    fseeko(file, 88LL, SEEK_SET);
    fprintf(file, "%s", str);
  }
  else
  {
    strcpy(scratchpad, lineEdit1->text().toLatin1().data());
    for(j=strlen(scratchpad); j<80; j++)
    {
      scratchpad[j] = ' ';
    }
    latin1_to_ascii(scratchpad, 80);
    scratchpad[80] = 0;
    fseeko(file, 8LL, SEEK_SET);
    fprintf(file, "%s", scratchpad);

    strcpy(scratchpad, lineEdit2->text().toLatin1().data());
    for(j=strlen(scratchpad); j<80; j++)
    {
      scratchpad[j] = ' ';
    }
    latin1_to_ascii(scratchpad, 80);
    scratchpad[80] = 0;
    fseeko(file, 88LL, SEEK_SET);
    fprintf(file, "%s", scratchpad);
  }

  fseeko(file, 170LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 173LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 178LL, SEEK_SET);
  fputc('.', file);
  fseeko(file, 181LL, SEEK_SET);
  fputc('.', file);

  if(edfplus || bdfplus)           // reserved
  {
    fseeko(file, 197LL, SEEK_SET);
    for(j=0; j<39; j++)
    {
      fputc(' ', file);
    }
  }
  else
  {
    fseeko(file, 192LL, SEEK_SET);
    for(j=0; j<44; j++)
    {
      fputc(' ', file);
    }
  }

  for(i=0; i<edfsignals; i++)
  {
    if(((QLineEdit *)(signallist->cellWidget(i, 0)))->isEnabled() == false)  // EDF/BDF Annotations signal
    {
      fseeko(file, (long long)(256 + (edfsignals * 96) + (i * 8)), SEEK_SET);  // physical dimension
      fprintf(file, "        ");

      for(j=0; j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 136) + (i * 80)), SEEK_SET);  // prefiltering
      fprintf(file, "%s", scratchpad);

      for(j=0; j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 16) + (i * 80)), SEEK_SET);  // transducer type
      fprintf(file, "%s", scratchpad);
      fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);  // digital minimum
      if(edfplus)
      {
        fprintf(file, "-32768  ");
      }
      else
      {
        if(bdfplus)
        {
          fprintf(file, "-8388608");
        }
      }
      fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);  // digital maximum
      if(edfplus)
      {
        fprintf(file, "32767   ");
      }
      else
      {
        if(bdfplus)
        {
          fprintf(file, "8388607 ");
        }
      }
      fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);  // physical minimum
      fprintf(file, "-1      ");
      fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);  // physical maximum
      fprintf(file, "1       ");
    }
    else
    {
      strcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 0)))->text().toLatin1().data());
      for(j=strlen(scratchpad); j<16; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 16);
      scratchpad[16] = 0;
      fseeko(file, (long long)(256 + (i * 16)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 1)))->text().toLatin1().data());
      for(j=strlen(scratchpad); j<8; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 8);
      scratchpad[8] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 96) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 2)))->text().toLatin1().data());
      for(j=strlen(scratchpad); j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 80);
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 136) + (i * 80)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strcpy(scratchpad, ((QLineEdit *)(signallist->cellWidget(i, 3)))->text().toLatin1().data());
      for(j=strlen(scratchpad); j<80; j++)
      {
        scratchpad[j] = ' ';
      }
      latin1_to_ascii(scratchpad, 80);
      scratchpad[80] = 0;
      fseeko(file, (long long)(256 + (edfsignals * 16) + (i * 80)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strncpy(scratchpad, hdr + 256 + (edfsignals * 120) + (i * 8), 8);  // digital minimum
      hassign = 0;
      p = 0;
      dig_ok = 0;
      if((scratchpad[0] == '+') || (scratchpad[0] == '-'))
      {
        hassign = 1;
        p++;
      }
      for(; p<8; p++)
      {
        if((scratchpad[p] < '0') || (scratchpad[p] > '9'))
        {
          break;
        }
      }
      if(p != hassign)
      {
        for(; p<8; p++)
        {
          scratchpad[p] = ' ';
        }
        scratchpad[8] = 0;
        fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad);
        digmin = atoi(scratchpad);
        dig_ok++;
      }

      strncpy(scratchpad2, hdr + 256 + (edfsignals * 128) + (i * 8), 8);  // digital maximum
      hassign = 0;
      p = 0;
      if((scratchpad2[0] == '+') || (scratchpad2[0] == '-'))
      {
        hassign = 1;
        p++;
      }
      for(; p<8; p++)
      {
        if((scratchpad2[p] < '0') || (scratchpad2[p] > '9'))
        {
          break;
        }
      }
      if(p != hassign)
      {
        for(; p<8; p++)
        {
          scratchpad2[p] = ' ';
        }
        scratchpad2[8] = 0;
        fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad2);
        digmax = atoi(scratchpad2);
        dig_ok++;
      }
      if(dig_ok == 2)
      {
        if(digmax < digmin)
        {
          dig_ok++;

          fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
          fprintf(file, "%s", scratchpad2);

          fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
          fprintf(file, "%s", scratchpad);
        }
        if(digmax == digmin)
        {
          if(edf)
          {
            if(digmax < 32767)
            {
              digmax++;
              sprintf(scratchpad, "%i", digmax);
              strcat(scratchpad, "        ");
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
            else
            {
              digmin--;
              sprintf(scratchpad, "%i", digmin);
              strcat(scratchpad, "        ");
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
          }
          if(bdf)
          {
            if(digmax < 8388607)
            {
              digmax++;
              sprintf(scratchpad, "%i", digmax);
              strcat(scratchpad, "        ");
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 128) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
            else
            {
              digmin--;
              sprintf(scratchpad, "%i", digmin);
              strcat(scratchpad, "        ");
              scratchpad[8] = 0;
              fseeko(file, (long long)(256 + (edfsignals * 120) + (i * 8)), SEEK_SET);
              fprintf(file, "%s", scratchpad);
            }
          }
        }
      }

      strncpy(scratchpad, hdr + 256 + (edfsignals * 104) + (i * 8), 8);  // physical minimum
      for(p=7; p>0; p--)
      {
        if((scratchpad[p] < '0') || (scratchpad[p] > '9'))
        {
          scratchpad[p] = ' ';
        }
        else
        {
          break;
        }
      }
      scratchpad[8] = 0;

      for(p=0, dots=0, commas=0; p<8; p++)
      {
        if(scratchpad[p] == ',')  commas++;
        if(scratchpad[p] == '.')  dots++;
      }

      if((commas == 1) && (dots == 0))
      {
        for(p=0; p<8; p++)
        {
          if(scratchpad[p] == ',')
          {
            scratchpad[p] = '.';
            break;
          }
        }
      }

      fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad);

      strncpy(scratchpad2, hdr + 256 + (edfsignals * 112) + (i * 8), 8);  // physical maximum
      for(p=7; p>0; p--)
      {
        if((scratchpad2[p] < '0') || (scratchpad2[p] > '9'))
        {
          scratchpad2[p] = ' ';
        }
        else
        {
          break;
        }
      }
      scratchpad2[8] = 0;

      for(p=0, dots=0, commas=0; p<8; p++)
      {
        if(scratchpad2[p] == ',')  commas++;
        if(scratchpad2[p] == '.')  dots++;
      }

      if((commas == 1) && (dots == 0))
      {
        for(p=0; p<8; p++)
        {
          if(scratchpad2[p] == ',')
          {
            scratchpad2[p] = '.';
            break;
          }
        }
      }

      fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);
      fprintf(file, "%s", scratchpad2);

      if(dig_ok == 3)
      {
        fseeko(file, (long long)(256 + (edfsignals * 104) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad2);

        fseeko(file, (long long)(256 + (edfsignals * 112) + (i * 8)), SEEK_SET);
        fprintf(file, "%s", scratchpad);
      }
    }

    fseeko(file, (long long)(256 + (edfsignals * 224) + (i * 32)), SEEK_SET);  // reserved
    for(j=0; j<32; j++)
    {
      fputc(' ', file);
    }
  }

  fseeko(file, 236LL, SEEK_SET);
  fprintf(file, "%-8i", datrecs);  // datarecords

  fseeko(file, 0LL, SEEK_END);
  filesize = ftello(file);
  if(filesize != (((long long)recordsize * (long long)datrecs)) + ((edfsignals * 256) + 256))
  {
    l_tmp = filesize - ((edfsignals * 256) + 256);
    datrecs = l_tmp / (long long)recordsize;

    if(datrecs > 0)
    {
      if(datrecs > 99999999)
      {
        datrecs = 99999999;
      }

      fseeko(file, 236LL, SEEK_SET);
      fprintf(file, "%-8i", datrecs);

      l_tmp = ((edfsignals * 256) + 256);
      l_tmp += (long long)datrecs * (long long)recordsize;

      if(l_tmp != filesize)
      {
        fclose(file);

        if(QFile::resize(QString::fromLocal8Bit(path), l_tmp) == false)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not resize file.");
          messagewindow.exec();
        }

        file = fopeno(path, "r+b");
        if(file==NULL)
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file.");
          messagewindow.exec();
          return;
        }
      }
    }
  }

  fflush(file);

  read_header();

  QMessageBox messagewindow(QMessageBox::Information, "Ready", "File has been saved.");
  messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
  messagewindow.exec();
}


void UI_headerEditorWindow::calculate_chars_left_name(int)
{
  calculate_chars_left_name("");
}


int UI_headerEditorWindow::calculate_chars_left_name(const QString &)
{
  int n = 61;

  int len;

  int cursorposition[3];

  char str[128];

  if(checkBox1->checkState() == Qt::Checked)  // birthdate
  {
    n += 10;
  }

  cursorposition[0] = lineEdit3->cursorPosition();
  cursorposition[1] = lineEdit4->cursorPosition();
  cursorposition[2] = lineEdit5->cursorPosition();

  len = strlen(lineEdit3->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit4->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit5->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  if(n<1)
  {
    lineEdit3->setMaxLength(strlen(lineEdit3->text().toLatin1().data()));
    lineEdit4->setMaxLength(strlen(lineEdit4->text().toLatin1().data()));
    lineEdit5->setMaxLength(strlen(lineEdit5->text().toLatin1().data()));
  }
  else
  {
    lineEdit3->setMaxLength(80);
    lineEdit4->setMaxLength(80);
    lineEdit5->setMaxLength(80);
  }

  lineEdit3->setCursorPosition(cursorposition[0]);
  lineEdit4->setCursorPosition(cursorposition[1]);
  lineEdit5->setCursorPosition(cursorposition[2]);

  sprintf(str, "Characters left:  %i", n);

  charsleft1Label->setText(str);

  return n;
}


int UI_headerEditorWindow::calculate_chars_left_recording(const QString &)
{
  int n = 52;

  int len;

  int cursorposition[4];

  char str[128];


  cursorposition[0] = lineEdit6->cursorPosition();
  cursorposition[1] = lineEdit7->cursorPosition();
  cursorposition[2] = lineEdit8->cursorPosition();
  cursorposition[3] = lineEdit9->cursorPosition();

  len = strlen(lineEdit6->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit7->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit8->text().toLatin1().data());
  if(len>1)
  {
    n -= (len - 1);
  }

  len = strlen(lineEdit9->text().toLatin1().data());
  if(len>0)
  {
    n -= len;
  }

  if(n<1)
  {
    lineEdit6->setMaxLength(strlen(lineEdit6->text().toLatin1().data()));
    lineEdit7->setMaxLength(strlen(lineEdit7->text().toLatin1().data()));
    lineEdit8->setMaxLength(strlen(lineEdit8->text().toLatin1().data()));
    lineEdit9->setMaxLength(strlen(lineEdit9->text().toLatin1().data()));
  }
  else
  {
    lineEdit6->setMaxLength(80);
    lineEdit7->setMaxLength(80);
    lineEdit8->setMaxLength(80);
    lineEdit9->setMaxLength(80);
  }

  lineEdit6->setCursorPosition(cursorposition[0]);
  lineEdit7->setCursorPosition(cursorposition[1]);
  lineEdit8->setCursorPosition(cursorposition[2]);
  lineEdit9->setCursorPosition(cursorposition[3]);

  sprintf(str, "Characters left:  %i", n);

  charsleft2Label->setText(str);

  return n;
}


void UI_headerEditorWindow::helpbuttonpressed()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html#Header_editor"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strcpy(p_path, "file:///");
  strcat(p_path, mainwindow->specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data());
  strcat(p_path, "\\EDFbrowser\\manual.html#Header_editor");
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}





















