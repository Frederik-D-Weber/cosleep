/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 Teunis van Beelen
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


#ifndef UI_MORTARA2EDFFORM_H
#define UI_MORTARA2EDFFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QObject>
#include <QFileDialog>
#include <QCheckBox>
#include <QCursor>
#include <QDoubleSpinBox>
#include <QProgressDialog>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"
#include "edflib.h"
#include "xml.h"

#define MORTARA_MAX_CHNS 64


class UI_Mainwindow;


class UI_MortaraEDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_MortaraEDFwindow(QWidget *, char *recent_dir=NULL, char *save_dir=NULL);

  UI_Mainwindow  *mainwindow;

private:

QDialog       *myobjectDialog;

QTextEdit    *textEdit1;

QPushButton   *pushButton1,
              *pushButton2;

char *recent_opendir,
     *recent_savedir;

void enable_widgets(bool);

int chan_cnt,
    chan_offset[MORTARA_MAX_CHNS],
    chan_bits[MORTARA_MAX_CHNS],
    chan_units_per_mv[MORTARA_MAX_CHNS],
    chan_duration[MORTARA_MAX_CHNS],
    chan_sample_freq[MORTARA_MAX_CHNS],
    chan_decoded_len[MORTARA_MAX_CHNS],
    chan_sf_div,
    chan_sf,
    chan_sf_block,
    subject_gender;

char chan_format[MORTARA_MAX_CHNS][17],
     chan_name[MORTARA_MAX_CHNS][17],
     *chan_data_in[MORTARA_MAX_CHNS],
     *chan_data_out[MORTARA_MAX_CHNS],
     start_date_time[64],
     subject_name[512],
     device_name[512];


private slots:

void SelectFileButton();

};




#endif


