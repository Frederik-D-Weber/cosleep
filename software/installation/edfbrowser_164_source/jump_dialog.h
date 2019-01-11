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




#ifndef JUMPMENUFORM1_H
#define JUMPMENUFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QTimeEdit>
#include <QTime>
#include <QSpinBox>
#include <QMessageBox>

#include <string.h>

#include "global.h"
#include "mainwindow.h"


class UI_Mainwindow;



class UI_JumpMenuDialog : public QObject
{
  Q_OBJECT

public:

  UI_JumpMenuDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;

private:

QDialog      *jump_dialog;

QPushButton  *CloseButton,
             *jumpButton;

QLabel       *label1,
             *label2,
             *label3;

QTimeEdit    *timeEdit1,
             *timeEdit2;

QTime        time1,
             time2;

QSpinBox     *daybox1,
             *daybox2;

long long    starttime;


private slots:

void jumpbutton_pressed();
void absolutetime_changed(const QTime &);
void offsettime_changed(const QTime &);
void absoluteday_changed(int);
void offsetday_changed(int);

};



#endif // JUMPMENUFORM1_H


