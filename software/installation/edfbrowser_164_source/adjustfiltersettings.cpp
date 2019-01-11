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


#include "adjustfiltersettings.h"



AdjustFilterSettings::AdjustFilterSettings(struct signalcompblock *signal_comp, QWidget *w_parent)
{
  int i;

  char txtbuf[2048];

  signalcomp = signal_comp;

  maincurve = (ViewCurve *)w_parent;

  mainwindow = maincurve->mainwindow;

  filtersettings_dialog = new QDialog(w_parent);

  filtersettings_dialog->setMinimumSize(460, 255);
  filtersettings_dialog->setMaximumSize(460, 255);
  strcpy(txtbuf, "Filter settings ");
  if(signalcomp->alias[0] != 0)
  {
    strcat(txtbuf, signalcomp->alias);
  }
  else
  {
    strcat(txtbuf, signalcomp->signallabel);
  }
  filtersettings_dialog->setWindowTitle(txtbuf);
  filtersettings_dialog->setModal(true);
  filtersettings_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  for(i=0; i<5; i++)
  {
    label[i] = new QLabel(filtersettings_dialog);
    label[i]->setGeometry(10, 10 + (i * 35), 100, 25);
  }
  label[0]->setText("Filter");
  label[1]->setText("Order");
  label[2]->setText("Frequency");
  label[3]->setText("Frequency 2");
  label[3]->setVisible(false);
  label[4]->setText("Stepsize");

  filterbox = new QComboBox(filtersettings_dialog);
  filterbox->setGeometry(120, 10, 330, 25);

  orderbox = new QSpinBox(filtersettings_dialog);
  orderbox->setGeometry(120, 45, 100, 25);
  orderbox->setSuffix("th order");
  orderbox->setMinimum(1);
  orderbox->setMaximum(8);
  orderbox->setValue(1);
  orderbox->setSingleStep(1);

  freq1box = new QDoubleSpinBox(filtersettings_dialog);
  freq1box->setGeometry(120, 80, 180, 25);
  freq1box->setDecimals(6);
  freq1box->setSuffix(" Hz");
  freq1box->setMinimum(0.0001);
  freq1box->setMaximum(100000.0);
  freq1box->setValue(1.0);
  freq1box->setSingleStep(1.0);

  freq2box = new QDoubleSpinBox(filtersettings_dialog);
  freq2box->setGeometry(120, 115, 180, 25);
  freq2box->setDecimals(6);
  freq2box->setSuffix(" Hz");
  freq2box->setMinimum(0.0001);
  freq2box->setMaximum(100000.0);
  freq2box->setValue(2.0);
  freq2box->setVisible(false);
  freq2box->setSingleStep(1.0);

  stepsizebox = new QComboBox(filtersettings_dialog);
  stepsizebox->setGeometry(120, 150, 80, 25);
  stepsizebox->addItem("0.01Hz");
  stepsizebox->addItem("0.1Hz");
  stepsizebox->addItem("1Hz");
  stepsizebox->addItem("10Hz");
  stepsizebox->addItem("100Hz");
  stepsizebox->setCurrentIndex(2);

  RemoveButton = new QPushButton(filtersettings_dialog);
  RemoveButton->setGeometry(120, 220, 80, 25);
  RemoveButton->setText("Remove");

  CloseButton = new QPushButton(filtersettings_dialog);
  CloseButton->setGeometry(360, 220, 80, 25);
  CloseButton->setText("Close");

  filter_cnt = 0;

  loadFilterSettings();

  QObject::connect(freq1box,     SIGNAL(valueChanged(double)),                      this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box,     SIGNAL(valueChanged(double)),                      this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox,     SIGNAL(valueChanged(int)),                         this, SLOT(orderboxvaluechanged(int)));
  QObject::connect(filterbox,    SIGNAL(currentIndexChanged(int)),                  this, SLOT(filterboxchanged(int)));
  QObject::connect(stepsizebox,  SIGNAL(currentIndexChanged(int)),                  this, SLOT(stepsizeboxchanged(int)));
  QObject::connect(RemoveButton, SIGNAL(clicked()),                                 this, SLOT(removeButtonClicked()));
  QObject::connect(CloseButton,  SIGNAL(clicked()),                filtersettings_dialog, SLOT(close()));

  filtersettings_dialog->exec();
}


