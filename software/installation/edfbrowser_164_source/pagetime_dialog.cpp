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




#include "pagetime_dialog.h"



UI_Userdefined_timepage_Dialog::UI_Userdefined_timepage_Dialog(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  set_diplaytime_dialog = new QDialog(w_parent);

  set_diplaytime_dialog->setMinimumSize(335, 200);
  set_diplaytime_dialog->setMaximumSize(335, 200);
  set_diplaytime_dialog->setWindowTitle("Set timescale");
  set_diplaytime_dialog->setModal(true);
  set_diplaytime_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  label1 = new QLabel(set_diplaytime_dialog);
  label1->setGeometry(70, 25, 110, 25);
  label1->setText("hh:mm:ss:mmm");

  timeEdit1 = new QTimeEdit(set_diplaytime_dialog);
  timeEdit1->setGeometry(65, 50, 110, 25);
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));

  OKButton = new QPushButton(set_diplaytime_dialog);
  OKButton->setGeometry(10, 165, 100, 25);
  OKButton->setText("OK");

  CloseButton = new QPushButton(set_diplaytime_dialog);
  CloseButton->setGeometry(225, 165, 100, 25);
  CloseButton->setText("Cancel");

  QObject::connect(CloseButton, SIGNAL(clicked()), set_diplaytime_dialog, SLOT(close()));
  QObject::connect(OKButton,    SIGNAL(clicked()), this,                  SLOT(okbutton_pressed()));

  set_diplaytime_dialog->exec();
}


void UI_Userdefined_timepage_Dialog::okbutton_pressed()
{
  long long milliseconds;

  if(!mainwindow->files_open)
  {
    set_diplaytime_dialog->close();

    return;
  }

  milliseconds = (long long)(timeEdit1->time().hour()) * 3600000LL;
  milliseconds += (long long)(timeEdit1->time().minute()) * 60000LL;
  milliseconds += (long long)(timeEdit1->time().second()) * 1000LL;
  milliseconds += (long long)(timeEdit1->time().msec());

  if(!milliseconds)
  {
    return;
  }

  mainwindow->pagetime = milliseconds * (TIME_DIMENSION / 1000);

  mainwindow->setup_viewbuf();

  set_diplaytime_dialog->close();
}





