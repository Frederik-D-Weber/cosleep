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




#ifndef EXPORTANNOTATIONSFORM1_H
#define EXPORTANNOTATIONSFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QFileDialog>
#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QString>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QPixmap>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edflib.h"
#include "utc_date_time.h"
#include "edf_annot_list.h"



class UI_Mainwindow;



struct export_annotations_var_block
{
  int separator;
  int format;
  int duration;
};



class UI_ExportAnnotationswindow : public QObject
{
  Q_OBJECT

public:
  UI_ExportAnnotationswindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

  QDialog      *ExportAnnotsDialog;

  QListWidget  *filelist;

  QPushButton  *CloseButton,
               *ExportButton;

  QRadioButton *CSVRadioButton,
               *EDFplusRadioButton,
               *XMLRadioButton,
               *mergeRadioButton,
               *selectRadioButton,
               *asciiSecondsRadioButton,
               *asciiISOtimeRadioButton,
               *asciiISOtimedateRadioButton,
               *asciiISOtimeFractionRadioButton,
               *asciiISOtimedateFractionRadioButton;

  QGroupBox    *formatGroupBox,
               *fileGroupBox,
               *asciiSettingsGroupBox;

  QVBoxLayout  *formatVBoxLayout,
               *fileVBoxLayout,
               *asciiSettingsVBoxLayout;

  QHBoxLayout  *asciiSettingsHBoxLayout;

  QComboBox    *separatorBox;

  QLabel       *separatorLabel;

  QCheckBox    *durationCheckBox;

private slots:

  void ExportButtonClicked();
  void outputformatRadioButtonClicked(bool);

};



#endif




