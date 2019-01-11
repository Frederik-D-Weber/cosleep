/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "spike_filter_dialog.h"



UI_SpikeFilterDialog::UI_SpikeFilterDialog(QWidget *w_parent)
{
  int i, n, s, sf=0;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;


  mainwindow = (UI_Mainwindow *)w_parent;

  spikefilterdialog = new QDialog;

  spikefilterdialog->setMinimumSize(620, 365);
  spikefilterdialog->setMaximumSize(620, 365);
  spikefilterdialog->setWindowTitle("Add a spike filter");
  spikefilterdialog->setModal(true);
  spikefilterdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  velocityLabel = new QLabel(spikefilterdialog);
  velocityLabel->setGeometry(20, 45, 60, 25);
  velocityLabel->setText("Velocity");

  velocitySpinBox = new QDoubleSpinBox(spikefilterdialog);
  velocitySpinBox->setGeometry(90, 45, 230, 25);
  velocitySpinBox->setDecimals(6);
  velocitySpinBox->setSuffix(" units/(0.25 mSec)");
  velocitySpinBox->setMinimum(0.000001);
  velocitySpinBox->setMaximum(1E10);
  velocitySpinBox->setValue(1000.0);

  holdOffLabel = new QLabel(spikefilterdialog);
  holdOffLabel->setGeometry(20, 90, 60, 25);
  holdOffLabel->setText("Hold-off");

  holdOffSpinBox = new QSpinBox(spikefilterdialog);
  holdOffSpinBox->setGeometry(90, 90, 230, 25);
  holdOffSpinBox->setSuffix(" milliSec");
  holdOffSpinBox->setMinimum(10);
  holdOffSpinBox->setMaximum(1000);
  holdOffSpinBox->setValue(100);

  listlabel = new QLabel(spikefilterdialog);
  listlabel->setGeometry(440, 20, 100, 25);
  listlabel->setText("Select signals:");

  list = new QListWidget(spikefilterdialog);
  list->setGeometry(440, 45, 160, 300);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CancelButton = new QPushButton(spikefilterdialog);
  CancelButton->setGeometry(300, 320, 100, 25);
  CancelButton->setText("&Close");

  ApplyButton = new QPushButton(spikefilterdialog);
  ApplyButton->setGeometry(20, 320, 100, 25);
  ApplyButton->setText("&Apply");
  ApplyButton->setVisible(false);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    sf = ((long long)mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[0]].smp_per_record * TIME_DIMENSION) /
         mainwindow->signalcomp[i]->edfhdr->long_data_record_duration;

    if(sf < 4000)
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

    if(mainwindow->signalcomp[s]->spike_filter != NULL)
    {
      velocitySpinBox->setValue(mainwindow->signalcomp[s]->spike_filter_velocity);

      holdOffSpinBox->setValue(mainwindow->signalcomp[s]->spike_filter_holdoff);

      item->setSelected(true);
    }
    else
    {
      item->setSelected(false);
    }
  }

  QObject::connect(ApplyButton,     SIGNAL(clicked()),              this,              SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton,    SIGNAL(clicked()),              spikefilterdialog, SLOT(close()));
  QObject::connect(list,            SIGNAL(itemSelectionChanged()), this,              SLOT(ApplyButtonClicked()));
  QObject::connect(velocitySpinBox, SIGNAL(valueChanged(double)),   this,              SLOT(ApplyButtonClicked()));
  QObject::connect(holdOffSpinBox,  SIGNAL(valueChanged(int)),      this,              SLOT(ApplyButtonClicked()));

  spikefilterdialog->exec();
}


void UI_SpikeFilterDialog::ApplyButtonClicked()
{
  int i, s, n, sf, holdoff;

  double velocity;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->spike_filter)
    {
      free_spike_filter(mainwindow->signalcomp[i]->spike_filter);

      mainwindow->signalcomp[i]->spike_filter = NULL;
    }
  }

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    velocity = velocitySpinBox->value() /
               mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].bitvalue;

    if(velocity < 0) velocity *= -1;

    holdoff = holdOffSpinBox->value();

    sf = ((long long)mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record * TIME_DIMENSION) /
         mainwindow->signalcomp[s]->edfhdr->long_data_record_duration;

    mainwindow->signalcomp[s]->spike_filter = create_spike_filter(sf, velocity, holdoff, NULL);
    if(mainwindow->signalcomp[s]->spike_filter == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a spike filter.");
      messagewindow.exec();
    }

    mainwindow->signalcomp[s]->spike_filter_velocity = velocitySpinBox->value();

    mainwindow->signalcomp[s]->spike_filter_holdoff = holdOffSpinBox->value();
  }

  mainwindow->setup_viewbuf();
}










