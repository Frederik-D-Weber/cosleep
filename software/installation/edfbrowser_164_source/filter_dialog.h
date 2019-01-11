/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#ifndef ADD_FILTERFORM1_H
#define ADD_FILTERFORM1_H



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

#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "filter.h"
#include "filtercurve.h"
#include "utils.h"
#include "ravg_filter.h"

#include "third_party/fidlib/fidlib.h"



class UI_Mainwindow;



class UI_FilterDialog : public QObject
{
  Q_OBJECT

public:
  UI_FilterDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *filterdialog;

QPushButton    *CancelButton,
               *ApplyButton;

QListWidget    *list;

QComboBox      *typebox,
               *modelbox;

QDoubleSpinBox *freqbox,
               *ripplebox,
               *freq2box;

QSpinBox       *orderbox;

QLabel         *typeboxlabel,
               *freqboxlabel,
               *freqbox2label,
               *orderboxlabel,
               *modelboxlabel,
               *listlabel,
               *orderlabel,
               *ordervaluelabel;

FilterCurve    *curve1;

double array[400],
       array_pha[400],
       last_ripple;

int arraysize,
    last_order,
    last_qfactor,
    last_model,
    last_samples;

void updatecurve(void);

char *filter_spec,
     spec_str_1[256],
     spec_str_2[256];


private slots:

void ApplyButtonClicked();
void frequencyboxvaluechanged(double);
void orderboxvaluechanged(int);
void filtertypeboxvaluechanged(int);
void filtermodelboxvaluechanged(int);
void rippleboxvaluechanged(double);
void freq2boxvaluechanged(double);

};



#endif // ADD_FILTERFORM1_H


