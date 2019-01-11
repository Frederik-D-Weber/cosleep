/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "show_edf_hdr.h"



UI_EDFhdrwindow::UI_EDFhdrwindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  EDFhdrDialog = new QDialog;

  EDFhdrDialog->setMinimumSize(770, 680);

  EDFhdrDialog->setWindowTitle("File Info");
  EDFhdrDialog->setModal(true);
  EDFhdrDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(EDFhdrDialog);
  filelist->setMinimumSize(670, 75);
  filelist->setMaximumHeight(75);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  tabholder = new QTabWidget(EDFhdrDialog);

  tab1 = new QWidget;
  tab2 = new QWidget;

  label1 = new QLabel(EDFhdrDialog);
  label1->setMinimumSize(130, 25);
  label1->setMaximumSize(130, 25);
  label1->setText("Subject");

  label1a = new QLabel(EDFhdrDialog);
  label1a->setMinimumSize(580, 25);
  label1a->setMaximumSize(580, 25);
  label1a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label1a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label1a->setFrameStyle(QFrame::Box);

  label2 = new QLabel(EDFhdrDialog);
  label2->setMinimumSize(130, 25);
  label2->setMaximumSize(130, 25);
  label2->setText("Recording");

  label2a = new QLabel(EDFhdrDialog);
  label2a->setMinimumSize(580, 25);
  label2a->setMaximumSize(580, 25);
  label2a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label2a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label2a->setFrameStyle(QFrame::Box);

  label3 = new QLabel(EDFhdrDialog);
  label3->setMinimumSize(130, 25);
  label3->setMaximumSize(130, 25);
  label3->setText("Start");

  label3a = new QLabel(EDFhdrDialog);
  label3a->setMinimumSize(580, 25);
  label3a->setMaximumSize(580, 25);
  label3a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label3a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label3a->setFrameStyle(QFrame::Box);

  label4 = new QLabel(EDFhdrDialog);
  label4->setMinimumSize(130, 25);
  label4->setMaximumSize(130, 25);
  label4->setText("Duration");

  label4a = new QLabel(EDFhdrDialog);
  label4a->setMinimumSize(580, 25);
  label4a->setMaximumSize(580, 25);
  label4a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label4a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label4a->setFrameStyle(QFrame::Box);

  label5 = new QLabel(EDFhdrDialog);
  label5->setMinimumSize(130, 25);
  label5->setMaximumSize(130, 25);
  label5->setText("Reserved");

  label5a = new QLabel(EDFhdrDialog);
  label5a->setMinimumSize(580, 25);
  label5a->setMaximumSize(580, 25);
  label5a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label5a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label5a->setFrameStyle(QFrame::Box);

  label6 = new QLabel(EDFhdrDialog);
  label6->setMinimumSize(130, 25);
  label6->setMaximumSize(130, 25);
  label6->setText("Birthdate");
  label6->setVisible(false);

  label6a = new QLabel(EDFhdrDialog);
  label6a->setMinimumSize(580, 25);
  label6a->setMaximumSize(580, 25);
  label6a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label6a->setVisible(false);
  label6a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label6a->setFrameStyle(QFrame::Box);

  label7 = new QLabel(EDFhdrDialog);
  label7->setMinimumSize(130, 25);
  label7->setMaximumSize(130, 25);
  label7->setText("Subject name");
  label7->setVisible(false);

  label7a = new QLabel(EDFhdrDialog);
  label7a->setMinimumSize(580, 25);
  label7a->setMaximumSize(580, 25);
  label7a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label7a->setVisible(false);
  label7a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label7a->setFrameStyle(QFrame::Box);

  label8 = new QLabel(EDFhdrDialog);
  label8->setMinimumSize(130, 25);
  label8->setMaximumSize(130, 25);
  label8->setText("Additional info");
  label8->setVisible(false);

  label8a = new QLabel(EDFhdrDialog);
  label8a->setMinimumSize(580, 25);
  label8a->setMaximumSize(580, 25);
  label8a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label8a->setVisible(false);
  label8a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label8a->setFrameStyle(QFrame::Box);

  label9 = new QLabel(EDFhdrDialog);
  label9->setMinimumSize(130, 25);
  label9->setMaximumSize(130, 25);
  label9->setText("Startdate");
  label9->setVisible(false);

  label9a = new QLabel(EDFhdrDialog);
  label9a->setMinimumSize(580, 25);
  label9a->setMaximumSize(580, 25);
  label9a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label9a->setVisible(false);
  label9a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label9a->setFrameStyle(QFrame::Box);

  label10 = new QLabel(EDFhdrDialog);
  label10->setMinimumSize(130, 25);
  label10->setMaximumSize(130, 25);
  label10->setText("Administr. code");
  label10->setVisible(false);

  label10a = new QLabel(EDFhdrDialog);
  label10a->setMinimumSize(580, 25);
  label10a->setMaximumSize(580, 25);
  label10a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label10a->setVisible(false);
  label10a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label10a->setFrameStyle(QFrame::Box);

  label11 = new QLabel(EDFhdrDialog);
  label11->setMinimumSize(130, 25);
  label11->setMaximumSize(130, 25);
  label11->setText("Technician");
  label11->setVisible(false);

  label11a = new QLabel(EDFhdrDialog);
  label11a->setMinimumSize(580, 25);
  label11a->setMaximumSize(580, 25);
  label11a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label11a->setVisible(false);
  label11a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label11a->setFrameStyle(QFrame::Box);

  label12 = new QLabel(EDFhdrDialog);
  label12->setMinimumSize(130, 25);
  label12->setMaximumSize(130, 25);
  label12->setText("Device");
  label12->setVisible(false);

  label12a = new QLabel(EDFhdrDialog);
  label12a->setMinimumSize(580, 25);
  label12a->setMaximumSize(580, 25);
  label12a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label12a->setVisible(false);
  label12a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label12a->setFrameStyle(QFrame::Box);

  label13 = new QLabel(EDFhdrDialog);
  label13->setMinimumSize(130, 25);
  label13->setMaximumSize(130, 25);
  label13->setText("Additional info");
  label13->setVisible(false);

  label13a = new QLabel(EDFhdrDialog);
  label13a->setMinimumSize(580, 25);
  label13a->setMaximumSize(580, 25);
  label13a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label13a->setVisible(false);
  label13a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label13a->setFrameStyle(QFrame::Box);

  label20 = new QLabel(EDFhdrDialog);
  label20->setMinimumSize(130, 25);
  label20->setMaximumSize(130, 25);
  label20->setText("Datarecord duration");

  label20a = new QLabel(EDFhdrDialog);
  label20a->setMinimumSize(580, 25);
  label20a->setMaximumSize(580, 25);
  label20a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label20a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label20a->setFrameStyle(QFrame::Box);

  label21 = new QLabel(EDFhdrDialog);
  label21->setMinimumSize(130, 25);
  label21->setMaximumSize(130, 25);
  label21->setText("Version");

  label21a = new QLabel(EDFhdrDialog);
  label21a->setMinimumSize(580, 25);
  label21a->setMaximumSize(580, 25);
  label21a->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label21a->setStyleSheet("QLabel { background-color : white; color : black; }");
  label21a->setFrameStyle(QFrame::Box);

  signallist = new QTableWidget(EDFhdrDialog);
  signallist->setMinimumSize(670, 100);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setEditTriggers(QAbstractItemView::NoEditTriggers);

  pushButton1 = new QPushButton(EDFhdrDialog);
  pushButton1->setMinimumSize(100, 25);
  pushButton1->setMaximumSize(100, 25);
  pushButton1->setText("Close");

  generalHBoxLayout1 = new QHBoxLayout;
  generalHBoxLayout1->addWidget(label1);
  generalHBoxLayout1->addWidget(label1a);
  generalHBoxLayout1->addStretch(1000);

  generalHBoxLayout2 = new QHBoxLayout;
  generalHBoxLayout2->addWidget(label2);
  generalHBoxLayout2->addWidget(label2a);
  generalHBoxLayout2->addStretch(1000);

  generalHBoxLayout3 = new QHBoxLayout;
  generalHBoxLayout3->addWidget(label3);
  generalHBoxLayout3->addWidget(label3a);
  generalHBoxLayout3->addStretch(1000);

  generalHBoxLayout4 = new QHBoxLayout;
  generalHBoxLayout4->addWidget(label4);
  generalHBoxLayout4->addWidget(label4a);
  generalHBoxLayout4->addStretch(1000);

  generalHBoxLayout5 = new QHBoxLayout;
  generalHBoxLayout5->addWidget(label5);
  generalHBoxLayout5->addWidget(label5a);
  generalHBoxLayout5->addStretch(1000);

  generalHBoxLayout6 = new QHBoxLayout;
  generalHBoxLayout6->addWidget(label6);
  generalHBoxLayout6->addWidget(label6a);
  generalHBoxLayout6->addStretch(1000);

  generalHBoxLayout7 = new QHBoxLayout;
  generalHBoxLayout7->addWidget(label7);
  generalHBoxLayout7->addWidget(label7a);
  generalHBoxLayout7->addStretch(1000);

  generalHBoxLayout8 = new QHBoxLayout;
  generalHBoxLayout8->addWidget(label8);
  generalHBoxLayout8->addWidget(label8a);
  generalHBoxLayout8->addStretch(1000);

  generalHBoxLayout9 = new QHBoxLayout;
  generalHBoxLayout9->addWidget(label9);
  generalHBoxLayout9->addWidget(label9a);
  generalHBoxLayout9->addStretch(1000);

  generalHBoxLayout10 = new QHBoxLayout;
  generalHBoxLayout10->addWidget(label10);
  generalHBoxLayout10->addWidget(label10a);
  generalHBoxLayout10->addStretch(1000);

  generalHBoxLayout11 = new QHBoxLayout;
  generalHBoxLayout11->addWidget(label11);
  generalHBoxLayout11->addWidget(label11a);
  generalHBoxLayout11->addStretch(1000);

  generalHBoxLayout12 = new QHBoxLayout;
  generalHBoxLayout12->addWidget(label12);
  generalHBoxLayout12->addWidget(label12a);
  generalHBoxLayout12->addStretch(1000);

  generalHBoxLayout13 = new QHBoxLayout;
  generalHBoxLayout13->addWidget(label13);
  generalHBoxLayout13->addWidget(label13a);
  generalHBoxLayout13->addStretch(1000);

  generalHBoxLayout14 = new QHBoxLayout;
  generalHBoxLayout14->addWidget(label20);
  generalHBoxLayout14->addWidget(label20a);
  generalHBoxLayout14->addStretch(1000);

  generalHBoxLayout15 = new QHBoxLayout;
  generalHBoxLayout15->addWidget(label21);
  generalHBoxLayout15->addWidget(label21a);
  generalHBoxLayout15->addStretch(1000);

  generalVBoxLayout1 = new QVBoxLayout;
  generalVBoxLayout1->addLayout(generalHBoxLayout1);
  generalVBoxLayout1->addLayout(generalHBoxLayout2);
  generalVBoxLayout1->addLayout(generalHBoxLayout3);
  generalVBoxLayout1->addLayout(generalHBoxLayout4);
  generalVBoxLayout1->addLayout(generalHBoxLayout5);
  generalVBoxLayout1->addLayout(generalHBoxLayout6);
  generalVBoxLayout1->addLayout(generalHBoxLayout7);
  generalVBoxLayout1->addLayout(generalHBoxLayout8);
  generalVBoxLayout1->addLayout(generalHBoxLayout9);
  generalVBoxLayout1->addLayout(generalHBoxLayout10);
  generalVBoxLayout1->addLayout(generalHBoxLayout11);
  generalVBoxLayout1->addLayout(generalHBoxLayout12);
  generalVBoxLayout1->addLayout(generalHBoxLayout13);
  generalVBoxLayout1->addLayout(generalHBoxLayout14);
  generalVBoxLayout1->addLayout(generalHBoxLayout15);
  generalVBoxLayout1->addStretch(1000);
  tab1->setLayout(generalVBoxLayout1);

  signalsHBoxLayout1 = new QHBoxLayout;
  signalsHBoxLayout1->addWidget(signallist);
  tab2->setLayout(signalsHBoxLayout1);

  tabholder->addTab(tab1, "General");
  tabholder->addTab(tab2, "Signals");

  mainHBoxLayout1 = new QHBoxLayout;
  mainHBoxLayout1->addStretch(100);
  mainHBoxLayout1->addWidget(pushButton1);

  mainVBoxLayout1 = new QVBoxLayout;
  mainVBoxLayout1->addWidget(filelist);
  mainVBoxLayout1->addSpacing(10);
  mainVBoxLayout1->addWidget(tabholder, 1000);
  mainVBoxLayout1->addSpacing(10);
  mainVBoxLayout1->addLayout(mainHBoxLayout1);

  EDFhdrDialog->setLayout(mainVBoxLayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()),              EDFhdrDialog, SLOT(close()));
  QObject::connect(filelist,    SIGNAL(currentRowChanged(int)), this,         SLOT(show_params(int)));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  EDFhdrDialog->exec();
}


