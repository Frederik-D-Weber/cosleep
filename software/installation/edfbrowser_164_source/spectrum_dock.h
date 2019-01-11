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



#ifndef SPECTRUMANALYZERDOCK_H
#define SPECTRUMANALYZERDOCK_H



#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QDockWidget>
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


struct spectrumdocksettings{
                             int signalnr;
                             int span;
                             int center;
                             int amp;
                             int log_min_sl;
                             int wheel;
                             int log;
                             int sqrt;
                             int colorbar;
                             double maxvalue;
                             double maxvalue_sqrt;
                             double maxvalue_vlog;
                             double maxvalue_sqrt_vlog;
                             double minvalue_vlog;
                             double minvalue_sqrt_vlog;
                           };




class UI_SpectrumDockWindow : public QObject
{
  Q_OBJECT

public:
  UI_SpectrumDockWindow(QWidget *parent);

  ~UI_SpectrumDockWindow();

  UI_Mainwindow *mainwindow;

  QDialog *SpectrumDialog;

  QDockWidget *dock;

  struct signalcompblock *signalcomp;

  struct spectrum_markersblock *spectrum_color;

void rescan();
void init(int);
void clear();
void getsettings(struct spectrumdocksettings *);
void setsettings(struct spectrumdocksettings);


private:

  QWidget *parentwindow;

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

  QCheckBox *sqrtButton,
            *vlogButton,
            *colorBarButton;

  QSpinBox *dftsz_spinbox;

  QComboBox *windowBox;

  UI_Flywheel *flywheel1;

  int samples,
      buf1_sz,
      dftblocksize,
      window_type,
      steps,
      spectrumdock_sqrt,
      spectrumdock_vlog,
      dashboard,
      flywheel_value,
      init_maxvalue,
      signal_nr,
      set_settings;

  volatile int busy;

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

  struct fft_wrap_settings_struct *fft_data;

  struct spectrumdocksettings settings;



private slots:

void update_curve();
void sliderMoved(int);
void sqrtButtonClicked(bool);
void vlogButtonClicked(bool);
void colorBarButtonClicked(bool);
void print_to_txt();
void setdashboard();
void update_flywheel(int);
void dftsz_value_changed(int);
void windowBox_changed(int);

void open_close_dock(bool);
};


#endif



















