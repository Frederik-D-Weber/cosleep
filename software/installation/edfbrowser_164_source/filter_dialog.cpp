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


/*

type:

0 : highpass
1 : lowpass
2 : notch
3 : bandpass
4 : bandstop

model:

0 : Butterworth
1 : Chebyshev
2 : Bessel
3 : moving average

0 : resonator

*/

#define FILTERTYPE_HIGHPASS  0
#define FILTERTYPE_LOWPASS   1
#define FILTERTYPE_NOTCH     2
#define FILTERTYPE_BANDPASS  3
#define FILTERTYPE_BANDSTOP  4

#define FILTERMODEL_BUTTERWORTH    0
#define FILTERMODEL_CHEBYSHEV      1
#define FILTERMODEL_BESSEL         2
#define FILTERMODEL_MOVINGAVERAGE  3


#include "filter_dialog.h"



UI_FilterDialog::UI_FilterDialog(QWidget *w_parent)
{
  int i;

  QListWidgetItem *item;


  arraysize = 400;

  last_order = 1;

  last_samples = 16;

  last_qfactor = 20;

  last_model = 0;

  last_ripple = -1.0;

  mainwindow = (UI_Mainwindow *)w_parent;

  filterdialog = new QDialog;

  filterdialog->setMinimumSize(620, 365);
  filterdialog->setMaximumSize(620, 365);
  filterdialog->setWindowTitle("Add a filter");
  filterdialog->setModal(true);
  filterdialog->setAttribute(Qt::WA_DeleteOnClose, true);

  typeboxlabel = new QLabel(filterdialog);
  typeboxlabel->setGeometry(20, 20, 120, 25);
  typeboxlabel->setText("Type");

  freqboxlabel = new QLabel(filterdialog);
  freqboxlabel->setGeometry(160, 20, 120, 25);
  freqboxlabel->setText("Frequency");

  freqbox2label = new QLabel(filterdialog);
  freqbox2label->setGeometry(300, 20, 120, 25);
  freqbox2label->setText("Frequency 2");
  freqbox2label->setVisible(false);

  orderboxlabel = new QLabel(filterdialog);
  orderboxlabel->setGeometry(160, 80, 120, 25);
  orderboxlabel->setText("Order");

  modelboxlabel = new QLabel(filterdialog);
  modelboxlabel->setGeometry(20, 80, 120, 25);
  modelboxlabel->setText("Model");

  orderlabel = new QLabel(filterdialog);
  orderlabel->setGeometry(300, 80, 120, 25);
  orderlabel->setText("Slope roll-off:");

  ordervaluelabel = new QLabel(filterdialog);
  ordervaluelabel->setGeometry(300, 105, 120, 25);
  ordervaluelabel->setText("6 dB / octave");

  typebox = new QComboBox(filterdialog);
  typebox->setGeometry(20, 45, 120, 25);
  typebox->addItem("Highpass");
  typebox->addItem("Lowpass");
  typebox->addItem("Notch");
  typebox->addItem("Bandpass");
  typebox->addItem("Bandstop");

  freqbox = new QDoubleSpinBox(filterdialog);
  freqbox->setGeometry(160, 45, 120, 25);
  freqbox->setDecimals(6);
  freqbox->setSuffix(" Hz");
  freqbox->setMinimum(0.0001);
  freqbox->setMaximum(100000000.0);
  freqbox->setValue(1.0);

  freq2box = new QDoubleSpinBox(filterdialog);
  freq2box->setGeometry(300, 45, 120, 25);
  freq2box->setDecimals(6);
  freq2box->setSuffix(" Hz");
  freq2box->setMinimum(0.0001);
  freq2box->setMaximum(100000000.0);
  freq2box->setValue(2.0);
  freq2box->setVisible(false);

  orderbox = new QSpinBox(filterdialog);
  orderbox->setGeometry(160, 105, 120, 25);
  orderbox->setMinimum(1);
  orderbox->setMaximum(8);
  orderbox->setSingleStep(1);
  orderbox->setValue(1);

  modelbox = new QComboBox(filterdialog);
  modelbox->setGeometry(20, 105, 120, 25);
  modelbox->addItem("Butterworth");
  modelbox->addItem("Chebyshev");
  modelbox->addItem("Bessel");
  modelbox->addItem("Moving Average");

  ripplebox = new QDoubleSpinBox(filterdialog);
  ripplebox->setGeometry(300, 105, 120, 25);
  ripplebox->setDecimals(6);
  ripplebox->setSuffix(" dB");
  ripplebox->setMinimum(0.1);
  ripplebox->setMaximum(6.0);
  ripplebox->setValue(1.0);
  ripplebox->setVisible(false);

  listlabel = new QLabel(filterdialog);
  listlabel->setGeometry(440, 20, 100, 25);
  listlabel->setText("Select signals:");

  list = new QListWidget(filterdialog);
  list->setGeometry(440, 45, 160, 300);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  CancelButton = new QPushButton(filterdialog);
  CancelButton->setGeometry(300, 320, 100, 25);
  CancelButton->setText("&Cancel");

  ApplyButton = new QPushButton(filterdialog);
  ApplyButton->setGeometry(20, 320, 100, 25);
  ApplyButton->setText("&Apply");

  curve1 = new FilterCurve(filterdialog);
  curve1->setGeometry(20, 145, 400, 150);
  curve1->setH_RasterSize(10);
  curve1->setV_RasterSize(14);

  for(i=0; i<mainwindow->signalcomps; i++)
  {
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

  list->setCurrentRow(0);

  QObject::connect(ApplyButton,  SIGNAL(clicked()),                this,         SLOT(ApplyButtonClicked()));
  QObject::connect(CancelButton, SIGNAL(clicked()),                filterdialog, SLOT(close()));
  QObject::connect(freqbox,      SIGNAL(valueChanged(double)),     this,         SLOT(frequencyboxvaluechanged(double)));
  QObject::connect(typebox,      SIGNAL(currentIndexChanged(int)), this,         SLOT(filtertypeboxvaluechanged(int)));
  QObject::connect(orderbox,     SIGNAL(valueChanged(int)),        this,         SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox,     SIGNAL(currentIndexChanged(int)), this,         SLOT(filtermodelboxvaluechanged(int)));
  QObject::connect(ripplebox,    SIGNAL(valueChanged(double)),     this,         SLOT(rippleboxvaluechanged(double)));
  QObject::connect(freq2box,     SIGNAL(valueChanged(double)),     this,         SLOT(freq2boxvaluechanged(double)));

  updatecurve();

  filterdialog->exec();
}



void UI_FilterDialog::updatecurve(void)
{
  int i,
      type=0,
      rate=3200,
      order=1,
      model=0;

  double frequency=150.0,
         ripple=1.0,
         frequency2=200.0;

  char *err;

  FidFilter *ff;


  order = orderbox->value();

  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    for(i=0; i<arraysize; i++)
    {
      array[i] = ravg_filter_response(type, order, (double)i / 800.0);
    }

    curve1->drawCurve(array, arraysize, 1.4, 0.0);

    return;
  }

  ripple = -(ripplebox->value());

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    frequency = frequency2 * (freqbox->value() / freq2box->value());
  }

  spec_str_1[0] = 0;

  if(type == FILTERTYPE_HIGHPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      sprintf(spec_str_1, "HpBu%i/%f", order, frequency);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      sprintf(spec_str_1, "HpCh%i/%f/%f", order, ripple, frequency);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      sprintf(spec_str_1, "HpBe%i/%f", order, frequency);
    }
  }

  if(type == FILTERTYPE_LOWPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      sprintf(spec_str_1, "LpBu%i/%f", order, frequency);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      sprintf(spec_str_1, "LpCh%i/%f/%f", order, ripple, frequency);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      sprintf(spec_str_1, "LpBe%i/%f", order, frequency);
    }
  }

  if(type == FILTERTYPE_NOTCH)
  {
    sprintf(spec_str_1, "BsRe/%i/%f", order, frequency);
  }

  if(type == FILTERTYPE_BANDPASS)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      sprintf(spec_str_1, "BpBu%i/%f-%f", order, frequency, frequency2);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      sprintf(spec_str_1, "BpCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      sprintf(spec_str_1, "BpBe%i/%f-%f", order, frequency, frequency2);
    }
  }

  if(type == FILTERTYPE_BANDSTOP)
  {
    if(model == FILTERMODEL_BUTTERWORTH)
    {
      sprintf(spec_str_1, "BsBu%i/%f-%f", order, frequency, frequency2);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      sprintf(spec_str_1, "BsCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      sprintf(spec_str_1, "BsBe%i/%f-%f", order, frequency, frequency2);
    }
  }

  strcpy(spec_str_2, spec_str_1);

  filter_spec = spec_str_2;

  err = fid_parse(rate, &filter_spec, &ff);

  if(err != NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", err);
    messagewindow.exec();
    free(err);
    return;
  }

  for(i=0; i<arraysize; i++)
  {
    array[i] = fid_response_pha(ff, (double)i / (double)rate, array_pha + i);
  }

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    curve1->drawCurve(array, arraysize, 1.4, 0.0, array_pha, arraysize, 0.0, 1.0, frequency, frequency2);
  }
  else
  {
    curve1->drawCurve(array, arraysize, 1.4, 0.0, array_pha, arraysize, 0.0, 1.0, frequency);
  }

  free(ff);
}