void UI_EDFhdrwindow::show_params(int row)
{
  int i,
      signal_cnt;

  char str[512];

  long long file_duration;

  struct date_time_struct date_time;


  if(row<0)  return;

  if(mainwindow->edfheaderlist[row]->edfplus || mainwindow->edfheaderlist[row]->bdfplus)
  {
    label1a->setText(mainwindow->edfheaderlist[row]->plus_patientcode);
    label1a->setText(mainwindow->edfheaderlist[row]->plus_patientcode);

    label1->setText("Subject code");

    label2a->setText(mainwindow->edfheaderlist[row]->plus_gender);

    label2->setText("Sex");

    label6->setVisible(true);
    label6a->setVisible(true);
    label6a->setText(mainwindow->edfheaderlist[row]->plus_birthdate);

    label7->setVisible(true);
    label7a->setVisible(true);
    label7a->setText(mainwindow->edfheaderlist[row]->plus_patient_name);

    label8->setVisible(true);
    label8a->setVisible(true);
    label8a->setText(mainwindow->edfheaderlist[row]->plus_patient_additional);

    label9->setVisible(true);
    label9a->setVisible(true);
    label9a->setText(mainwindow->edfheaderlist[row]->plus_startdate);

    label10->setVisible(true);
    label10a->setVisible(true);
    label10a->setText(mainwindow->edfheaderlist[row]->plus_admincode);

    label11->setVisible(true);
    label11a->setVisible(true);
    label11a->setText(mainwindow->edfheaderlist[row]->plus_technician);

    label12->setVisible(true);
    label12a->setVisible(true);
    label12a->setText(mainwindow->edfheaderlist[row]->plus_equipment);

    label13->setVisible(true);
    label13a->setVisible(true);
    label13a->setText(mainwindow->edfheaderlist[row]->plus_recording_additional);
  }
  else // old EDF
  {
    label6->setVisible(false);
    label6a->setVisible(false);
    label7->setVisible(false);
    label7a->setVisible(false);
    label8->setVisible(false);
    label8a->setVisible(false);
    label9->setVisible(false);
    label9a->setVisible(false);
    label10->setVisible(false);
    label10a->setVisible(false);
    label11->setVisible(false);
    label11a->setVisible(false);
    label12->setVisible(false);
    label12a->setVisible(false);
    label13->setVisible(false);
    label13a->setVisible(false);

    label1->setText("Subject");

    label1a->setText(mainwindow->edfheaderlist[row]->patient);

    label2->setText("Recording");

    label2a->setText(mainwindow->edfheaderlist[row]->recording);
  }

  utc_to_date_time(mainwindow->edfheaderlist[row]->utc_starttime, &date_time);

  date_time.month_str[0] += 32;
  date_time.month_str[1] += 32;
  date_time.month_str[2] += 32;

  snprintf(str, 400, "%i %s %i  %2i:%02i:%02i",
          date_time.day,
          date_time.month_str,
          date_time.year,
          date_time.hour,
          date_time.minute,
          date_time.second);

  if(mainwindow->edfheaderlist[row]->starttime_offset != 0LL)
  {
#ifdef Q_OS_WIN32
    __mingw_snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#else
    snprintf(str + strlen(str), 100, ".%07lli", mainwindow->edfheaderlist[row]->starttime_offset);
#endif

    remove_trailing_zeros(str);
  }

  label3a->setText(str);

  file_duration = mainwindow->edfheaderlist[row]->long_data_record_duration * mainwindow->edfheaderlist[row]->datarecords;

  if((file_duration / TIME_DIMENSION) / 10)
  {
    snprintf(str, 512,
            "%2i:%02i:%02i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL));
  }
  else
  {
    snprintf(str, 512,
            "%2i:%02i:%02i.%06i",
            (int)((file_duration / TIME_DIMENSION)/ 3600LL),
            (int)(((file_duration / TIME_DIMENSION) % 3600LL) / 60LL),
            (int)((file_duration / TIME_DIMENSION) % 60LL),
            (int)((file_duration % TIME_DIMENSION) / 10LL));
  }

  label4a->setText(str);

  label5a->setText(mainwindow->edfheaderlist[row]->reserved);

  snprintf(str,  512, "%.12f", mainwindow->edfheaderlist[row]->data_record_duration);

  remove_trailing_zeros(str);

  label20a->setText(str);

  label21a->setText(mainwindow->edfheaderlist[row]->version);

  signal_cnt = mainwindow->edfheaderlist[row]->edfsignals;

  signallist->setColumnCount(10);
  signallist->setRowCount(signal_cnt);
  signallist->setSelectionMode(QAbstractItemView::NoSelection);
  signallist->setColumnWidth(0, 180);
  signallist->setColumnWidth(1, 120);
  signallist->setColumnWidth(2, 120);
  signallist->setColumnWidth(3, 120);
  signallist->setColumnWidth(4, 120);
  signallist->setColumnWidth(5, 120);
  signallist->setColumnWidth(6, 120);
  signallist->setColumnWidth(7, 120);
  signallist->setColumnWidth(8, 520);
  signallist->setColumnWidth(9, 520);
  QStringList horizontallabels;
  horizontallabels += "Label";
  horizontallabels += "Samplefrequency";
  horizontallabels += "Physical maximum";
  horizontallabels += "Physical minimum";
  horizontallabels += "Physical dimension";
  horizontallabels += "Digital maximum";
  horizontallabels += "Digital minimum";
  horizontallabels += "Samples per record";
  horizontallabels += "Prefilter";
  horizontallabels += "Transducer";
  signallist->setHorizontalHeaderLabels(horizontallabels);

  QLabel *ql;

  int margin=3;

  for(i=0; i<signal_cnt; i++)
  {
    signallist->setRowHeight(i, 25);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].label);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 0, ql);
    convert_to_metric_suffix(str,
                             (double)mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record / mainwindow->edfheaderlist[row]->data_record_duration,
                             3);
    strcat(str, "Hz");
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 1, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_max);
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 2, ql);
    snprintf(str, 512, "%+f", mainwindow->edfheaderlist[row]->edfparam[i].phys_min);
    remove_trailing_zeros(str);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 3, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].physdimension);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 4, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_max);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 5, ql);
    snprintf(str, 512, "%+i", mainwindow->edfheaderlist[row]->edfparam[i].dig_min);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 6, ql);
    snprintf(str, 512, "%i", mainwindow->edfheaderlist[row]->edfparam[i].smp_per_record);
    ql = new QLabel(str);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 7, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].prefilter);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 8, ql);
    ql = new QLabel(mainwindow->edfheaderlist[row]->edfparam[i].transducer);
    ql->setMargin(margin);
    ql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    signallist->setCellWidget(i, 9, ql);
  }
}

















