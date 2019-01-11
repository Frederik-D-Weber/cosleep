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




#ifndef SIGNAL_CHOOSERFORM1_H
#define SIGNAL_CHOOSERFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QListWidget>
#include <QListWidgetItem>
#include <QList>
#include <QDialog>
#include <QPushButton>
#include <QVariant>
#include <QMessageBox>

#include "global.h"
#include "mainwindow.h"
#include "viewcurve.h"
#include "adjustfiltersettings.h"
#include "ecg_filter.h"
#include "utils.h"




class UI_Mainwindow;
class ViewCurve;



class UI_SignalChooser : public QObject
{
  Q_OBJECT

public:
  UI_SignalChooser(QWidget *, int, int *sgnl_nr = NULL);

  UI_Mainwindow *mainwindow;


private:

QDialog      *signalchooser_dialog;

QPushButton  *CloseButton,
             *UpButton,
             *DownButton,
             *DeleteButton,
             *InvertButton;

QListWidget  *list;

int task,
    *signal_nr;

void strip_types_from_label(char *);

void load_signalcomps(void);

int get_selectionlist(int *);

private slots:

void call_sidemenu(QListWidgetItem *);
void signalUp();
void signalDown();
void signalDelete();
void signalInvert();

};



#endif // SIGNAL_CHOOSERFORM1_H


