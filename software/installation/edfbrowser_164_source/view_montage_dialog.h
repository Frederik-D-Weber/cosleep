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




#ifndef VIEWMONTAGEFORM1_H
#define VIEWMONTAGEFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QDialog>
#include <QFileDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <QIcon>
#include <QColor>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mainwindow.h"
#include "xml.h"
#include "utils.h"



class UI_Mainwindow;



class UI_ViewMontagewindow : public QObject
{
  Q_OBJECT

public:
  UI_ViewMontagewindow(QWidget *parent);

  UI_Mainwindow *mainwindow;


private:

  QDialog      *ViewMontageDialog;

  QPushButton  *CloseButton,
               *SelectButton;

  QBoxLayout   *box;

  QHBoxLayout  *hbox;

  QTreeView    *tree;

  QStandardItemModel *t_model;

  char mtg_path[MAX_PATH_LENGTH],
       mtg_dir[MAX_PATH_LENGTH];

private slots:

  void SelectButtonClicked();

};



#endif // VIEWMONTAGEFORM1_H