void UI_FilterDialog::rippleboxvaluechanged(double value)
{
  last_ripple = value;

  updatecurve();
}



void UI_FilterDialog::filtermodelboxvaluechanged(int model)
{
  int type;


  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::disconnect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  type = typebox->currentIndex();

  last_model = model;

  freqboxlabel->setText("Frequency");
  freqbox->setVisible(true);

  if(type != FILTERTYPE_NOTCH)
  {
    orderboxlabel->setText("Order");
    orderboxlabel->setVisible(true);

    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      if(model == FILTERMODEL_BESSEL)
      {
        orderbox->setMaximum(10);
      }
      else
      {
        orderbox->setMaximum(16);
      }
    }
    else
    {
      orderbox->setMaximum(8);
    }
    orderbox->setSingleStep(1);
    orderbox->setMinimum(1);
    orderbox->setValue(last_order);
    orderbox->setVisible(true);
  }

  if(model == FILTERMODEL_BUTTERWORTH)
  {
    ripplebox->setVisible(false);
    orderlabel->setText("Slope roll-off:");
    orderlabel->setVisible(true);
    ordervaluelabel->setText(QString::number(6 * orderbox->value(), 'f', 0).append(" dB / octave"));
    ordervaluelabel->setVisible(true);
  }

  if(model == FILTERMODEL_CHEBYSHEV)
  {
    orderlabel->setText("passband ripple");
    orderlabel->setVisible(true);
    ordervaluelabel->setVisible(false);
    ripplebox->setVisible(true);
  }

  if(model == FILTERMODEL_BESSEL)
  {
    ripplebox->setVisible(false);
    ordervaluelabel->setVisible(false);
    orderlabel->setVisible(false);
  }

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    orderlabel->setVisible(false);
    ordervaluelabel->setVisible(false);
    freqboxlabel->setVisible(false);
    freqbox2label->setVisible(false);
    freqbox->setVisible(false);
    ripplebox->setVisible(false);
    orderboxlabel->setText("Samples");
    orderbox->setMaximum(10000);
    orderbox->setSingleStep(1);
    orderbox->setMinimum(2);
    orderbox->setValue(last_samples);
  }

  updatecurve();

  QObject::connect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));
}