void AdjustFilterSettings::loadFilterSettings(void)
{
  int i;

  char txtbuf[2048];

  txtbuf[0] = 0;

  filterbox->clear();

  for(i=0; i<signalcomp->fidfilter_cnt; i++)
  {
    type = signalcomp->fidfilter_type[i];

    model = signalcomp->fidfilter_model[i];

    order = signalcomp->fidfilter_order[i];

    ripple = signalcomp->fidfilter_ripple[i];

    frequency1 = signalcomp->fidfilter_freq[i];

    if(type == 0)
    {
      if(model == 0)
      {
        sprintf(txtbuf, "HighPass Butterworth");
      }

      if(model == 1)
      {
        sprintf(txtbuf, "HighPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == 2)
      {
        sprintf(txtbuf, "HighPass Bessel");
      }
    }

    if(type == 1)
    {
      if(model == 0)
      {
        sprintf(txtbuf, "LowPass Butterworth");
      }

      if(model == 1)
      {
        sprintf(txtbuf, "LowPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == 2)
      {
        sprintf(txtbuf, "LowPass Bessel");
      }
    }

    if(type == 2)
    {
      sprintf(txtbuf, "Notch (resonator)");
    }

    if(type == 3)
    {
      if(model == 0)
      {
        sprintf(txtbuf, "BandPass Butterworth");
      }

      if(model == 1)
      {
        sprintf(txtbuf, "BandPass Chebyshev %.1fdB ripple", ripple);
      }

      if(model == 2)
      {
        sprintf(txtbuf, "BandPass Bessel");
      }
    }

    if(type == 4)
    {
      if(model == 0)
      {
        sprintf(txtbuf, "BandStop Butterworth");
      }

      if(model == 1)
      {
        sprintf(txtbuf, "BandStop Chebyshev %.1fdB ripple", ripple);
      }

      if(model == 2)
      {
        sprintf(txtbuf, "BandStop Bessel");
      }
    }

    sprintf(txtbuf + strlen(txtbuf), " %f", frequency1);

    remove_trailing_zeros(txtbuf);

    strcat(txtbuf, " Hz");

    filterbox->addItem(txtbuf);

    brand[filter_cnt++] = 0;
  }

  for(i=0; i<signalcomp->ravg_filter_cnt; i++)
  {
    type = signalcomp->ravg_filter_type[i];

    size = signalcomp->ravg_filter_size[i];

    if(type == 0)
    {
      sprintf(txtbuf, "Highpass Moving Average %i samples", size);
    }

    if(type == 1)
    {
      sprintf(txtbuf, "Lowpass Moving Average %i samples", size);
    }

    filterbox->addItem(txtbuf);

    brand[filter_cnt++] = 1;
  }

  filterboxchanged(filterbox->currentIndex());
}


void AdjustFilterSettings::orderboxvaluechanged(int)
{
  int j;

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::freqbox1valuechanged(double value)
{
  int j;

  if((type == 3) || (type == 4))
  {
    if(freq2box->value() < (value * 1.12))
    {
      QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));

      freq2box->setValue(value * 1.12);

      QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
    }
  }

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::freqbox2valuechanged(double value)
{
  int j;

  if(freq1box->value() > (value * 0.9))
  {
    QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));

    freq1box->setValue(value * 0.9);

    QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  }

  for(j=0; j<10; j++)  qApp->processEvents();

  update_filter();
}


void AdjustFilterSettings::filterboxchanged(int i)
{
  int n;


  if(i < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[i] == 0)  // fidfilter
  {
    n = i;

    frequency1 = signalcomp->fidfilter_freq[n];

    frequency2 = signalcomp->fidfilter_freq2[n];

    type = signalcomp->fidfilter_type[n];

    model = signalcomp->fidfilter_model[n];

    order = signalcomp->fidfilter_order[n];

    ripple = signalcomp->fidfilter_ripple[n];

    freq1box->setValue(frequency1);

    label[1]->setText("Order");
    label[2]->setText("Frequency");
    freq1box->setVisible(true);
    stepsizebox->clear();
    stepsizebox->addItem("0.01Hz");
    stepsizebox->addItem("0.1Hz");
    stepsizebox->addItem("1Hz");
    stepsizebox->addItem("10Hz");
    stepsizebox->addItem("100Hz");
    stepsizebox->setCurrentIndex(2);

    if((type == 3) || (type == 4))
    {
      freq2box->setValue(frequency2);
      label[3]->setVisible(true);
      freq2box->setVisible(true);
      orderbox->setMinimum(2);
      orderbox->setSingleStep(2);
      orderbox->setMaximum(16);
    }
    else
    {
      freq2box->setValue(0.0);
      label[3]->setVisible(false);
      freq2box->setVisible(false);
      orderbox->setMinimum(1);
      orderbox->setSingleStep(1);
      orderbox->setMaximum(8);
    }

    if(type == 2)
    {
      orderbox->setPrefix("Q-factor ");
      orderbox->setSuffix("");
      orderbox->setMaximum(100);
      orderbox->setMinimum(3);
    }
    else
    {
      orderbox->setPrefix("");
      orderbox->setSuffix("th order");
    }

    orderbox->setValue(order);
  }

  if(brand[i] == 1)  // moving average filter
  {
    n = i - signalcomp->fidfilter_cnt;

    type = signalcomp->ravg_filter_type[n];

    size = signalcomp->ravg_filter_size[n];

    stepsizebox->clear();
    stepsizebox->addItem("1");
    stepsizebox->addItem("10");
    stepsizebox->addItem("100");
    stepsizebox->addItem("1000");
    stepsizebox->setCurrentIndex(0);

    label[1]->setText("Size");
    label[2]->setText("");
    freq1box->setVisible(false);
    label[3]->setVisible(false);
    freq2box->setVisible(false);
    orderbox->setMinimum(2);
    orderbox->setSingleStep(1);
    orderbox->setMaximum(10000);
    orderbox->setPrefix("");
    orderbox->setSuffix(" samples");
    orderbox->setValue(size);
  }

  QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));
}


