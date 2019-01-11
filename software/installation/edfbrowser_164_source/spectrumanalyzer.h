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



#ifndef SPECTRUMANALYZER_H
#define SPECTRUMANALYZER_H


#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QSlider>
#include <QLabel>
#include <QProgressDialog>
#include <QCheckBox>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QThread>
#include <QSpinBox>
#include <QComboBox>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "mainwindow.h"
#include "signalcurve.h"
#include "utils.h"
#include "filter.h"
#include "spike_filter.h"
#include "ravg_filter.h"
#include "flywheel.h"
#include "fft_wrap.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;




class UI_FreqSpectrumWindow : public QThread
{
  Q_OBJECT

public:
  UI_FreqSpectrumWindow(struct signalcompblock *, char *, UI_FreqSpectrumWindow **, int, QWidget *);

  ~UI_FreqSpectrumWindow();

  UI_Mainwindow *mainwindow;

  QDialog *SpectrumDialog;

  struct signalcompblock *signalcomp;

private:

  QHBoxLayout *hlayout1,
              *hlayout2,
              *hlayout3,
              *hlayout4;

  QVBoxLayout *vlayout1,
              *vlayout2,
              *vlayout3;

  SignalCurve *curve1;

  QTimer *t1;

  QSlider *amplitudeSlider,
          *spanSlider,
          *centerSlider,
          *log_minslider;

  QLabel *spanLabel,
         *centerLabel,
         *amplitudeLabel,
         *dftsz_label;

  QCheckBox *sqrtCheckBox,
            *VlogCheckBox,
            *BWCheckBox;

  QSpinBox *dftsz_spinbox;

  QComboBox *windowBox;

  UI_Flywheel *flywheel1;

  int samples,
      dftblocksize,
      window_type,
      spectrumdialog_is_destroyed,
      class_is_deleted,
      spectrumdialognumber,
      flywheel_value,
      first_run,
      fft_inputbufsize;


  volatile int busy,
               malloc_err;

  double samplefreq,
         freqstep,
         maxvalue,
         maxvalue_sqrt,
         maxvalue_vlog,
         maxvalue_sqrt_vlog,
         minvalue_vlog,
         minvalue_sqrt_vlog,
         *buf1,
         *buf2,
         *buf3,
         *buf4,
         *buf5;

  char *viewbuf,
       signallabel[512],
       physdimension[9];

  UI_FreqSpectrumWindow **spectrumdialog;

  struct fft_wrap_settings_struct *fft_data;

  void run();

private slots:

void update_curve();
void sliderMoved(int);
void SpectrumDialogDestroyed(QObject *);
void print_to_txt();
void update_flywheel(int);
void thr_finished_func();
void dftsz_value_changed(int);
void windowBox_changed(int);

};


#endif



















