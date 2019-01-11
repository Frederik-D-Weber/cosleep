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



#include "spectrum_dock.h"


#define SPECT_LOG_MINIMUM 0.000001
#define SPECT_LOG_MINIMUM_LOG (-5)



UI_SpectrumDockWindow::UI_SpectrumDockWindow(QWidget *w_parent)
{
  char str[1024];

  fft_data = NULL;

  buf1 = NULL;
  buf2 = NULL;
  buf3 = NULL;
  buf4 = NULL;
  buf5 = NULL;

  buf1_sz = 0;

  busy = 0;

  dashboard = 0;

  signalcomp = NULL;

  signal_nr = -1;

  set_settings = 0;

  physdimension[0] = 0;

  mainwindow = (UI_Mainwindow *)w_parent;

  spectrum_color = mainwindow->spectrum_colorbar;

  SpectrumDialog = new QDialog;

  flywheel_value = 1050;

  init_maxvalue = 1;

  dftblocksize = mainwindow->maxdftblocksize;

  window_type = 0;

  if(mainwindow->spectrumdock_sqrt)
  {
    dock = new QDockWidget("Amplitude Spectrum", w_parent);
  }
  else
  {
    dock = new QDockWidget("Power Spectrum", w_parent);
  }

  dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  dock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  dock->setMinimumHeight(300);
  if(dashboard)
  {
    dock->setWidget(SpectrumDialog);
  }

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::green);
  curve1->setBackgroundColor(Qt::black);
  curve1->setRasterColor(Qt::gray);
  curve1->setTraceWidth(0);
  curve1->setH_label("Hz");
  curve1->setLowerLabel("Frequency");
  if(mainwindow->spectrumdock_sqrt)
  {
    if(mainwindow->spectrumdock_vlog)
    {
      snprintf(str, 512, "log10(%s)", physdimension);
      curve1->setV_label(str);
    }
    else
    {
      curve1->setV_label(physdimension);
    }
  }
  else
  {
    if(mainwindow->spectrumdock_vlog)
    {
      snprintf(str, 512, "log((%s)^2/Hz)", physdimension);
    }
    else
    {
      snprintf(str, 512, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str);
  }
  curve1->create_button("to Text");

  if(mainwindow->spectrumdock_colorbars)
  {
    curve1->enableSpectrumColors(spectrum_color);
  }
  else
  {
    curve1->disableSpectrumColors();
  }

  if(!dashboard)
  {
    dock->setWidget(curve1);
  }

  flywheel1 = new UI_Flywheel;
  flywheel1->setMinimumSize(20, 85);

  amplitudeSlider = new QSlider;
  amplitudeSlider->setOrientation(Qt::Vertical);
  amplitudeSlider->setMinimum(1);
  amplitudeSlider->setMaximum(2000);
  amplitudeSlider->setValue(1000);
  amplitudeSlider->setInvertedAppearance(true);
  amplitudeSlider->setMinimumSize(15, 110);

  log_minslider = new QSlider;
  log_minslider->setOrientation(Qt::Vertical);
  log_minslider->setMinimum(1);
  log_minslider->setMaximum(2000);
  log_minslider->setValue(1000);
  log_minslider->setInvertedAppearance(false);
  log_minslider->setMinimumSize(15, 110);

  amplitudeLabel = new QLabel;
  amplitudeLabel->setText("Amplitude");
  amplitudeLabel->setMinimumSize(100, 15);
  amplitudeLabel->setAlignment(Qt::AlignHCenter);

  sqrtButton = new QCheckBox;
  sqrtButton->setMinimumSize(50, 20);
  sqrtButton->setText("Amplitude");
  sqrtButton->setTristate(false);
  if(mainwindow->spectrumdock_sqrt)
  {
    sqrtButton->setChecked(true);
  }
  else
  {
    sqrtButton->setChecked(false);
  }

  vlogButton = new QCheckBox;
  vlogButton->setMinimumSize(50, 20);
  vlogButton->setText("Log");
  vlogButton->setTristate(false);
  if(mainwindow->spectrumdock_vlog)
  {
    vlogButton->setChecked(true);

    log_minslider->setVisible(true);
  }
  else
  {
    vlogButton->setChecked(false);

    log_minslider->setVisible(false);
  }

  colorBarButton = new QCheckBox;
  colorBarButton->setMinimumSize(50, 20);
  colorBarButton->setText("Colorbar");
  colorBarButton->setTristate(false);
  if(mainwindow->spectrumdock_colorbars)
  {
    colorBarButton->setCheckState(Qt::Checked);
  }
  else
  {
    colorBarButton->setCheckState(Qt::Unchecked);
  }

  windowBox = new QComboBox;
  windowBox->setMinimumSize(70, 25);
  windowBox->addItem("Rectangular");
  windowBox->addItem("Hamming");
  windowBox->addItem("Blackman");
  windowBox->setCurrentIndex(window_type);

  dftsz_label = new QLabel;
  dftsz_label->setText("Blocksize:");
  dftsz_label->setMinimumSize(100, 25);

  dftsz_spinbox = new QSpinBox;
  dftsz_spinbox->setMinimumSize(70, 25);
  dftsz_spinbox->setMinimum(10);
  dftsz_spinbox->setMaximum(16777216);  // (2^24)
  dftsz_spinbox->setSingleStep(2);
  dftsz_spinbox->setValue(dftblocksize);

  vlayout3 = new QVBoxLayout;
  vlayout3->addStretch(100);
  vlayout3->addWidget(flywheel1, 100);
  vlayout3->addStretch(100);

  hlayout4 = new QHBoxLayout;
  hlayout4->addStretch(100);
  hlayout4->addLayout(vlayout3, 100);
  hlayout4->addStretch(100);
  hlayout4->addWidget(amplitudeSlider, 300);
  hlayout4->addWidget(log_minslider, 300);

  vlayout2 = new QVBoxLayout;
  vlayout2->setSpacing(5);
  vlayout2->addStretch(100);
  vlayout2->addWidget(amplitudeLabel, 0, Qt::AlignHCenter);
  vlayout2->addLayout(hlayout4, 200);
//  vlayout2->addWidget(amplitudeSlider, 0, Qt::AlignHCenter);
  vlayout2->addWidget(sqrtButton);
  vlayout2->addWidget(vlogButton);
  vlayout2->addWidget(colorBarButton);
  vlayout2->addWidget(windowBox);
  vlayout2->addWidget(dftsz_label);
  vlayout2->addWidget(dftsz_spinbox);

  spanSlider = new QSlider;
  spanSlider->setOrientation(Qt::Horizontal);
  spanSlider->setMinimum(1);
  spanSlider->setMaximum(1000);
  spanSlider->setValue(1000);
  spanSlider->setMinimumSize(500, 15);

  spanLabel = new QLabel;
  spanLabel->setText("Span");
  spanLabel->setMinimumSize(100, 15);
  spanLabel->setAlignment(Qt::AlignHCenter);

  centerSlider = new QSlider;
  centerSlider->setOrientation(Qt::Horizontal);
  centerSlider->setMinimum(0);
  centerSlider->setMaximum(1000);
  centerSlider->setValue(0);
  centerSlider->setMinimumSize(500, 15);

  centerLabel = new QLabel;
  centerLabel->setText("Center");
  centerLabel->setMinimumSize(100, 15);
  centerLabel->setAlignment(Qt::AlignHCenter);

  hlayout1 = new QHBoxLayout;
  hlayout1->setSpacing(20);
  hlayout1->addLayout(vlayout2);
  if(dashboard)
  {
    hlayout1->addWidget(curve1, 100);
  }

  hlayout2 = new QHBoxLayout;
  hlayout2->setSpacing(20);
  hlayout2->addWidget(spanLabel);
  hlayout2->addWidget(spanSlider);
  hlayout2->addStretch(100);

  hlayout3 = new QHBoxLayout;
  hlayout3->setSpacing(20);
  hlayout3->addWidget(centerLabel);
  hlayout3->addWidget(centerSlider);
  hlayout3->addStretch(100);

  vlayout1 = new QVBoxLayout;
  vlayout1->setSpacing(20);
  vlayout1->addLayout(hlayout1);
  vlayout1->addLayout(hlayout2);
  vlayout1->addLayout(hlayout3);

  SpectrumDialog->setLayout(vlayout1);

  t1 = new QTimer(this);
  t1->setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1->setTimerType(Qt::PreciseTimer);
#endif

  QObject::connect(t1,              SIGNAL(timeout()),               this, SLOT(update_curve()));
  QObject::connect(amplitudeSlider, SIGNAL(valueChanged(int)),       this, SLOT(sliderMoved(int)));
  QObject::connect(log_minslider,   SIGNAL(valueChanged(int)),       this, SLOT(sliderMoved(int)));
  QObject::connect(spanSlider,      SIGNAL(valueChanged(int)),       this, SLOT(sliderMoved(int)));
  QObject::connect(centerSlider,    SIGNAL(valueChanged(int)),       this, SLOT(sliderMoved(int)));
  QObject::connect(sqrtButton,      SIGNAL(toggled(bool)),           this, SLOT(sqrtButtonClicked(bool)));
  QObject::connect(vlogButton,      SIGNAL(toggled(bool)),           this, SLOT(vlogButtonClicked(bool)));
  QObject::connect(colorBarButton,  SIGNAL(toggled(bool)),           this, SLOT(colorBarButtonClicked(bool)));
  QObject::connect(curve1,          SIGNAL(extra_button_clicked()),  this, SLOT(print_to_txt()));
  QObject::connect(curve1,          SIGNAL(dashBoardClicked()),      this, SLOT(setdashboard()));
  QObject::connect(flywheel1,       SIGNAL(dialMoved(int)),          this, SLOT(update_flywheel(int)));
  QObject::connect(dock,            SIGNAL(visibilityChanged(bool)), this, SLOT(open_close_dock(bool)));
  QObject::connect(dftsz_spinbox,   SIGNAL(valueChanged(int)),        this, SLOT(dftsz_value_changed(int)));
  QObject::connect(windowBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(windowBox_changed(int)));
}


