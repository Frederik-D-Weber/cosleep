/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#ifndef AVERAGER_FORM1_H
#define AVERAGER_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QComboBox>
#include <QList>
#include <QTime>
#include <QTimeEdit>
#include <QSpinBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "averager_curve_wnd.h"
#include "utils.h"



class UI_Mainwindow;



class UI_AveragerWindow : public QObject
{
  Q_OBJECT

public:
  UI_AveragerWindow(QWidget *, int, int);

  ~UI_AveragerWindow();

  UI_Mainwindow *mainwindow;


private:

QDialog     *averager_dialog;

QLabel      *annotLabel,
            *annotNameLabel,
            *signalLabel,
            *ratioLabel,
            *bufsizeLabel,
            *time1Label,
            *time2Label;

QPushButton *CloseButton,
            *StartButton;

QListWidget *list;

QComboBox   *ratioBox;

QTimeEdit    *timeEdit1,
             *timeEdit2;

QTime        time1,
             time2;

QSpinBox *avg_periodspinbox;

char annot_str[MAX_ANNOTATION_LEN + 1];

int signal_nr,
    file_num;

double *avgbuf;


void process_avg(struct signalcompblock *);

private slots:

void startButtonClicked();

};



#endif // AVERAGER_FORM1_H


