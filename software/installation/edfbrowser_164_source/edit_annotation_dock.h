/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#ifndef ANNOTATION_EDIT_FORM1_H
#define ANNOTATION_EDIT_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDockWidget>
#include <QByteArray>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "annotations_dock.h"
#include "popup_save_cancelwindow.h"
#include "edf_annot_list.h"
#include "utils.h"



class UI_Mainwindow;




class UI_AnnotationEditwindow : public QObject
{
  Q_OBJECT

public:
  UI_AnnotationEditwindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

  QDockWidget  *dockedit;

  void annotEditSetOnset(long long);

  long long annotEditGetOnset(void);

  void annotEditSetDuration(long long);

  void set_selected_annotation(int, int);

  void set_selected_annotation(struct annotationblock *);

private:

  int file_num,
      annot_num;

  QDialog *annot_edit_dialog;

  QLabel *onsetLabel,
         *durationLabel,
         *descriptionLabel;

  QLineEdit *annot_descript_lineEdit;

  QTimeEdit *onset_timeEdit;

  QSpinBox  *onset_daySpinbox;

  QDoubleSpinBox *duration_spinbox;

  QPushButton *modifybutton,
              *deletebutton,
              *createbutton;

  QComboBox *posNegTimebox;

private slots:

  void modifyButtonClicked();
  void deleteButtonClicked();
  void createButtonClicked();

  void open_close_dock(bool);
};



#endif