void UI_SpectrumDockWindow::windowBox_changed(int idx)
{
  if(busy)  return;

  if(window_type == idx)  return;

  window_type = idx;

  update_curve();
}


void UI_SpectrumDockWindow::dftsz_value_changed(int new_val)
{
  if(busy)  return;

  if(dftblocksize == new_val)  return;

  dftblocksize = new_val;

  update_curve();
}


void UI_SpectrumDockWindow::open_close_dock(bool)
{
  if(mainwindow->files_open != 1 || signal_nr < 0)
  {
    dock->hide();

    return;
  }
}


void UI_SpectrumDockWindow::setsettings(struct spectrumdocksettings sett)
{
  settings = sett;

  set_settings = 1;
}


void UI_SpectrumDockWindow::getsettings(struct spectrumdocksettings *sett)
{
  sett->signalnr = signal_nr;

  sett->amp = amplitudeSlider->value();

  sett->log_min_sl = log_minslider->value();

  sett->wheel = flywheel_value;

  sett->span = spanSlider->value();

  sett->center = centerSlider->value();

  if(sqrtButton->isChecked() == true)
  {
    sett->sqrt = 1;
  }
  else
  {
    sett->sqrt = 0;
  }

  if(vlogButton->isChecked() == true)
  {
    sett->log = 1;
  }
  else
  {
    sett->log = 0;
  }

  if(colorBarButton->isChecked() == true)
  {
    sett->colorbar = 1;
  }
  else
  {
    sett->colorbar = 0;
  }

  sett->maxvalue = maxvalue;

  sett->maxvalue_sqrt = maxvalue_sqrt;

  sett->maxvalue_vlog = maxvalue_vlog;

  sett->maxvalue_sqrt_vlog = maxvalue_sqrt_vlog;

  sett->minvalue_vlog = minvalue_vlog;

  sett->minvalue_sqrt_vlog = minvalue_sqrt_vlog;
}


