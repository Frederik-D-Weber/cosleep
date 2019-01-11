/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017, 2018 Teunis van Beelen
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




#ifndef ANNOTFILTER_FORM1_H
#define ANNOTFILTER_FORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QToolTip>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "edf_annot_list.h"
#include "utils.h"
#include "annotations_dock.h"



class UI_Mainwindow;

class UI_Annotationswindow;



class UI_AnnotFilterWindow : public QObject
{
  Q_OBJECT

public:
  UI_AnnotFilterWindow(QWidget *, struct annotationblock *, struct annot_filter_struct *, int);

  UI_Mainwindow *mainwindow;


private:

QDialog     *annot_filter_dialog;

QLabel      *annotLabel,
            *annotNameLabel,
            *t1Label,
            *t2Label;

QPushButton *CloseButton,
            *ApplyButton,
            *UndoButton;

QListWidget *list;

QSpinBox *t1_spinbox,
         *t2_spinbox;

QCheckBox *hide_in_list_checkbox,
          *hide_other_checkbox,
          *invert_checkbox;

int file_num;

char sel_annot_str[MAX_ANNOTATION_LEN + 1];

struct annotation_list *annot_list;

struct annotationblock *sel_annot_ptr;

struct annot_filter_struct *filter_params;

UI_Annotationswindow *annots_dock;

private slots:

void apply_filter();
void undo_filter();

};



#endif // ANNOTFILTER_FORM1_H










