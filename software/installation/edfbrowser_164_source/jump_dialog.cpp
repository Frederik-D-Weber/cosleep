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




#include "jump_dialog.h"



UI_JumpMenuDialog::UI_JumpMenuDialog(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  jump_dialog = new QDialog(w_parent);

  jump_dialog->setMinimumSize(435, 200);
  jump_dialog->setMaximumSize(435, 200);
  jump_dialog->setWindowTitle("Jump to");
  jump_dialog->setModal(true);
  jump_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  label1 = new QLabel(jump_dialog);
  label1->setGeometry(10, 10, 155, 25);
  label1->setText(" dd      hh:mm:ss.ms ");

  daybox1 = new QSpinBox(jump_dialog);
  daybox1->setGeometry(10, 45, 45, 25);
  daybox1->setRange(0, 30);
  daybox1->setEnabled(false);

  daybox2 = new QSpinBox(jump_dialog);
  daybox2->setGeometry(10, 80, 45, 25);
  daybox2->setRange(0, 30);
  daybox2->setEnabled(false);

  timeEdit1 = new QTimeEdit(jump_dialog);
  timeEdit1->setGeometry(65, 45, 110, 25);
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit1->setEnabled(false);

  timeEdit2 = new QTimeEdit(jump_dialog);
  timeEdit2->setGeometry(65, 80, 110, 25);
  timeEdit2->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit2->setEnabled(false);

  label2 = new QLabel("Offset from start of recording", jump_dialog);
  label2->setGeometry(185, 45, 250, 25);

  label3 = new QLabel("Absolute time", jump_dialog);
  label3->setGeometry(185, 80, 250, 25);

  jumpButton = new QPushButton(jump_dialog);
  jumpButton->setGeometry(10, 165, 100, 25);
  jumpButton->setText("Jump");
  jumpButton->setEnabled(false);

  CloseButton = new QPushButton(jump_dialog);
  CloseButton->setGeometry(325, 165, 100, 25);
  CloseButton->setText("Cancel");

  if(mainwindow->files_open)
  {
    daybox1->setEnabled(true);
    daybox2->setEnabled(true);
    timeEdit1->setEnabled(true);
    timeEdit2->setEnabled(true);

    starttime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->l_starttime
                + mainwindow->edfheaderlist[mainwindow->sel_viewtime]->starttime_offset;

    starttime /= 10000;

    time2.setHMS((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL));

    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));

    timeEdit2->setTime(time2);

    jumpButton->setEnabled(true);
  }

  QObject::connect(CloseButton, SIGNAL(clicked()),                  jump_dialog, SLOT(close()));
  QObject::connect(jumpButton,  SIGNAL(clicked()),                  this,        SLOT(jumpbutton_pressed()));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));

  jump_dialog->exec();
}


void UI_JumpMenuDialog::offsetday_changed(int days)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  milliseconds += ((long long)days * 86400000LL);

  milliseconds += starttime;

  time2.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit2->setTime(time2);

  daybox2->setValue((int)(milliseconds / 86400000LL));

  if(daybox2->value() < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::absoluteday_changed(int days)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::disconnect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  if(days < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  milliseconds = (long long)(timeEdit2->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit2->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit2->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit2->time().msec());

  milliseconds += ((long long)days * 86400000LL);

  if(milliseconds<0)  milliseconds = 0;

  milliseconds -= starttime;

  time1.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit1->setTime(time1);

  daybox1->setValue((int)(milliseconds / 86400000LL));

  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::offsettime_changed(const QTime &time_1)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::disconnect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));

  milliseconds = (long long)(time_1.hour()) * 3600000LL;
  milliseconds += (long long)(time_1.minute()) * 60000LL;
  milliseconds += (long long)(time_1.second()) * 1000LL;
  milliseconds += (long long)(time_1.msec());

  milliseconds += ((long long)daybox1->value() * 86400000LL);

  milliseconds += starttime;

  time2.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit2->setTime(time2);

  daybox2->setValue((int)(milliseconds / 86400000LL));

  if(daybox2->value() < 1)
  {
    timeEdit2->setMinimumTime(QTime((int)((starttime / 3600000LL) % 24LL), (int)((starttime % 3600000LL) / 60000LL), (int)((starttime % 60000LL) / 1000LL), (int)(starttime % 1000LL)));
  }
  else
  {
    timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  }

  QObject::connect(daybox2,     SIGNAL(valueChanged(int)),          this,        SLOT(absoluteday_changed(int)));
  QObject::connect(timeEdit2,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(absolutetime_changed(const QTime &)));
}


void UI_JumpMenuDialog::absolutetime_changed(const QTime &time_2)
{
  long long milliseconds;

  if(!mainwindow->files_open)  return;

  QObject::disconnect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::disconnect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));

  milliseconds = (long long)(time_2.hour()) * 3600000LL;
  milliseconds += (long long)(time_2.minute()) * 60000LL;
  milliseconds += (long long)(time_2.second()) * 1000LL;
  milliseconds += (long long)(time_2.msec());

  milliseconds += ((long long)daybox2->value() * 86400000LL);

  if(milliseconds<0)  milliseconds = 0;

  milliseconds -= starttime;

  time1.setHMS((int)((milliseconds / 3600000LL) % 24LL), (int)((milliseconds % 3600000LL) / 60000LL), (int)((milliseconds % 60000LL) / 1000LL), (int)(milliseconds % 1000LL));

  timeEdit1->setTime(time1);

  daybox1->setValue((int)(milliseconds / 86400000LL));

  QObject::connect(daybox1,     SIGNAL(valueChanged(int)),          this,        SLOT(offsetday_changed(int)));
  QObject::connect(timeEdit1,   SIGNAL(timeChanged(const QTime &)), this,        SLOT(offsettime_changed(const QTime &)));
}


void UI_JumpMenuDialog::jumpbutton_pressed()
{
  int i;

  long long milliseconds;

  if(!mainwindow->files_open)  return;

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  milliseconds += ((long long)daybox1->value() * 86400000LL);

  if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      mainwindow->edfheaderlist[i]->viewtime = milliseconds * (TIME_DIMENSION / 1000);
    }
  }

  if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
  {
    mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime = milliseconds * (TIME_DIMENSION / 1000);
  }

  if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)
  {
    mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime = milliseconds * (TIME_DIMENSION / 1000);

    for(i=0; i<mainwindow->files_open; i++)
    {
      if(i!=mainwindow->sel_viewtime)
      {
        mainwindow->edfheaderlist[i]->viewtime = mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - ((mainwindow->edfheaderlist[i]->utc_starttime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime) * TIME_DIMENSION);
      }
    }
  }

  if(mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      if(i!=mainwindow->sel_viewtime)
      {
        mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime - milliseconds * (TIME_DIMENSION / 1000));
      }
    }

    mainwindow->edfheaderlist[mainwindow->sel_viewtime]->viewtime = milliseconds * (TIME_DIMENSION / 1000);
  }

  mainwindow->setup_viewbuf();

  jump_dialog->close();
}





