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




#ifndef SHOWEDFHDRFORM1_H
#define SHOWEDFHDRFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QFrame>
#include <QStyle>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "global.h"
#include "mainwindow.h"
#include "utc_date_time.h"
#include "utils.h"


class UI_Mainwindow;



class UI_EDFhdrwindow : public QObject
{
  Q_OBJECT

public:
  UI_EDFhdrwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

QDialog      *EDFhdrDialog;

QVBoxLayout    *generalVBoxLayout1,
               *signalsVBoxLayout,
               *mainVBoxLayout1;

QHBoxLayout    *generalHBoxLayout1,
               *generalHBoxLayout2,
               *generalHBoxLayout3,
               *generalHBoxLayout4,
               *generalHBoxLayout5,
               *generalHBoxLayout6,
               *generalHBoxLayout7,
               *generalHBoxLayout8,
               *generalHBoxLayout9,
               *generalHBoxLayout10,
               *generalHBoxLayout11,
               *generalHBoxLayout12,
               *generalHBoxLayout13,
               *generalHBoxLayout14,
               *generalHBoxLayout15,
               *signalsHBoxLayout1,
               *mainHBoxLayout1;

QTabWidget   *tabholder;

QWidget      *tab1,
             *tab2;

QListWidget  *filelist;

QTableWidget *signallist;

QLabel       *label1,
             *label2,
             *label6,
             *label7,
             *label8,
             *label9,
             *label10,
             *label11,
             *label12,
             *label13,
             *label3,
             *label4,
             *label5,
             *label20,
             *label21,
             *label1a,
             *label2a,
             *label6a,
             *label7a,
             *label8a,
             *label9a,
             *label10a,
             *label11a,
             *label12a,
             *label13a,
             *label3a,
             *label4a,
             *label5a,
             *label20a,
             *label21a;

QPushButton  *pushButton1;


private slots:

void show_params(int);

};



#endif // SHOWEDFHDRORM1_H