void UI_FilterDialog::orderboxvaluechanged(int order)
{
  int type,
      model;

  char str[256];


  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  if(type == FILTERTYPE_NOTCH)
  {
    sprintf(str,"%f Hz", freqbox->value() / orderbox->value());
    remove_trailing_zeros(str);
    ordervaluelabel->setText(str);
    ordervaluelabel->setVisible(true);

    last_qfactor = order;
  }
  else
  {
    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      last_order = order / 2;
    }
    else
    {
      if(model == FILTERMODEL_MOVINGAVERAGE)
      {
        last_samples = order;
      }
      else
      {
        last_order = order;
      }
    }

    if(model == FILTERMODEL_BUTTERWORTH)
    {
      orderlabel->setText("Slope roll-off:");
      orderlabel->setVisible(true);
      ordervaluelabel->setText(QString::number(6 * last_order, 'f', 0).append(" dB / octave"));
      ordervaluelabel->setVisible(true);
    }

    if(model == FILTERMODEL_CHEBYSHEV)
    {
      orderlabel->setText("passband ripple");
      orderlabel->setVisible(true);
      ordervaluelabel->setVisible(false);
    }

    if(model == FILTERMODEL_BESSEL)
    {
      ripplebox->setVisible(false);
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
    }

    if(model == FILTERMODEL_MOVINGAVERAGE)
    {
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
      freqboxlabel->setVisible(false);
    }
  }

  updatecurve();
}



