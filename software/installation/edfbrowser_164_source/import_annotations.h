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




#ifndef IMPORTANNOTATIONSFORM1_H
#define IMPORTANNOTATIONSFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QFileDialog>
#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QProgressDialog>
#include <QCursor>
#include <QDesktopServices>
#include <QUrl>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "utc_date_time.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"
#include "xml.h"
#include "edflib.h"
#include "check_edf_file.h"
#include "edf_annotations.h"



class UI_Mainwindow;



struct import_annotations_var_block
{
  char separator[4];
  int format;
  int onsettimeformat;
  int onsetcolumn;
  int descriptioncolumn;
  int manualdescription;
  int useduration;
  int durationcolumn;
  char description[21];
  int datastartline;
  int dceventbittime;
  double triggerlevel;
  int ignoreconsecutive;
};



class UI_ImportAnnotationswindow : public QObject
{
  Q_OBJECT

public:

UI_ImportAnnotationswindow(QWidget *);

UI_Mainwindow  *mainwindow;

private:

int mal_formatted_lines,
    mal_formatted_line_nrs[32];

QDialog        *ImportAnnotsDialog;

QTabWidget     *tabholder;

QWidget        *tab[5];

QLabel         *SeparatorLabel,
               *OnsetColumnLabel,
               *DatastartLabel,
               *OnsetTimeLabel,
               *DCEventSignalLabel,
               *DCEventBitTimeLabel,
               *DCEventTriggerLevelLabel,
               *SampleTimeLabel,
               *importStandardLabel,
               *importAuxLabel;

QLineEdit      *SeparatorLineEdit,
               *DescriptionLineEdit;

QSpinBox       *OnsetColumnSpinBox,
               *DurationColumnSpinBox,
               *DescriptionColumnSpinBox,
               *DatastartSpinbox,
               *BitTimeSpinbox,
               *SampleTimeSpinbox;

QDoubleSpinBox *DCEventTriggerLevelSpinBox;

QComboBox      *RelativeTimeComboBox,
               *DCEventSignalComboBox;

QVBoxLayout    *asciiSettingsVBoxLayout,
               *DCEventVBoxLayout,
               *mitwfdbVBoxLayout1,
               *mainLayout;

QHBoxLayout    *asciiSettingsHBoxLayout1,
               *asciiSettingsHBoxLayout2,
               *asciiSettingsHBoxLayout3,
               *asciiSettingsHBoxLayout4,
               *asciiSettingsHBoxLayout5,
               *asciiSettingsHBoxLayout6,
               *asciiSettingsHBoxLayout7,
               *DCEventHBoxLayout1,
               *DCEventHBoxLayout2,
               *DCEventHBoxLayout3,
               *mitwfdbHBoxLayout1,
               *mitwfdbHBoxLayout2,
               *mitwfdbHBoxLayout3,
               *horLayout;

QRadioButton   *DescriptionColumnRadioButton,
               *UseManualDescriptionRadioButton;

QCheckBox      *IgnoreConsecutiveCheckBox,
               *DurationCheckBox,
               *importStandardCheckBox,
               *importAuxCheckBox;

QPushButton    *CloseButton,
               *ImportButton,
               *helpButton;

int tab_index_array[5];

int import_from_xml(void);
int import_from_ascii(void);
int import_from_edfplus(void);
int import_from_dcevent(void);
int import_from_mitwfdb(void);
int get_samplefreq_inf(void);

int get_onset_time_from_ascii(const char *, long long *, long long *, int);

private slots:

void ImportButtonClicked();
void descriptionRadioButtonClicked(bool);
void DCEventSignalChanged(int);
void DurationCheckBoxChanged(int);
void TabChanged(int);
void helpbuttonpressed();

};



#endif




