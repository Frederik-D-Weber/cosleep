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




#include "active_file_chooser.h"



UI_activeFileChooserWindow::UI_activeFileChooserWindow(int *file_nr, QWidget *mainwndw)
{
  int i;

  mainwindow = (UI_Mainwindow *)mainwndw;

  file_num = file_nr;

  *file_num = -1;

  chooserDialog = new QDialog;

  chooserDialog->setMinimumSize(800, 200);
  chooserDialog->setMaximumSize(800, 200);
  chooserDialog->setWindowTitle("Choose file");
  chooserDialog->setModal(true);
  chooserDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(chooserDialog);
  filelist->setGeometry(10, 10, 780, 110);
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  okButton = new QPushButton(chooserDialog);
  okButton->setGeometry(10, 140, 100, 25);
  okButton->setText("OK");

  cancelButton = new QPushButton(chooserDialog);
  cancelButton->setGeometry(690, 140, 100, 25);
  cancelButton->setText("Cancel");

  QObject::connect(cancelButton, SIGNAL(clicked()), chooserDialog, SLOT(close()));
  QObject::connect(okButton,     SIGNAL(clicked()), this,          SLOT(okButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  chooserDialog->exec();
}


void UI_activeFileChooserWindow::okButtonClicked()
{
  if(!mainwindow->files_open)
  {
    *file_num = -1;

    chooserDialog->close();

    return;
  }

  *file_num = filelist->currentRow();

  chooserDialog->close();
}














