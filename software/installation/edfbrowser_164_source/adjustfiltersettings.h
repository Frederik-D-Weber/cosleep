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




#ifndef ADJUSTFILTERSETTINGS_H
#define ADJUSTFILTERSETTINGS_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QMessageBox>

#include "global.h"
#include "mainwindow.h"
#include "viewcurve.h"
#include "ravg_filter.h"
#include "utils.h"

#include "third_party/fidlib/fidlib.h"


class UI_Mainwindow;
class ViewCurve;



class AdjustFilterSettings : public QObject
{
  Q_OBJECT

public:

AdjustFilterSettings(struct signalcompblock *, QWidget *);


private:

int filter_nr,
    filter_cnt,
    type,
    model,
    order,
    size,
    brand[MAXFILTERS];

double frequency1,
       frequency2,
       ripple;

struct signalcompblock * signalcomp;

UI_Mainwindow  *mainwindow;

ViewCurve      *maincurve;

QDialog        *filtersettings_dialog;

QLabel         *label[5];

QComboBox      *filterbox,
               *stepsizebox;

QSpinBox       *orderbox;

QDoubleSpinBox *freq1box,
               *freq2box;

QPushButton    *CloseButton,
               *RemoveButton;

void update_filter(void);
void loadFilterSettings(void);


private slots:

void freqbox1valuechanged(double);
void freqbox2valuechanged(double);
void orderboxvaluechanged(int);
void stepsizeboxchanged(int);
void filterboxchanged(int);
void removeButtonClicked();

};



#endif // ADJUSTFILTERSETTINGS_H


