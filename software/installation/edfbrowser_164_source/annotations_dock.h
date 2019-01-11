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




#ifndef ANNOTATIONSFORM1_H
#define ANNOTATIONSFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QByteArray>
#include <QPalette>
#include <QTime>
#include <QTimeEdit>
#include <QString>
#include <QDialog>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QAction>
#include <QMessageBox>
#include <QVariant>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edit_annotation_dock.h"
#include "viewcurve.h"
#include "utils.h"
#include "averager_dialog.h"
#include "edf_annot_list.h"
#include "annotlist_filter_dialog.h"
#include "statistics_dialog.h"


class UI_Mainwindow;




class UI_Annotationswindow : public QObject
{
  Q_OBJECT

public:
  UI_Annotationswindow(int, QWidget *parent);

  UI_Mainwindow *mainwindow;

  QDockWidget  *docklist;

  QListWidget  *list;

  void updateList(void);

private:

  int file_num,
      relative,
      selected,
      invert_filter,
      hide_nk_triggers,
      hide_bs_triggers;

  QDialog *dialog1;

  QHBoxLayout *h_layout;

  QVBoxLayout *v_layout;

  QCheckBox *checkbox1,
            *checkbox2;

  QLabel *label1;

  QLineEdit *lineedit1;

  QAction *show_between_act,
          *average_annot_act,
          *hide_annot_act,
          *unhide_annot_act,
          *hide_same_annots_act,
          *unhide_same_annots_act,
          *unhide_all_annots_act,
          *hide_all_NK_triggers_act,
          *hide_all_BS_triggers_act,
          *unhide_all_NK_triggers_act,
          *unhide_all_BS_triggers_act,
          *filt_ival_time_act,
          *show_stats_act;

private slots:

  void annotation_selected(QListWidgetItem *, int centered=1);
  void hide_editdock(bool);
  void checkbox1_clicked(int);
  void checkbox2_clicked(int);
  void show_between(bool);
  void average_annot(bool);
  void hide_annot(bool);
  void unhide_annot(bool);
  void hide_same_annots(bool);
  void unhide_same_annots(bool);
  void unhide_all_annots(bool);
  void filter_edited(const QString);
  void hide_all_NK_triggers(bool);
  void hide_all_BS_triggers(bool);
  void unhide_all_NK_triggers(bool);
  void unhide_all_BS_triggers(bool);
  void filt_ival_time(bool);
  void show_stats(bool);
};



#endif // ANNOTATIONSFORM1_H