void UI_FilterDialog::filtertypeboxvaluechanged(int type)
{
  char str[256];

  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::disconnect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  if(type == FILTERTYPE_NOTCH)
  {
    ripplebox->setVisible(false);
    orderboxlabel->setText("Notch Q-factor");
    orderboxlabel->setVisible(true);
    orderbox->setMaximum(100);
    orderbox->setSingleStep(1);
    orderbox->setMinimum(3);
    orderbox->setValue(last_qfactor);
    orderbox->setVisible(true);
    orderlabel->setText("-3 dB bandwidth:");
    sprintf(str,"%f Hz", freqbox->value() / orderbox->value());
    remove_trailing_zeros(str);
    ordervaluelabel->setText(str);
    ordervaluelabel->setVisible(true);
    modelbox->clear();
    modelbox->addItem("Resonator");
    modelbox->setVisible(true);
  }
  else
  {
    orderboxlabel->setText("Order");
    orderboxlabel->setVisible(true);
    if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
    {
      if(last_model == FILTERMODEL_BESSEL)
      {
        orderbox->setMaximum(10);
      }
      else
      {
        orderbox->setMaximum(16);
      }
      orderbox->setSingleStep(2);
      orderbox->setMinimum(2);
      orderbox->setValue(last_order * 2);
      orderbox->setVisible(true);
    }
    else
    {
      orderbox->setMaximum(8);
      orderbox->setSingleStep(1);
      orderbox->setMinimum(1);
      orderbox->setValue(last_order);
      orderbox->setVisible(true);
    }

    if(last_model == FILTERMODEL_MOVINGAVERAGE)
    {
      if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS))
      {
        ripplebox->setVisible(false);
        ordervaluelabel->setVisible(false);
        orderlabel->setVisible(false);
        ordervaluelabel->setVisible(false);
        orderboxlabel->setText("Samples");
        orderboxlabel->setVisible(true);
        orderbox->setMaximum(10000);
        orderbox->setSingleStep(1);
        orderbox->setMinimum(2);
        orderbox->setValue(last_samples);
        orderbox->setVisible(true);
      }
      else
      {
        last_model = FILTERMODEL_BUTTERWORTH;
      }
    }

    if(last_model == FILTERMODEL_BUTTERWORTH)
    {
      ripplebox->setVisible(false);
      orderlabel->setText("Slope roll-off:");
      orderlabel->setVisible(true);
      ordervaluelabel->setText(QString::number(6 * last_order, 'f', 0).append(" dB / octave"));
      ordervaluelabel->setVisible(true);
    }

    if(last_model == FILTERMODEL_CHEBYSHEV)
    {
      ordervaluelabel->setVisible(false);
      ripplebox->setVisible(true);
      orderlabel->setText("passband ripple");
      orderlabel->setVisible(true);
    }

    if(last_model == FILTERMODEL_BESSEL)
    {
      ripplebox->setVisible(false);
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
    }

    modelbox->clear();
    modelbox->addItem("Butterworth");
    modelbox->addItem("Chebyshev");
    modelbox->addItem("Bessel");
    if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS))
    {
      modelbox->addItem("Moving Average");
    }
    modelbox->setCurrentIndex(last_model);
  }

  if((type == FILTERTYPE_BANDPASS) || (type == FILTERTYPE_BANDSTOP))
  {
    freqboxlabel->setText("Frequency 1");
    freqboxlabel->setVisible(true);
    freqbox2label->setText("Frequency 2");
    freqbox2label->setVisible(true);
    freqbox->setVisible(true);
    freq2box->setVisible(true);
  }
  else
  {
    freqbox2label->setVisible(false);
    freq2box->setVisible(false);

    if(last_model == FILTERMODEL_MOVINGAVERAGE)
    {
      orderlabel->setVisible(false);
      ordervaluelabel->setVisible(false);
      freqboxlabel->setVisible(false);
    }
    else
    {
      freqboxlabel->setText("Frequency");
      freqboxlabel->setVisible(true);
    }
  }

  QObject::connect(orderbox, SIGNAL(valueChanged(int)),        this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(modelbox, SIGNAL(currentIndexChanged(int)), this, SLOT(filtermodelboxvaluechanged(int)));

  updatecurve();
}



