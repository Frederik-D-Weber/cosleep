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



#include "plif_ecg_subtract_filter_dialog.h"



UI_PLIF_ECG_filter_dialog::UI_PLIF_ECG_filter_dialog(QWidget *w_parent)
{
  int i, n, s, sf=0;

  char str[256];

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  mainwindow = (UI_Mainwindow *)w_parent;

  plifecgfilterdialog = new QDialog;

  plifecgfilterdialog->setMinimumSize(620, 365);
  plifecgfilterdialog->setMaximumSize(620, 365);
  plifecgfilterdialog->setWindowTitle("Add a powerline interference filter");
  plifecgfilterdialog->setModal(true);
  plifecgfilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  plfLabel = new QLabel(plifecgfilterdialog);
  plfLabel->setGeometry(20, 90, 200, 25);
  plfLabel->setText("Powerline frequency");

  plfBox = new QComboBox(plifecgfilterdialog);
  plfBox->setGeometry(240, 90, 150, 25);
  plfBox->addItem(" 50 Hz");
  plfBox->addItem(" 60 Hz");

  listlabel = new QLabel(plifecgfilterdialog);
  listlabel->setGeometry(440, 20, 100, 25);
  listlabel->setText("Select signals:");

  list = new QListWidget(plifecgfilterdialog);
  list->setGeometry(440, 45, 160, 300);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);
  list->setToolTip("Only signals with a physical dimension V, mV or uV and\n"
                   "a samplerate of >= 500Hz and\n"
                   "an integer ratio to 50 or 60 Hz will be listed here.");

  CancelButton = new QPushButton(plifecgfilterdialog);
  CancelButton->setGeometry(300, 320, 100, 25);
  CancelButton->setText("&Close");

  ApplyButton = new QPushButton(plifecgfilterdialog);
  ApplyButton->setGeometry(20, 270, 100, 25);
  ApplyButton->setText("&Apply");
  ApplyButton->setVisible(false);

  helpButton = new QPushButton(plifecgfilterdialog);
  helpButton->setGeometry(20, 320, 100, 25);
  helpButton->setText("Help");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    sf = ((long long)mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].smp_per_record * TIME_DIMENSION) /
         mainwindow->signalcomp[i]->edfhdr->long_data_record_duration;

    if(sf < 500)  /* don't list signals that have low samplerate */
    {
      continue;
    }

    if((sf % 50) && (sf % 60))  /* don't list signals that can not be filtered */
    {
      continue;
    }

    strcpy(str, mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].physdimension);

    remove_trailing_spaces(str);

    if(strcmp(str, "uV") && strcmp(str, "mV") && strcmp(str, "V"))
    {
      continue;
    }

    item = new QListWidgetItem;
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      item->setText(mainwindow->signalcomp[i]->alias);
    }
    else
    {
      item->setText(mainwindow->signalcomp[i]->signallabel);
    }
    item->setData(Qt::UserRole, QVariant(i));
    list->addItem(item);
  }

  n = list->count();

  for(i=0; i<n; i++)
  {
    item = list->item(i);
    s = item->data(Qt::UserRole).toInt();

    if(mainwindow->signalcomp[s]->plif_ecg_filter != NULL)
    {
      plfBox->setCurrentIndex(mainwindow->signalcomp[s]->plif_ecg_subtract_filter_plf);

      item->setSelected(true);
    }
    else
    {
      item->setSelected(false);
    }
  }

  QObject::connect(ApplyButton,            SIGNAL(clicked()),                this,                SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton,           SIGNAL(clicked()),                plifecgfilterdialog, SLOT(close()));
  QObject::connect(list,                   SIGNAL(itemSelectionChanged()),   this,                SLOT(ApplyButtonClicked()));
  QObject::connect(plfBox,                 SIGNAL(currentIndexChanged(int)), this,                SLOT(ApplyButtonClicked()));
  QObject::connect(helpButton,             SIGNAL(clicked()),                this,                SLOT(helpbuttonpressed()));

  plifecgfilterdialog->exec();
}


void UI_PLIF_ECG_filter_dialog::ApplyButtonClicked()
{
  int i, s, n, sf, plf, err=0;

  char str[256];

  double dthreshold=0;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->plif_ecg_filter)
    {
      plif_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter);

      mainwindow->signalcomp[i]->plif_ecg_filter = NULL;
    }

    if(mainwindow->signalcomp[i]->plif_ecg_filter_sav)
    {
      plif_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter_sav);

      mainwindow->signalcomp[i]->plif_ecg_filter_sav = NULL;
    }
  }

  if(plfBox->currentIndex() == 0)
  {
    plf = 50;
  }
  else
  {
    plf = 60;
  }

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    sf = ((long long)mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record * TIME_DIMENSION) /
         mainwindow->signalcomp[s]->edfhdr->long_data_record_duration;

    if(sf % plf)
    {
      err = 1;
      sprintf(str, "Samplefrequency (%iHz) is not an integer multiple of the powerline frequency (%iHz)", sf, plf);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
      messagewindow.exec();
      break;
    }

    strcpy(str, mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].physdimension);

    remove_trailing_spaces(str);

    if(!strcmp(str, "uV"))
    {
      dthreshold = 5.0 / mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].bitvalue;
    }
    else if(!strcmp(str, "mV"))
      {
        dthreshold = 5e-3 / mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].bitvalue;
      }
      else if(!strcmp(str, "V"))
        {
          dthreshold = 5e-6 / mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].bitvalue;
        }

    mainwindow->signalcomp[s]->plif_ecg_filter = plif_create_subtract_filter(sf, plf, dthreshold);
    if(mainwindow->signalcomp[s]->plif_ecg_filter == NULL)
    {
      err = 1;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a powerline interference filter.");
      messagewindow.exec();
      break;
    }

    mainwindow->signalcomp[s]->plif_ecg_filter_sav = plif_create_subtract_filter(sf, plf, dthreshold);
    if(mainwindow->signalcomp[s]->plif_ecg_filter_sav == NULL)
    {
      err = 1;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a powerline interference filter.");
      messagewindow.exec();
      break;
    }

    mainwindow->signalcomp[s]->plif_ecg_subtract_filter_plf = plfBox->currentIndex();
  }

  if(err)
  {
    for(i=0; i<mainwindow->signalcomps; i++)
    {
      if(mainwindow->signalcomp[i]->plif_ecg_filter)
      {
        plif_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter);

        mainwindow->signalcomp[i]->plif_ecg_filter = NULL;
      }

      if(mainwindow->signalcomp[i]->plif_ecg_filter_sav)
      {
        plif_free_subtract_filter(mainwindow->signalcomp[i]->plif_ecg_filter_sav);

        mainwindow->signalcomp[i]->plif_ecg_filter_sav = NULL;
      }
    }
  }

  mainwindow->setup_viewbuf();
}


void UI_PLIF_ECG_filter_dialog::helpbuttonpressed()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html#ECG_PLIF_filter"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strcpy(p_path, "file:///");
  strcat(p_path, mainwindow->specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data());
  strcat(p_path, "\\EDFbrowser\\manual.html#ECG_PLIF_filter");
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}











