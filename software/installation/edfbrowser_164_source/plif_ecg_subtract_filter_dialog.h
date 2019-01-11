/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017, 2018 Teunis van Beelen
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



#ifndef ADD_PLIF_ECG_FILTERFORM1_H
#define ADD_PLIF_ECG_FILTERFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QDialog>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QString>
#include <QMessageBox>
#include <QVariant>
#include <QDesktopServices>
#include <QUrl>

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "plif_ecg_subtract_filter.h"
#include "utils.h"



class UI_Mainwindow;



class UI_PLIF_ECG_filter_dialog : public QObject
{
  Q_OBJECT

public:
  UI_PLIF_ECG_filter_dialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *plifecgfilterdialog;

QPushButton    *CancelButton,
               *ApplyButton,
               *helpButton;

QListWidget    *list;

QLabel         *listlabel,
               *plfLabel;

QComboBox      *plfBox;


private slots:

void ApplyButtonClicked();
void helpbuttonpressed();

};



#endif // ADD_PLIF_ECG_FILTERFORM1_H








