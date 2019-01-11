/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#ifndef ZSCORE_FORM1_H
#define ZSCORE_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QProgressDialog>
#include <QList>
#include <QTime>
#include <QTimeEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "filteredblockread.h"
#include "signalcurve.h"
#include "annotations_dock.h"
#include "filter.h"
#include "spike_filter.h"
#include "ravg_filter.h"

#include "third_party/kiss_fft/kiss_fftr.h"




class UI_Mainwindow;



class UI_ZScoreWindow : public QObject
{
  Q_OBJECT

public:
  UI_ZScoreWindow(QWidget *, UI_ZScoreWindow **, int, int);
  ~UI_ZScoreWindow();

  UI_Mainwindow *mainwindow;

  QDialog     *zscore_dialog;


private:

QHBoxLayout *hlayout1,
            *hlayout2,
            *hlayout3,
            *hlayout4,
            *hlayout5;

QVBoxLayout *vlayout2,
            *vlayout3;

QLabel      *crossoverbinLabel,
            *thresholdLabel,
            *pagelenLabel,
            *hysteresisLabel;
//            *errordetectionLabel;

QPushButton *CloseButton,
            *StartButton,
            *get_annotButton,
            *defaultButton,
            *jumpButton,
            *addTraceButton;

QSpinBox    *pagelenSpinbox;
//            *errordetectionSpinbox;

QDoubleSpinBox *thresholdSpinbox,
               *crossoverSpinbox,
               *hysteresisSpinbox;

QRadioButton *epochRadioButton,
             *pageRadioButton,
             *wakesleepRadioButton;

SignalCurve *curve1;

UI_ZScoreWindow **zscoredialog;

QAction     *shift_page_right_Act,
            *shift_page_left_Act;

int signalnr,
    zscore_page_len,
    avg_majority,
    epoch_cntr,
    zscore_pages,
    zscore_wake,
    *zscore_sleepstage_buf,
    *zscore_error_buf,
    plot_type,
    zscore_dialog_is_destroyed,
    zscore_dialognumber,
    zscore_error_detection;

double *zscore_epoch_buf,
       *zscore_page_buf,
       crossoverfreq,
       z_threshold,
       z_hysteresis;


private slots:

void startButtonClicked();
void get_annotationsButtonClicked();
void jumpButtonClicked();
void ZscoreDialogDestroyed(QObject *);
void RadioButtonsClicked(bool);
void defaultButtonClicked();
void markersMoved();
void addTraceButtonClicked();
void shift_page_right();
void shift_page_left();

};



#endif // ZSCORE_FORM1_H


