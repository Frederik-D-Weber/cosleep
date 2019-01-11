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



#include "spectrumanalyzer.h"


#define SPECT_LOG_MINIMUM (0.000001)
#define SPECT_LOG_MINIMUM_LOG (-5)




UI_FreqSpectrumWindow::UI_FreqSpectrumWindow(struct signalcompblock *signal_comp, char *view_buf, UI_FreqSpectrumWindow **spectrdialog, int number, QWidget *w_parent)
{
  int i;

  char str[1024];

  long long l_temp;

  fft_data = NULL;

  buf1 = NULL;
  buf2 = NULL;
  buf3 = NULL;
  buf4 = NULL;
  buf5 = NULL;

  busy = 0;

  first_run = 1;

  fft_inputbufsize = 0;

  spectrumdialog_is_destroyed = 0;

  class_is_deleted = 0;

  flywheel_value = 1050;

  spectrumdialog = spectrdialog;

  spectrumdialognumber = number;

  mainwindow = (UI_Mainwindow *)w_parent;

  viewbuf = view_buf;

  signalcomp = signal_comp;

  dftblocksize = mainwindow->maxdftblocksize;

  window_type = 0;

  for(i=strlen(signalcomp->edfhdr->filename); i>0; i--)
  {
       if((signalcomp->edfhdr->filename[i-1]=='/')||(signalcomp->edfhdr->filename[i-1]=='\\'))  break;
  }

  if((signalcomp->edfhdr->viewtime + signalcomp->edfhdr->starttime_offset)>=0LL)
  {
    if(signalcomp->alias[0] != 0)
    {
      snprintf(signallabel, 512, "   %s   %i:%02i:%02i.%04i   %s", signalcomp->alias,
                (int)((signalcomp->edfhdr->viewtime / TIME_DIMENSION)/ 3600LL),
                (int)(((signalcomp->edfhdr->viewtime / TIME_DIMENSION) % 3600LL) / 60LL),
                (int)((signalcomp->edfhdr->viewtime / TIME_DIMENSION) % 60LL),
                (int)((signalcomp->edfhdr->viewtime % TIME_DIMENSION) / 1000LL),
                signalcomp->edfhdr->filename + i);
    }
    else
    {
      snprintf(signallabel, 512, "   %s   %i:%02i:%02i.%04i   %s", signalcomp->signallabel,
                (int)((signalcomp->edfhdr->viewtime / TIME_DIMENSION)/ 3600LL),
                (int)(((signalcomp->edfhdr->viewtime / TIME_DIMENSION) % 3600LL) / 60LL),
                (int)((signalcomp->edfhdr->viewtime / TIME_DIMENSION) % 60LL),
                (int)((signalcomp->edfhdr->viewtime % TIME_DIMENSION) / 1000LL),
                signalcomp->edfhdr->filename + i);
    }
  }
  else
  {
    l_temp = signalcomp->edfhdr->l_starttime + ((signalcomp->edfhdr->viewtime + signalcomp->edfhdr->starttime_offset) % (86400LL * TIME_DIMENSION));
    if(l_temp<=0)
    {
      l_temp = (86400LL * TIME_DIMENSION) + l_temp;
    }

    l_temp = -signalcomp->edfhdr->viewtime;

    if(signalcomp->alias[0] != 0)
    {
      snprintf(signallabel, 512, "   %s   -%i:%02i:%02i.%04i   %s", signalcomp->alias,
              (int)((l_temp / TIME_DIMENSION)/ 3600LL),
              (int)(((l_temp / TIME_DIMENSION) % 3600LL) / 60LL),
              (int)((l_temp / TIME_DIMENSION) % 60LL),
              (int)((l_temp % TIME_DIMENSION) / 1000LL),
              signalcomp->edfhdr->filename + i);
    }
    else
    {
      snprintf(signallabel, 512, "   %s   -%i:%02i:%02i.%04i   %s", signalcomp->signallabel,
              (int)((l_temp / TIME_DIMENSION)/ 3600LL),
              (int)(((l_temp / TIME_DIMENSION) % 3600LL) / 60LL),
              (int)((l_temp / TIME_DIMENSION) % 60LL),
              (int)((l_temp % TIME_DIMENSION) / 1000LL),
              signalcomp->edfhdr->filename + i);
    }
  }

  remove_trailing_zeros(signallabel);

  strcpy(physdimension, signalcomp->physdimension);

  SpectrumDialog = new QDialog();
  SpectrumDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  SpectrumDialog->setMinimumSize(650, 530);
  SpectrumDialog->setSizeGripEnabled(true);
  SpectrumDialog->setModal(false);
  SpectrumDialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  if(mainwindow->spectrum_sqrt)
  {
    snprintf(str, 512, "Amplitude Spectrum %s", signallabel);
  }
  else
  {
    snprintf(str, 512, "Power Spectral Density %s", signallabel);
  }
  SpectrumDialog->setWindowTitle(str);
  SpectrumDialog->setWindowIcon(QIcon(":/images/edf.png"));

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::green);
  curve1->setBackgroundColor(Qt::black);
  curve1->setRasterColor(Qt::gray);
  curve1->setTraceWidth(0);
  curve1->setH_label("Hz");
  curve1->setLowerLabel("Frequency");
  if(mainwindow->spectrum_sqrt)
  {
    if(mainwindow->spectrum_vlog)
    {
      sprintf(str, "log10(%s)", physdimension);

      curve1->setV_label(str);
    }
    else
    {
      curve1->setV_label(physdimension);
    }
  }
  else
  {
    if(mainwindow->spectrum_vlog)
    {
      sprintf(str, "log10((%s)^2/Hz)", physdimension);
    }
    else
    {
      sprintf(str, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str);
  }
  curve1->create_button("to Text");
  curve1->setDashBoardEnabled(false);

  flywheel1 = new UI_Flywheel;
  flywheel1->setMinimumSize(20, 85);

  amplitudeSlider = new QSlider;
  amplitudeSlider->setOrientation(Qt::Vertical);
  amplitudeSlider->setMinimum(1);
  amplitudeSlider->setMaximum(2000);
  amplitudeSlider->setValue(1000);
  amplitudeSlider->setInvertedAppearance(true);
  amplitudeSlider->setMinimumSize(15, 280);

  log_minslider = new QSlider;
  log_minslider->setOrientation(Qt::Vertical);
  log_minslider->setMinimum(1);
  log_minslider->setMaximum(2000);
  log_minslider->setValue(1000);
  log_minslider->setInvertedAppearance(false);
  log_minslider->setMinimumSize(15, 280);

  amplitudeLabel = new QLabel;
  amplitudeLabel->setText("Amplitude");
  amplitudeLabel->setMinimumSize(100, 25);
  amplitudeLabel->setAlignment(Qt::AlignHCenter);

  sqrtCheckBox = new QCheckBox("Amplitude");
  sqrtCheckBox->setMinimumSize(70, 25);
  sqrtCheckBox->setTristate(false);
  if(mainwindow->spectrum_sqrt)
  {
    sqrtCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    sqrtCheckBox->setCheckState(Qt::Unchecked);
  }

  VlogCheckBox = new QCheckBox("Log");
  VlogCheckBox->setMinimumSize(70, 25);
  VlogCheckBox->setTristate(false);
  if(mainwindow->spectrum_vlog)
  {
    VlogCheckBox->setCheckState(Qt::Checked);

    log_minslider->setVisible(true);
  }
  else
  {
    VlogCheckBox->setCheckState(Qt::Unchecked);

    log_minslider->setVisible(false);
  }

  BWCheckBox = new QCheckBox("B/W");
  BWCheckBox->setMinimumSize(70, 25);
  BWCheckBox->setTristate(false);
  if(mainwindow->spectrum_bw == 1)
  {
    BWCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    BWCheckBox->setCheckState(Qt::Unchecked);
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
  vlayout2->setSpacing(10);
  vlayout2->addStretch(100);
  vlayout2->addWidget(amplitudeLabel, 0, Qt::AlignHCenter);
  vlayout2->addLayout(hlayout4, 200);
  vlayout2->addWidget(sqrtCheckBox);
  vlayout2->addWidget(VlogCheckBox);
  vlayout2->addWidget(BWCheckBox);
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
  spanLabel->setMinimumSize(110, 25);
  spanLabel->setAlignment(Qt::AlignHCenter);

  centerSlider = new QSlider;
  centerSlider->setOrientation(Qt::Horizontal);
  centerSlider->setMinimum(0);
  centerSlider->setMaximum(1000);
  centerSlider->setValue(0);
  centerSlider->setMinimumSize(500, 15);

  centerLabel = new QLabel;
  centerLabel->setText("Center");
  centerLabel->setMinimumSize(110, 25);
  centerLabel->setAlignment(Qt::AlignHCenter);

  hlayout1 = new QHBoxLayout;
  hlayout1->setSpacing(20);
  hlayout1->addLayout(vlayout2);
  hlayout1->addWidget(curve1, 100);

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
  t1->start(10);

  QObject::connect(t1,              SIGNAL(timeout()),                this, SLOT(update_curve()));
  QObject::connect(amplitudeSlider, SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(log_minslider,   SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(spanSlider,      SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(centerSlider,    SIGNAL(valueChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(sqrtCheckBox,    SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(VlogCheckBox,    SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(BWCheckBox,      SIGNAL(stateChanged(int)),        this, SLOT(sliderMoved(int)));
  QObject::connect(SpectrumDialog,  SIGNAL(destroyed(QObject *)),     this, SLOT(SpectrumDialogDestroyed(QObject *)));
  QObject::connect(curve1,          SIGNAL(extra_button_clicked()),   this, SLOT(print_to_txt()));
  QObject::connect(flywheel1,       SIGNAL(dialMoved(int)),           this, SLOT(update_flywheel(int)));
  QObject::connect(this,            SIGNAL(finished()),               this, SLOT(thr_finished_func()));
  QObject::connect(dftsz_spinbox,   SIGNAL(valueChanged(int)),        this, SLOT(dftsz_value_changed(int)));
  QObject::connect(windowBox,       SIGNAL(currentIndexChanged(int)), this, SLOT(windowBox_changed(int)));

  SpectrumDialog->show();

  SpectrumDialog->resize(650, 530);
}


void UI_FreqSpectrumWindow::windowBox_changed(int idx)
{
  if(busy)  return;

  if(window_type == idx)  return;

  window_type = idx;

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::dftsz_value_changed(int new_val)
{
  if(busy)  return;

  if(dftblocksize == new_val)  return;

  dftblocksize = new_val;

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::update_flywheel(int new_value)
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


void UI_FreqSpectrumWindow::print_to_txt()
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

  sprintf(str, "FFT Power Spectral Density log10(Power/%fHz)\n", freqstep);
  remove_trailing_zeros(str);
  fprintf(outputfile, "%s", str);
  if(signalcomp->alias[0] != 0)
  {
    fprintf(outputfile, "Signal: %s\n", signalcomp->alias);
  }
  else
  {
    fprintf(outputfile, "Signal: %s\n", signalcomp->signallabel);
  }
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


void UI_FreqSpectrumWindow::sliderMoved(int)
{
  long long startstep,
            stopstep,
            spanstep;

  double max_freq,
         start_freq;

  char str[1024];


  if(VlogCheckBox->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_vlog = 1;

    log_minslider->setVisible(true);
  }
  else
  {
    mainwindow->spectrum_vlog = 0;

    log_minslider->setVisible(false);
  }

  if(sqrtCheckBox->checkState() == Qt::Checked)
  {
    mainwindow->spectrum_sqrt = 1;

    sprintf(str, "Amplitude Spectrum %s", signallabel);

    SpectrumDialog->setWindowTitle(str);

    if(mainwindow->spectrum_vlog)
    {
      sprintf(str, "log10(%s)", physdimension);

      curve1->setV_label(str);
    }
    else
    {
      curve1->setV_label(physdimension);
    }
  }
  else
  {
    mainwindow->spectrum_sqrt = 0;

    sprintf(str, "Power Spectral Density %s", signallabel);

    SpectrumDialog->setWindowTitle(str);

    if(mainwindow->spectrum_vlog)
    {
      sprintf(str, "log10((%s)^2/Hz)", physdimension);
    }
    else
    {
      sprintf(str, "(%s)^2/Hz", physdimension);
    }

    curve1->setV_label(str);
  }

  if(BWCheckBox->isChecked() == true)
  {
    curve1->setSignalColor(Qt::black);
    curve1->setBackgroundColor(Qt::white);
    curve1->setRasterColor(Qt::black);
    curve1->setMarker1Color(Qt::black);
    curve1->setBorderColor(Qt::white);
    curve1->setTextColor(Qt::black);
    curve1->setCrosshairColor(Qt::black);

    mainwindow->spectrum_bw = 1;
  }
  else
  {
    curve1->setSignalColor(Qt::green);
    curve1->setBackgroundColor(Qt::black);
    curve1->setRasterColor(Qt::gray);
    curve1->setMarker1Color(Qt::yellow);
    curve1->setBorderColor(Qt::lightGray);
    curve1->setTextColor(Qt::black);
    curve1->setCrosshairColor(Qt::red);

    mainwindow->spectrum_bw = 0;
  }

  spanstep = (long long)spanSlider->value() * (long long)fft_data->sz_out / 1000LL;

  startstep = (long long)centerSlider->value() * ((long long)fft_data->sz_out - spanstep) / 1000LL;

  stopstep = startstep + spanstep;

  if(mainwindow->spectrum_sqrt)
  {
    if(mainwindow->spectrum_vlog)
    {
      curve1->drawCurve(buf5 + startstep, stopstep - startstep, (maxvalue_sqrt_vlog * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, minvalue_sqrt_vlog * (double)log_minslider->value() / 1000.0);
    }
    else
    {
      curve1->drawCurve(buf3 + startstep, stopstep - startstep, (maxvalue_sqrt * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, 0.0);
    }
  }
  else
  {
    if(mainwindow->spectrum_vlog)
    {
      curve1->drawCurve(buf4 + startstep, stopstep - startstep, (maxvalue_vlog * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, minvalue_vlog * (double)log_minslider->value() / 1000.0);
    }
    else
    {
      curve1->drawCurve(buf2 + startstep, stopstep - startstep, (maxvalue * ((double)flywheel_value / 1000.0) * (double)amplitudeSlider->value()) / 1000.0, 0.0);
    }
  }

  max_freq = ((double)samplefreq / 2.0) * stopstep / fft_data->sz_out;

  start_freq = ((double)samplefreq / 2.0) * startstep / fft_data->sz_out;

//   printf("steps: %i\n"
//          "startstep: %lli\n"
//          "stopstep: %lli\n"
//          "spanstep: %lli\n"
//          "samplefreq: %f\n"
//          "max_freq: %f\n"
//          "start_freq: %f\n",
//          fft_data->sz_out,
//          startstep,
//          stopstep,
//          spanstep,
//          samplefreq,
//          max_freq,
//          start_freq);

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


void UI_FreqSpectrumWindow::run()
{
  int i, j, k;

  long long s, s2;

  double dig_value=0.0,
         f_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  if(first_run)
  {
    first_run = 0;

    fft_inputbufsize = samples;

    free(buf1);
    buf1 = (double *)malloc(sizeof(double) * fft_inputbufsize + 16);
    if(buf1 == NULL)
    {
      malloc_err = 1;
      return;
    }

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

    if(samples > fft_inputbufsize)
    {
      malloc_err = 2;
      return;
    }

    samplefreq = (double)signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record / ((double)signalcomp->edfhdr->long_data_record_duration / TIME_DIMENSION);

    if(dftblocksize > samples)
    {
      dftblocksize = samples;
    }

    if(dftblocksize & 1)
    {
      dftblocksize--;
    }
  }  // end of first_run

  free_fft_wrap(fft_data);
  fft_data = fft_wrap_create(buf1, fft_inputbufsize, dftblocksize, window_type);
  if(fft_data == NULL)
  {
//     printf("buf1: %p   fft_inputbufsize: %i   dftblocksize: %i   window_type: %i\n",
//            buf1, fft_inputbufsize, dftblocksize, window_type);

    malloc_err = 3;
    free(buf1);
    buf1 = NULL;
    return;
  }

  freqstep = samplefreq / (double)fft_data->dft_sz;

  free(buf2);
  buf2 = (double *)calloc(1, sizeof(double) * fft_data->sz_out);
  if(buf2 == NULL)
  {
    malloc_err = 4;
    free(buf1);
    buf1 = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf3);
  buf3 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf3 == NULL)
  {
    malloc_err = 5;
    free(buf1);
    free(buf2);
    buf1 = NULL;
    buf2 = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf4);
  buf4 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf4 == NULL)
  {
    malloc_err = 6;
    free(buf1);
    free(buf2);
    free(buf3);
    buf1 = NULL;
    buf2 = NULL;
    buf3 = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  free(buf5);
  buf5 = (double *)malloc(sizeof(double) * fft_data->sz_out);
  if(buf5 == NULL)
  {
    malloc_err = 7;
    free(buf1);
    free(buf2);
    free(buf3);
    free(buf4);
    buf1 = NULL;
    buf2 = NULL;
    buf3 = NULL;
    buf4 = NULL;
    free_fft_wrap(fft_data);
    fft_data = NULL;
    return;
  }

  maxvalue = 0.000001;
  maxvalue_sqrt = 0.000001;
  maxvalue_vlog = 0.000001;
  maxvalue_sqrt_vlog = 0.000001;
  minvalue_vlog = 0.0;
  minvalue_sqrt_vlog = 0.0;

#ifdef CHECK_POWERSPECTRUM
  printf("samples is %i   dftblocksize is %i   dftblocks is %i    samplesleft is %i   fft_outputbufsize is %i\n", samples, dftblocksize, fft_data->blocks, fft_data->smpls_left, fft_data->sz_out);

  double power1=0.0, power2=0.0;

  for(i=0; i<samples; i++)
  {
    power1 += (buf1[i] * buf1[i]);
  }
#endif

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

#ifdef CHECK_POWERSPECTRUM
    power2 += buf2[i];
#endif

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

  if(minvalue_vlog < SPECT_LOG_MINIMUM_LOG)
    minvalue_vlog = SPECT_LOG_MINIMUM_LOG;

  if(minvalue_sqrt_vlog < SPECT_LOG_MINIMUM_LOG)
    minvalue_sqrt_vlog = SPECT_LOG_MINIMUM_LOG;

#ifdef CHECK_POWERSPECTRUM
  power1 /= samples;
  power2 *= freqstep;

  printf("\n power1 is %f\n power2 is %f\n\n", power1, power2);
#endif
}


void UI_FreqSpectrumWindow::update_curve()
{
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

  samples = signalcomp->samples_on_screen;

  if(signalcomp->samples_on_screen > signalcomp->sample_stop)
  {
    samples = signalcomp->sample_stop;
  }

  samples -= signalcomp->sample_start;

  if((samples < 10) || (viewbuf == NULL))
  {
    curve1->clear();

    return;
  }

  dftsz_spinbox->setMaximum(samples);

  busy = 1;

  malloc_err = 0;

  curve1->setUpdatesEnabled(false);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  start();
}


void UI_FreqSpectrumWindow::thr_finished_func()
{
  char str[4096];

  if(spectrumdialog_is_destroyed)
  {
    return;
  }

  if(malloc_err)
  {
    QApplication::restoreOverrideCursor();

    sprintf(str,
            "The system was not able to provide enough resources (memory) to perform the requested action. (%i)",
            malloc_err);

    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();

    curve1->clear();

    busy = 0;

    return;
  }

  strcpy(str, "FFT resolution: ");
  convert_to_metric_suffix(str + strlen(str), freqstep, 3);
  remove_trailing_zeros(str);
  sprintf(str + strlen(str), "Hz   %i blocks of %i samples", fft_data->blocks, fft_data->dft_sz);

  curve1->setUpperLabel1(str);

  curve1->setUpperLabel2(signallabel);

  sliderMoved(0);

  curve1->setUpdatesEnabled(true);

  busy = 0;

  QApplication::restoreOverrideCursor();
}


void UI_FreqSpectrumWindow::SpectrumDialogDestroyed(QObject *)
{
  spectrumdialog_is_destroyed = 1;

  QObject::disconnect(this, SIGNAL(finished()), this, SLOT(thr_finished_func()));

  if(isRunning())
  {
    wait(ULONG_MAX);

    QApplication::restoreOverrideCursor();
  }

  spectrumdialog[spectrumdialognumber] = NULL;

  if(!class_is_deleted)
  {
    delete this;
  }
}


UI_FreqSpectrumWindow::~UI_FreqSpectrumWindow()
{
  int i;

  class_is_deleted = 1;

  if(!spectrumdialog_is_destroyed)
  {
    SpectrumDialog->close();
  }

  free(buf1);
  free(buf2);
  free(buf3);
  free(buf4);
  free(buf5);
  buf1 = NULL;
  buf2 = NULL;
  buf3 = NULL;
  buf4 = NULL;
  buf5 = NULL;

  free_fft_wrap(fft_data);
  fft_data = NULL;

  spectrumdialog[spectrumdialognumber] = NULL;

  for(i=0; i<MAXSPECTRUMDIALOGS; i++)
  {
    if(signalcomp->spectr_dialog[i] == spectrumdialognumber + 1)
    {
      signalcomp->spectr_dialog[i] = 0;

      break;
    }
  }
}



