void UI_SpectrumDockWindow::update_flywheel(int new_value)
{
  flywheel_value += new_value;

  if(flywheel_value < 10)
  {
    flywheel_value = 10;
  }

  if(flywheel_value > 100000)
  {
    flywheel_value = 100000;
  }

  sliderMoved(0);
}


void UI_SpectrumDockWindow::setdashboard()
{
  if(dashboard)
  {
    dashboard = 0;
    hlayout1->removeWidget(curve1);
    dock->setWidget(curve1);
    dock->setMinimumHeight(300);
  }
  else
  {
    dashboard = 1;
    hlayout1->addWidget(curve1, 100);
    dock->setWidget(SpectrumDialog);
  }
}


void UI_SpectrumDockWindow::print_to_txt()
{
  int i;

  char str[1024],
       path[MAX_PATH_LENGTH];

  FILE *outputfile;


  if(fft_data == NULL)  return;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  strcat(path, "spectrum.txt");

  strcpy(path, QFileDialog::getSaveFileName(0, "Export to text (ASCII)", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  outputfile = fopen(path, "wb");
  if(outputfile == NULL)
  {
    snprintf(str, 1024, "Can not open file %s for writing.", path);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", QString::fromLocal8Bit(str));
    messagewindow.exec();
    return;
  }

  sprintf(str, "FFT Power Spectral Density (Power/%fHz)\n", freqstep);
  remove_trailing_zeros(str);
  fprintf(outputfile, "%s", str);
  fprintf(outputfile, "Signal: %s\n", signalcomp->signallabel);
  sprintf(str, "FFT blocksize: %i\n", fft_data->dft_sz);
  sprintf(str + strlen(str), "FFT resolution: %f Hz\n", freqstep);
  sprintf(str + strlen(str), "Data Samples: %i\n", fft_data->sz_in);
  sprintf(str + strlen(str), "Power Samples: %i\n", fft_data->sz_out);
  sprintf(str + strlen(str), "Samplefrequency: %f Hz\n", (double)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record / ((double)signalcomp->edfhdr->long_data_record_duration / TIME_DIMENSION));
  remove_trailing_zeros(str);
  fprintf(outputfile, "%s", str);

  for(i=0; i<fft_data->sz_out; i++)
  {
    fprintf(outputfile, "%.16f\t%.16f\n", freqstep * i, buf2[i]);
  }

  fclose (outputfile);
}


void UI_SpectrumDockWindow::colorBarButtonClicked(bool value)
{
  if(value == true)
  {
    mainwindow->spectrumdock_colorbars = 1;

    curve1->enableSpectrumColors(spectrum_color);
  }
  else
  {
    mainwindow->spectrumdock_colorbars = 0;

    curve1->disableSpectrumColors();
  }
}


void UI_SpectrumDockWindow::sqrtButtonClicked(bool value)
{
  char str[600];

  if(value == false)
  {
    mainwindow->spectrumdock_sqrt = 0;

    sprintf(str, "Power Spectrum %s", signallabel);

    dock->setWindowTitle(str);

    if(mainwindow->spectrumdock_vlog)
    {
      sprintf(str, "log10((%s)^2/Hz)", physdimension);
    }
    else
    {
      sprintf(str, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str);
  }
  else
  {
    mainwindow->spectrumdock_sqrt = 1;

    sprintf(str, "Amplitude Spectrum %s", signallabel);

    dock->setWindowTitle(str);

    if(mainwindow->spectrumdock_vlog)
    {
      sprintf(str, "log(%s)", physdimension);
    }
    else
    {
      sprintf(str, "%s", physdimension);
    }

    curve1->setV_label(str);
  }

  sliderMoved(0);
}


void UI_SpectrumDockWindow::vlogButtonClicked(bool value)
{
  char str[600];

  if(value == false)
  {
    mainwindow->spectrumdock_vlog = 0;

    if(mainwindow->spectrumdock_sqrt)
    {
      sprintf(str, "%s", physdimension);
    }
    else
    {
      sprintf(str, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str);

    log_minslider->setVisible(false);
  }
  else
  {
    mainwindow->spectrumdock_vlog = 1;

    if(mainwindow->spectrumdock_sqrt)
    {
      sprintf(str, "log10(%s)", physdimension);
    }
    else
    {
      sprintf(str, "log10((%s)^2/Hz)", physdimension);
    }

    curve1->setV_label(str);

    log_minslider->setVisible(true);
  }

  sliderMoved(0);
}


void UI_SpectrumDockWindow::sliderMoved(int)
{
  long long startstep,
            stopstep,
            spanstep;

  double max_freq,
         start_freq;

  char str[1024];

  if(fft_data == NULL)  return;

  spanstep = (long long)spanSlider->value() * (long long)steps / 1000LL;

  startstep = (long long)centerSlider->value() * ((long long)steps - spanstep) / 1000LL;

  stopstep = startstep + spanstep;

  if(sqrtButton->checkState() == Qt::Checked)
  {
    mainwindow->spectrumdock_sqrt = 1;

    if(vlogButton->checkState() == Qt::Checked)
    {
      mainwindow->spectrumdock_vlog = 1;

      curve1->drawCurve(buf5 + startstep, stopstep - startstep, (maxvalue_sqrt_vlog * 1.05 * (((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value())) / 1000.0, minvalue_sqrt_vlog * (double)log_minslider->value() / 1000.0);
    }
    else
    {
      mainwindow->spectrumdock_vlog = 0;

      curve1->drawCurve(buf3 + startstep, stopstep - startstep, (maxvalue_sqrt * 1.05 * (((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value())) / 1000.0, 0.0);
    }
  }
  else
  {
    mainwindow->spectrumdock_sqrt = 0;

    if(vlogButton->checkState() == Qt::Checked)
    {
      mainwindow->spectrumdock_vlog = 1;

      curve1->drawCurve(buf4 + startstep, stopstep - startstep, (maxvalue_vlog * 1.05 * (((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value())) / 1000.0, minvalue_vlog * (double)log_minslider->value() / 1000.0);
    }
    else
    {
      mainwindow->spectrumdock_vlog = 0;

      curve1->drawCurve(buf2 + startstep, stopstep - startstep, (maxvalue * 1.05 * (((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value())) / 1000.0, 0.0);
    }
  }

  max_freq = ((double)samplefreq / 2.0) * stopstep / steps;

  start_freq = ((double)samplefreq / 2.0) * startstep / steps;

  curve1->setH_RulerValues(start_freq, max_freq);

  strcpy(str, "Center ");
  convert_to_metric_suffix(str + strlen(str), start_freq + ((max_freq - start_freq) / 2.0), 3);
  remove_trailing_zeros(str);
  strcat(str, "Hz");
  centerLabel->setText(str);

  strcpy(str, "Span ");
  convert_to_metric_suffix(str + strlen(str), max_freq - start_freq, 3);
  remove_trailing_zeros(str);
  strcat(str, "Hz");
  spanLabel->setText(str);
}


void UI_SpectrumDockWindow::init(int signal_num)
{
  char str[600];

  init_maxvalue = 1;


  if(signal_num < 0)
  {
    signalcomp = NULL;

    viewbuf = NULL;
  }
  else
  {
    signal_nr = signal_num;

    signalcomp = mainwindow->signalcomp[signal_num];

    viewbuf = mainwindow->viewbuf;

    strcpy(signallabel, signalcomp->signallabel);

    strcpy(physdimension, signalcomp->physdimension);

    if(mainwindow->spectrumdock_sqrt)
    {
      sqrtButton->setChecked(true);

      if(mainwindow->spectrumdock_vlog)
      {
        vlogButton->setChecked(true);

        snprintf(str, 512, "log10(%s)", physdimension);
        curve1->setV_label(str);
      }
      else
      {
        vlogButton->setChecked(false);

        curve1->setV_label(physdimension);
      }
    }
    else
    {
      sqrtButton->setChecked(false);

      if(mainwindow->spectrumdock_vlog)
      {
        vlogButton->setChecked(true);

        snprintf(str, 512, "log((%s)^2/Hz)", physdimension);
      }
      else
      {
        vlogButton->setChecked(false);

        snprintf(str, 512, "(%s)^2/Hz", physdimension);
      }

      curve1->setV_label(str);
    }

    amplitudeSlider->setValue(1000);

    log_minslider->setValue(1000);

    dock->show();

    t1->start(1);
  }
}


void UI_SpectrumDockWindow::rescan()
{
  t1->start(1);
}


void UI_SpectrumDockWindow::clear()
{
  int i;

  init_maxvalue = 1;

  signalcomp = NULL;

  viewbuf = NULL;

  free(buf1);
  buf1 = NULL;
  buf1_sz = 0;
  free(buf2);
  buf2 = NULL;
  free(buf3);
  buf3 = NULL;
  free(buf4);
  buf4 = NULL;
  free(buf5);
  buf5 = NULL;

  free_fft_wrap(fft_data);
  fft_data = NULL;

  if(spectrum_color != NULL)
  {
    for(i=0; i < spectrum_color->items; i++)
    {
      spectrum_color->value[i] = 0.0;
    }
  }

  curve1->clear();

  signal_nr = -1;
}


void UI_SpectrumDockWindow::update_curve()
{
  int i, j, k, n,
      fft_inputbufsize=0;
//      fft_outputbufsize;

  long long s, s2;

  char str[512];

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  if(signalcomp == NULL)
  {
    return;
  }

  if(busy)
  {
    return;
  }

  viewbuf = mainwindow->viewbuf;

  if(viewbuf == NULL)
  {
    return;
  }

  busy = 1;

  curve1->setUpdatesEnabled(false);

  samples = signalcomp->samples_on_screen;

  if(signalcomp->samples_on_screen > signalcomp->sample_stop)
  {
    samples = signalcomp->sample_stop;
  }

  samples -= signalcomp->sample_start;

  if((samples < 10) || (viewbuf == NULL))
  {
    curve1->setUpdatesEnabled(true);

    busy = 0;

    if(spectrum_color != NULL)
    {
      for(i=0; i < spectrum_color->items; i++)
      {
        spectrum_color->value[i] = 0.0;
      }
    }

    curve1->clear();

    busy = 0;

    return;
  }

  dftsz_spinbox->setMaximum(samples);

  if(buf1_sz != signalcomp->samples_on_screen + 32)
  {
    free(buf1);
    buf1 = (double *)malloc((sizeof(double) * signalcomp->samples_on_screen) + 32);
    if(buf1 == NULL)
    {
      buf1_sz = 0;
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.\n"
                                    "Decrease the timescale and try again.");
      messagewindow.exec();
      busy = 0;
      return;
    }
  }

  buf1_sz = signalcomp->samples_on_screen + 32;

  samples = 0;

  for(s=signalcomp->sample_start; s<signalcomp->samples_on_screen; s++)
  {
    if(s>=signalcomp->sample_stop)  break;

    dig_value = 0.0;
    s2 = s + signalcomp->sample_timeoffset - signalcomp->sample_start;

    for(j=0; j<signalcomp->num_of_signals; j++)
    {
      if(signalcomp->edfhdr->bdf)
      {
        var.two[0] = *((unsigned short *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)));

        var.four[2] = *((unsigned char *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset
          + ((s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record) * 3)
          + 2));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        f_tmp = var.one_signed;
      }

      if(signalcomp->edfhdr->edf)
      {
        f_tmp = *(((short *)(
          viewbuf
          + signalcomp->viewbufoffset
          + (signalcomp->edfhdr->recordsize * (s2 / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record))
          + signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].buf_offset))
          + (s2 % signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].smp_per_record));
      }

      f_tmp += signalcomp->edfhdr->edfparam[signalcomp->edfsignal[j]].offset;
      f_tmp *= signalcomp->factor[j];

      dig_value += f_tmp;
    }

    if(signalcomp->spike_filter)
    {
      if(s==signalcomp->sample_start)
      {
        spike_filter_restore_buf(signalcomp->spike_filter);
      }

      dig_value = run_spike_filter(dig_value, signalcomp->spike_filter);
    }

    for(k=0; k<signalcomp->filter_cnt; k++)
    {
      dig_value = first_order_filter(dig_value, signalcomp->filter[k]);
    }

    for(k=0; k<signalcomp->ravg_filter_cnt; k++)
    {
      if(s==signalcomp->sample_start)
      {
        ravg_filter_restore_buf(signalcomp->ravg_filter[k]);
      }

      dig_value = run_ravg_filter(dig_value, signalcomp->ravg_filter[k]);
    }

    for(k=0; k<signalcomp->fidfilter_cnt; k++)
    {
      if(s==signalcomp->sample_start)
      {
        memcpy(signalcomp->fidbuf[k], signalcomp->fidbuf2[k], fid_run_bufsize(signalcomp->fid_run[k]));
      }

      dig_value = signalcomp->fidfuncp[k](signalcomp->fidbuf[k], dig_value);
    }

    if(signalcomp->plif_ecg_filter)
    {
      if(s==signalcomp->sample_start)
      {
        if(mainwindow->edfheaderlist[signalcomp->filenum]->viewtime<=0)
        {
          plif_reset_subtract_filter(signalcomp->plif_ecg_filter, 0);
        }
        else
        {
          plif_subtract_filter_state_copy(signalcomp->plif_ecg_filter, signalcomp->plif_ecg_filter_sav);
        }
      }

      dig_value = plif_run_subtract_filter(dig_value, signalcomp->plif_ecg_filter);
    }

    if(signalcomp->ecg_filter != NULL)
    {
      if(s==signalcomp->sample_start)
      {
        ecg_filter_restore_buf(signalcomp->ecg_filter);
      }

      dig_value = run_ecg_filter(dig_value, signalcomp->ecg_filter);
    }

    if(s>=signalcomp->sample_start)
    {
      buf1[samples++] = dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;
    }
  }

  if(samples > buf1_sz)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (12345).");
    messagewindow.exec();
    busy = 0;
    return;
  }

  fft_inputbufsize = samples;

  dftsz_spinbox->setMaximum(fft_inputbufsize);

  samplefreq = (double)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record / ((double)signalcomp->edfhdr->long_data_record_duration / TIME_DIMENSION);

  if(dftblocksize & 1)
  {
    dftblocksize--;
  }

  if(dftblocksize > fft_inputbufsize)
  {
    dftblocksize = fft_inputbufsize;
  }

  if(dftblocksize & 1)
  {
    dftblocksize--;
  }

  free_fft_wrap(fft_data);
  fft_data = fft_wrap_create(buf1, fft_inputbufsize, dftblocksize, window_type);
  if(fft_data == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    busy = 0;
    return;
  }

  freqstep = samplefreq / (double)fft_data->dft_sz;

  steps = fft_data->sz_out;

  free(buf2);
  buf2 = (double *)calloc(1, sizeof(double) * fft_data->sz_out);
  if(buf2 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    free_fft_wrap(fft_data);
    fft_data = NULL;
    free(buf1);
    buf1 = NULL;
    buf1_sz = 0;
    busy = 0;
    return;
  }

  free(buf3);
  buf3 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf3 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    free_fft_wrap(fft_data);
    fft_data = NULL;
    free(buf1);
    free(buf2);
    buf1 = NULL;
    buf1_sz = 0;
    buf2 = NULL;
    busy = 0;
    return;
  }

  free(buf4);
  buf4 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf4 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    free_fft_wrap(fft_data);
    fft_data = NULL;
    free(buf1);
    buf1_sz = 0;
    free(buf2);
    free(buf3);
    buf1 = NULL;
    buf2 = NULL;
    buf3 = NULL;
    busy = 0;
    return;
  }

  free(buf5);
  buf5 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf5 == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    free_fft_wrap(fft_data);
    fft_data = NULL;
    free(buf1);
    buf1_sz = 0;
    free(buf2);
    free(buf3);
    free(buf4);
    buf1 = NULL;
    buf2 = NULL;
    buf3 = NULL;
    buf4 = NULL;
    busy = 0;
    return;
  }

  if(init_maxvalue && !set_settings)
  {
    maxvalue = 0.000001;
    maxvalue_sqrt = 0.000001;
    maxvalue_vlog = 0.000001;
    maxvalue_sqrt_vlog = 0.000001;
    minvalue_vlog = 0.0;
    minvalue_sqrt_vlog = 0.0;
  }

  if(set_settings)
  {
    maxvalue = settings.maxvalue;
    maxvalue_sqrt = settings.maxvalue_sqrt;
    maxvalue_vlog = settings.maxvalue_vlog;
    maxvalue_sqrt_vlog = settings.maxvalue_sqrt_vlog;
    minvalue_vlog = settings.minvalue_vlog;
    minvalue_sqrt_vlog = settings.minvalue_sqrt_vlog;
  }

  fft_wrap_run(fft_data);

  if(signalcomp->ecg_filter == NULL)
  {
    fft_data->buf_out[0] /= 2.0;  // DC!
  }
  else
  {
    fft_data->buf_out[0] = 0.0;  // Remove DC because heart rate is always a positive value
  }

  for(i=0; i<fft_data->sz_out; i++)
  {
    buf2[i] = fft_data->buf_out[i] / samplefreq;

    buf3[i] = sqrt(buf2[i] * freqstep);

    if(buf2[i] <= SPECT_LOG_MINIMUM)
    {
      buf4[i] = log10(SPECT_LOG_MINIMUM);
    }
    else
    {
      buf4[i] = log10(buf2[i]);
    }

    if(buf3[i] <= SPECT_LOG_MINIMUM)
    {
      buf5[i] = log10(SPECT_LOG_MINIMUM);
    }
    else
    {
      buf5[i] = log10(buf3[i]);
    }

    if(init_maxvalue && !set_settings)
    {
      if(i)  // don't use the dc-bin for the autogain of the screen
      {
        if(buf2[i] > maxvalue)
        {
          maxvalue = buf2[i];
        }

        if(buf3[i] > maxvalue_sqrt)
        {
          maxvalue_sqrt = buf3[i];
        }

        if(buf4[i] > maxvalue_vlog)
        {
          maxvalue_vlog = buf4[i];
        }

        if(buf5[i] > maxvalue_sqrt_vlog)
        {
          maxvalue_sqrt_vlog = buf5[i];
        }

        if((buf4[i] < minvalue_vlog) && (buf4[i] >= SPECT_LOG_MINIMUM_LOG))
        {
          minvalue_vlog = buf4[i];
        }

        if((buf5[i] < minvalue_sqrt_vlog) && (buf5[i] >= SPECT_LOG_MINIMUM_LOG))
        {
          minvalue_sqrt_vlog = buf5[i];
        }
      }
    }
  }

  if(init_maxvalue)
  {
    if(minvalue_vlog < SPECT_LOG_MINIMUM_LOG)
      minvalue_vlog = SPECT_LOG_MINIMUM_LOG;

    if(minvalue_sqrt_vlog < SPECT_LOG_MINIMUM_LOG)
      minvalue_sqrt_vlog = SPECT_LOG_MINIMUM_LOG;
  }

  if(buf1 != NULL)
  {
    free(buf1);
    buf1_sz = 0;
    buf1 = NULL;
  }

//  sprintf(str, "FFT resolution: %f Hz   %i blocks of %i samples", freqstep, dftblocks, dftblocksize);

  strcpy(str, "FFT resolution: ");
  convert_to_metric_suffix(str + strlen(str), freqstep, 3);
  remove_trailing_zeros(str);
  sprintf(str + strlen(str), "Hz   %i blocks of %i samples", fft_data->blocks, fft_data->dft_sz);
  curve1->setUpperLabel1(str);

  curve1->setUpperLabel2(signallabel);

  if(spectrum_color != NULL)
  {
    if(spectrum_color->items > 0)
    {
      spectrum_color->value[0] = 0.0;

      n = 0;

      for(j=0; j<steps; j++)
      {
        if(((freqstep * j) + (freqstep * 0.5)) < spectrum_color->freq[0])
        {
          if(spectrum_color->method == 0)  // sum
          {
            spectrum_color->value[0] += buf2[j];
          }

          if(spectrum_color->method == 1)  // peak
          {
            if(spectrum_color->value[0] < buf2[j])
            {
              spectrum_color->value[0] = buf2[j];
            }
          }

          if(spectrum_color->method == 2)  // average
          {
            spectrum_color->value[0] += buf2[j];

            n++;
          }
        }
      }

      if(spectrum_color->method == 2)  // average
      {
        if(n)
        {
          spectrum_color->value[0] /= n;
        }
      }
    }

    for(i=1; i < spectrum_color->items; i++)
    {
      spectrum_color->value[i] = 0.0;

      n = 0;

      for(j=0; j<steps; j++)
      {
        if((((freqstep * j) + (freqstep * 0.5)) > spectrum_color->freq[i-1]) && (((freqstep * j) + (freqstep * 0.5)) < spectrum_color->freq[i]))
        {
          if(spectrum_color->method == 0)  // sum
          {
            spectrum_color->value[i] += buf2[j];
          }

          if(spectrum_color->method == 1)  // peak
          {
            if(spectrum_color->value[i] < buf2[j])
            {
              spectrum_color->value[i] = buf2[j];
            }
          }

          if(spectrum_color->method == 2)  // average
          {
            spectrum_color->value[i] += buf2[j];

            n++;
          }
        }
      }

      if(spectrum_color->method == 2)  // average
      {
        if(n)
        {
          spectrum_color->value[i] /= n;
        }
      }
    }
  }

  if(mainwindow->spectrumdock_sqrt)
  {
    snprintf(str, 512, "Amplitude Spectrum %s", signallabel);
  }
  else
  {
    snprintf(str, 512, "Power Spectrum %s", signallabel);
  }

  dock->setWindowTitle(str);

  if(set_settings)
  {
    set_settings = 0;

    if((settings.amp >= 1) && (settings.amp <= 2000))
    {
      amplitudeSlider->setValue(settings.amp);
    }

    if((settings.log_min_sl >= 1) && (settings.log_min_sl <= 2000))
    {
      log_minslider->setValue(settings.log_min_sl);
    }

    if((settings.span >= 10) && (settings.span <= 1000))
    {
      spanSlider->setValue(settings.span);
    }

    if((settings.center >= 0) && (settings.center <= 1000))
    {
      centerSlider->setValue(settings.center);
    }

    if(settings.sqrt > 0)
    {
      sqrtButton->setChecked(true);
    }
    else
    {
      sqrtButton->setChecked(false);
    }

    if(settings.log > 0)
    {
      vlogButton->setChecked(true);

      log_minslider->setVisible(true);
    }
    else
    {
      vlogButton->setChecked(false);

      log_minslider->setVisible(false);
    }

    if(settings.colorbar > 0)
    {
      colorBarButton->setChecked(true);
    }
    else
    {
      colorBarButton->setChecked(false);
    }

    if((flywheel_value >= 10) && (flywheel_value <= 100000))
    {
      flywheel_value = settings.wheel;
    }
  }

  sliderMoved(0);

  curve1->setUpdatesEnabled(true);

  busy = 0;

  init_maxvalue = 0;
}



UI_SpectrumDockWindow::~UI_SpectrumDockWindow()
{
  free(buf1);
  buf1 = NULL;
  free(buf2);
  buf2 = NULL;
  free(buf3);
  buf3 = NULL;
  free(buf4);
  buf4 = NULL;
  free(buf5);
  buf5 = NULL;

  free_fft_wrap(fft_data);
  fft_data = NULL;

  delete SpectrumDialog;
}




















