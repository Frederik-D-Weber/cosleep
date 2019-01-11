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


#ifndef UI_HEADEREDITORFORM_H
#define UI_HEADEREDITORFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QTabWidget>
#include <QFileDialog>
#include <QString>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QLineEdit>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateEdit>
#include <QDate>
#include <QCheckBox>
#include <QMessageBox>
#include <QDateTimeEdit>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utils.h"



class UI_Mainwindow;



class UI_headerEditorWindow : public QDialog
{
  Q_OBJECT

public:
  UI_headerEditorWindow(QWidget *parent);

  UI_Mainwindow *mainwindow;

private:

QTabWidget     *tabholder;

QWidget        *tab1,
               *tab2;

QTableWidget *signallist;

QLabel       *label1,
             *label2,
             *label3,
             *label4,
             *label5,
             *label6,
             *label7,
             *label8,
             *label9,
             *label11,
             *label12,
             *fileNameLabel,
             *charsleft1Label,
             *charsleft2Label,
	     *startTimeDateLabel;

QLineEdit    *lineEdit1,
             *lineEdit2,
             *lineEdit3,
             *lineEdit4,
             *lineEdit5,
             *lineEdit6,
             *lineEdit7,
             *lineEdit8,
             *lineEdit9;

QComboBox    *comboBox1;

QCheckBox    *checkBox1;

QDateEdit    *dateEdit1;

QPushButton  *pushButton1,
             *pushButton2,
             *pushButton3,
             *helpButton;

QDateTimeEdit *startTimeDate;

int edfplus,
    bdfplus,
    edf,
    bdf,
    edfsignals,
    has_startdate,
    recordsize,
    datrecs;

char path[MAX_PATH_LENGTH],
     *hdr;

FILE *file;

private slots:

void open_file();
void save_hdr();
void read_header();
void closeEvent(QCloseEvent *);
void calculate_chars_left_name(int);
int calculate_chars_left_name(const QString &);
int calculate_chars_left_recording(const QString &);
void helpbuttonpressed();

};


#endif



