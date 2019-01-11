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




#ifndef ACTIVEFILESAVERFORM1_H
#define ACTIVEFILESAVERFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QString>


#include "global.h"
#include "mainwindow.h"



class UI_Mainwindow;



class UI_activeFileChooserWindow : public QObject
{
  Q_OBJECT

public:
  UI_activeFileChooserWindow(int *, QWidget *);

private:

QDialog      *chooserDialog;

QPushButton  *okButton,
             *cancelButton;

QListWidget  *filelist;

int *file_num;

UI_Mainwindow *mainwindow;


private slots:

void okButtonClicked();

};



#endif