void UI_FilterDialog::frequencyboxvaluechanged(double value)
{
  char str[256];

  if(freq2box->value() < (value * 1.12))
  {
    freq2box->setValue(value * 1.12);
  }

  if(typebox->currentIndex() == FILTERTYPE_NOTCH)
  {
    sprintf(str,"%f Hz", freqbox->value() / orderbox->value());
    remove_trailing_zeros(str);
    ordervaluelabel->setText(str);
  }

  updatecurve();
}



void UI_FilterDialog::freq2boxvaluechanged(double value)
{
  QObject::disconnect(freqbox, SIGNAL(valueChanged(double)), this, SLOT(frequencyboxvaluechanged(double)));

  if(freqbox->value() > (value * 0.9))
  {
    freqbox->setValue(value * 0.9);
  }

  QObject::connect(freqbox, SIGNAL(valueChanged(double)), this, SLOT(frequencyboxvaluechanged(double)));

  updatecurve();
}



void UI_FilterDialog::ApplyButtonClicked()
{
  int i, s, n,
      type,
      model,
      order;

  double frequency,
         frequency2,
         ripple;

  char *err;

  QListWidgetItem *item;

  QList<QListWidgetItem *> selectedlist;

  selectedlist = list->selectedItems();

  n = selectedlist.size();

  if(!n)
  {
    filterdialog->close();
    return;
  }

  frequency = freqbox->value();

  frequency2 = freq2box->value();

  order = orderbox->value();

  type = typebox->currentIndex();

  model = modelbox->currentIndex();

  ripple = -(ripplebox->value());

  for(i=0; i<n; i++)
  {
    item = selectedlist.at(i);
    s = item->data(Qt::UserRole).toInt();

    if((mainwindow->signalcomp[s]->filter_cnt + mainwindow->signalcomp[s]->fidfilter_cnt) > MAXFILTERS - 1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                "The maximum amount of filters per signal has been reached.\n"
                                "Remove some filters first.");
      messagewindow.exec();
      return;
    }
  }

  if((model == FILTERMODEL_BUTTERWORTH) || (model == FILTERMODEL_CHEBYSHEV) || (model == FILTERMODEL_BESSEL))
  {
    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = list->row(item);

      if((type == FILTERTYPE_HIGHPASS) || (type == FILTERTYPE_LOWPASS) || (type == FILTERTYPE_NOTCH))
      {
        if(frequency >= ((mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record
                        / mainwindow->signalcomp[s]->edfhdr->data_record_duration)
                        / 2.0))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                    "The frequency of the filter(s) must be less than: samplerate / 2");
          messagewindow.exec();
          return;
        }
      }
      else
      {
        if(frequency2 >= ((mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record
                        / mainwindow->signalcomp[s]->edfhdr->data_record_duration)
                        / 2.0))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                    "The frequency of the filter(s) must be less than: samplerate / 2");
          messagewindow.exec();
          return;
        }
      }
    }

    spec_str_1[0] = 0;

    if(type == FILTERTYPE_HIGHPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        sprintf(spec_str_1, "HpBu%i/%f", order, frequency);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        sprintf(spec_str_1, "HpCh%i/%f/%f", order, ripple, frequency);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        sprintf(spec_str_1, "HpBe%i/%f", order, frequency);
      }
    }

    if(type == FILTERTYPE_LOWPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        sprintf(spec_str_1, "LpBu%i/%f", order, frequency);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        sprintf(spec_str_1, "LpCh%i/%f/%f", order, ripple, frequency);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        sprintf(spec_str_1, "LpBe%i/%f", order, frequency);
      }
    }

    if(type == FILTERTYPE_NOTCH)
    {
      sprintf(spec_str_1, "BsRe/%i/%f", order, frequency);
    }

    if(type == FILTERTYPE_BANDPASS)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        sprintf(spec_str_1, "BpBu%i/%f-%f", order, frequency, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        sprintf(spec_str_1, "BpCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        sprintf(spec_str_1, "BpBe%i/%f-%f", order, frequency, frequency2);
      }
    }

    if(type == FILTERTYPE_BANDSTOP)
    {
      if(model == FILTERMODEL_BUTTERWORTH)
      {
        sprintf(spec_str_1, "BsBu%i/%f-%f", order, frequency, frequency2);
      }

      if(model == FILTERMODEL_CHEBYSHEV)
      {
        sprintf(spec_str_1, "BsCh%i/%f/%f-%f", order, ripple, frequency, frequency2);
      }

      if(model == FILTERMODEL_BESSEL)
      {
        sprintf(spec_str_1, "BsBe%i/%f-%f", order, frequency, frequency2);
      }
    }

    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = list->row(item);

      strcpy(spec_str_2, spec_str_1);

      filter_spec = spec_str_2;

      err = fid_parse(((double)(mainwindow->signalcomp[s]->edfhdr->edfparam[mainwindow->signalcomp[s]->edfsignal[0]].smp_per_record)) / mainwindow->signalcomp[s]->edfhdr->data_record_duration,
                      &filter_spec,
                      &mainwindow->signalcomp[s]->fidfilter[mainwindow->signalcomp[s]->fidfilter_cnt]);

      if(err != NULL)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", err);
        messagewindow.exec();
        free(err);
        filterdialog->close();
        return;
      }

      mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_new(mainwindow->signalcomp[s]->fidfilter[mainwindow->signalcomp[s]->fidfilter_cnt],
                                                                                      &mainwindow->signalcomp[s]->fidfuncp[mainwindow->signalcomp[s]->fidfilter_cnt]);

      mainwindow->signalcomp[s]->fidbuf[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_newbuf(mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt]);
      mainwindow->signalcomp[s]->fidbuf2[mainwindow->signalcomp[s]->fidfilter_cnt] = fid_run_newbuf(mainwindow->signalcomp[s]->fid_run[mainwindow->signalcomp[s]->fidfilter_cnt]);

      mainwindow->signalcomp[s]->fidfilter_freq[mainwindow->signalcomp[s]->fidfilter_cnt] = frequency;

      mainwindow->signalcomp[s]->fidfilter_freq2[mainwindow->signalcomp[s]->fidfilter_cnt] = frequency2;

      mainwindow->signalcomp[s]->fidfilter_ripple[mainwindow->signalcomp[s]->fidfilter_cnt] = ripple;

      mainwindow->signalcomp[s]->fidfilter_order[mainwindow->signalcomp[s]->fidfilter_cnt] = order;

      mainwindow->signalcomp[s]->fidfilter_model[mainwindow->signalcomp[s]->fidfilter_cnt] = model;

      mainwindow->signalcomp[s]->fidfilter_type[mainwindow->signalcomp[s]->fidfilter_cnt] = type;

      mainwindow->signalcomp[s]->fidfilter_setup[mainwindow->signalcomp[s]->fidfilter_cnt] = 1;

      mainwindow->signalcomp[s]->fidfilter_cnt++;
    }
  }

  if(model == FILTERMODEL_MOVINGAVERAGE)
  {
    for(i=0; i<n; i++)
    {
      item = selectedlist.at(i);
      s = list->row(item);

      mainwindow->signalcomp[s]->ravg_filter[mainwindow->signalcomp[s]->ravg_filter_cnt] = create_ravg_filter(type, order);
      if(mainwindow->signalcomp[s]->ravg_filter[mainwindow->signalcomp[s]->ravg_filter_cnt] == NULL)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while creating a moving average filter.");
        messagewindow.exec();
        filterdialog->close();
        return;
      }

      mainwindow->signalcomp[s]->ravg_filter_type[mainwindow->signalcomp[s]->ravg_filter_cnt] = type;

      mainwindow->signalcomp[s]->ravg_filter_size[mainwindow->signalcomp[s]->ravg_filter_cnt] = order;

      mainwindow->signalcomp[s]->ravg_filter_setup[mainwindow->signalcomp[s]->ravg_filter_cnt] = 1;

      mainwindow->signalcomp[s]->ravg_filter_cnt++;
    }
  }

  filterdialog->close();

  mainwindow->setup_viewbuf();
}