void AdjustFilterSettings::removeButtonClicked()
{
  int i;

  filter_nr = filterbox->currentIndex();

  if(filter_nr < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[filter_nr] == 0)  // fidfilter
  {
    free(signalcomp->fidfilter[filter_nr]);
    fid_run_free(signalcomp->fid_run[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf2[filter_nr]);

    for(i=filter_nr; i<(signalcomp->fidfilter_cnt - 1); i++)
    {
      signalcomp->fidfilter[i] = signalcomp->fidfilter[i + 1];
      signalcomp->fid_run[i] = signalcomp->fid_run[i + 1];
      signalcomp->fidbuf[i] = signalcomp->fidbuf[i + 1];
      signalcomp->fidbuf2[i] = signalcomp->fidbuf2[i + 1];
    }

    signalcomp->fidfilter_cnt--;
  }
  else if(brand[filter_nr] == 1)  // moving average filter
    {
      filter_nr -= signalcomp->fidfilter_cnt;

      free_ravg_filter(signalcomp->ravg_filter[filter_nr]);

      for(i=filter_nr; i<(signalcomp->ravg_filter_cnt - 1); i++)
      {
        signalcomp->ravg_filter[i] = signalcomp->ravg_filter[i + 1];
      }

      signalcomp->ravg_filter_cnt--;
    }

  loadFilterSettings();

  mainwindow->setup_viewbuf();
}


void AdjustFilterSettings::update_filter()
{
  char *filter_spec,
      spec_str_1[256],
      spec_str_2[256],
      *err=NULL;


  filter_nr = filterbox->currentIndex();

  if(filter_nr < 0)
  {
    return;
  }

  QObject::disconnect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::disconnect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::disconnect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));

  if(brand[filter_nr] == 0)  // fidfilter
  {
    frequency1 = freq1box->value();
    frequency2 = freq2box->value();
    order = orderbox->value();

    if(type < 3)
    {
      if(frequency1 >= ((signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0))
      {
        frequency1 = (signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0;

        frequency1 -= freq1box->singleStep();

        freq1box->setValue(frequency1);
      }
    }
    else
    {
      if(frequency2 >= ((signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0))
      {
        frequency2 = (signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0;

        frequency2 -= freq2box->singleStep();

        freq2box->setValue(frequency2);
      }

      if(frequency1 >= frequency2 * 0.9)
      {
        frequency1 = frequency2 * 0.9;

        freq1box->setValue(frequency1);
      }
    }

    if(type < 3)
    {
      if(frequency1 >= ((signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                  "The frequency must be less than: samplerate / 2");
        messagewindow.exec();
        return;
      }
    }
    else
    {
      if(frequency2 >= ((signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record
                      / signalcomp->edfhdr->data_record_duration)
                      / 2.0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                  "The frequency must be less than: samplerate / 2");
        messagewindow.exec();
        return;
      }
    }

    spec_str_1[0] = 0;

    if(type == 0)
    {
      if(model == 0)
      {
        sprintf(spec_str_1, "HpBu%i/%f", order, frequency1);
      }

      if(model == 1)
      {
        sprintf(spec_str_1, "HpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == 2)
      {
        sprintf(spec_str_1, "HpBe%i/%f", order, frequency1);
      }
    }

    if(type == 1)
    {
      if(model == 0)
      {
        sprintf(spec_str_1, "LpBu%i/%f", order, frequency1);
      }

      if(model == 1)
      {
        sprintf(spec_str_1, "LpCh%i/%f/%f", order, ripple, frequency1);
      }

      if(model == 2)
      {
        sprintf(spec_str_1, "LpBe%i/%f", order, frequency1);
      }
    }

    if(type == 2)
    {
      sprintf(spec_str_1, "BsRe/%i/%f", order, frequency1);
    }

    if(type == 3)
    {
      if(model == 0)
      {
        sprintf(spec_str_1, "BpBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == 1)
      {
        sprintf(spec_str_1, "BpCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == 2)
      {
        sprintf(spec_str_1, "BpBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    if(type == 4)
    {
      if(model == 0)
      {
        sprintf(spec_str_1, "BsBu%i/%f-%f", order, frequency1, frequency2);
      }

      if(model == 1)
      {
        sprintf(spec_str_1, "BsCh%i/%f/%f-%f", order, ripple, frequency1, frequency2);
      }

      if(model == 2)
      {
        sprintf(spec_str_1, "BsBe%i/%f-%f", order, frequency1, frequency2);
      }
    }

    strcpy(spec_str_2, spec_str_1);

    filter_spec = spec_str_2;

    free(signalcomp->fidfilter[filter_nr]);
    fid_run_free(signalcomp->fid_run[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf[filter_nr]);
    fid_run_freebuf(signalcomp->fidbuf2[filter_nr]);

    err = fid_parse(((double)(signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record)) / signalcomp->edfhdr->data_record_duration,
                    &filter_spec,
                    &signalcomp->fidfilter[filter_nr]);

    if(err != NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", err);
      messagewindow.exec();
      free(err);
      return;
    }

    signalcomp->fid_run[filter_nr] = fid_run_new(signalcomp->fidfilter[filter_nr], &signalcomp->fidfuncp[filter_nr]);

    signalcomp->fidbuf[filter_nr] = fid_run_newbuf(signalcomp->fid_run[filter_nr]);
    signalcomp->fidbuf2[filter_nr] = fid_run_newbuf(signalcomp->fid_run[filter_nr]);

    signalcomp->fidfilter_freq[filter_nr] = frequency1;
    signalcomp->fidfilter_freq2[filter_nr] = frequency2;
    signalcomp->fidfilter_order[filter_nr] = order;
  }

  if(brand[filter_nr] == 1)  // moving average filter
  {
    filter_nr -= signalcomp->fidfilter_cnt;

    size = orderbox->value();

    free_ravg_filter(signalcomp->ravg_filter[filter_nr]);

    signalcomp->ravg_filter[filter_nr] = create_ravg_filter(type, size);
    if(signalcomp->ravg_filter[filter_nr] == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error",
                                "An internal error occurred while creating a moving average filter.");
      messagewindow.exec();
      return;
    }

    signalcomp->ravg_filter_size[filter_nr] = size;
  }

  mainwindow->setup_viewbuf();

  QObject::connect(freq1box, SIGNAL(valueChanged(double)), this, SLOT(freqbox1valuechanged(double)));
  QObject::connect(freq2box, SIGNAL(valueChanged(double)), this, SLOT(freqbox2valuechanged(double)));
  QObject::connect(orderbox, SIGNAL(valueChanged(int)),    this, SLOT(orderboxvaluechanged(int)));
}


void AdjustFilterSettings::stepsizeboxchanged(int index)
{
  int n;

  n = filterbox->currentIndex();

  if(n < 0)
  {
    return;
  }

  if(brand[n] == 0)  // fidfilter
  {
    switch(index)
    {
      case 0: freq1box->setSingleStep(0.01);
              freq2box->setSingleStep(0.01);
              break;
      case 1: freq1box->setSingleStep(0.1);
              freq2box->setSingleStep(0.1);
              break;
      case 2: freq1box->setSingleStep(1.0);
              freq2box->setSingleStep(1.0);
              break;
      case 3: freq1box->setSingleStep(10.0);
              freq2box->setSingleStep(10.0);
              break;
      case 4: freq1box->setSingleStep(100.0);
              freq2box->setSingleStep(100.0);
              break;
    }
  }

  if(brand[n] == 1)  // moving average filter
  {
    switch(index)
    {
      case 0: orderbox->setSingleStep(1);
              break;
      case 1: orderbox->setSingleStep(10);
              break;
      case 2: orderbox->setSingleStep(100);
              break;
      case 3: orderbox->setSingleStep(1000);
              break;
    }
  }
}


