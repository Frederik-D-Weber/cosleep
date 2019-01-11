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


#ifndef UI_BI98002EDFFORM_H
#define UI_BI98002EDFFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QFileDialog>
#include <QCursor>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QProgressDialog>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "edflib.h"
#include "utils.h"



class UI_BI98002EDFwindow : public QObject
{
  Q_OBJECT

public:

  UI_BI98002EDFwindow(char *recent_dir=NULL, char *save_dir=NULL);

private:

QPushButton  *pushButton1,
             *pushButton2;

QDialog      *myobjectDialog;

char  *recent_opendir,
      *recent_savedir;

private slots:

void SelectFileButton();

};




#endif


