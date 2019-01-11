/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "averager_curve_wnd.h"




UI_AverageCurveWindow::UI_AverageCurveWindow(struct signalcompblock *signal_comp, QWidget *w_parent, int number,
                                             double *abuf,
                                             double maxvalue,
                                             double minvalue,
                                             long long apagetime,
                                             long long samples_on_screen,
                                             int acnt,
                                             int trigger_position,
                                             char *annotation,
                                             int avg__period)
{
  char str[1024];


  averager_curve_dialog_is_destroyed = 0;

  class_is_deleted = 0;

  mainwindow = (UI_Mainwindow *)w_parent;

  signalcomp = signal_comp;

  averagecurvedialognumber = number;

  pagetime = apagetime;

  pagetime /= TIME_DIMENSION;

  avgbuf = abuf;

  avg_max_value = maxvalue;
  avg_min_value = minvalue;

  avg_samples_on_screen = samples_on_screen;

  avg_cnt = acnt;

  avg_trigger_position_ratio = trigger_position;

  avg_period = avg__period;

  strcpy(avg_annotation, annotation);

  flywheel_value = 1000;

  averager_curve_dialog = new QDialog;
  averager_curve_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  averager_curve_dialog->setMinimumSize(650, 480);
  averager_curve_dialog->setSizeGripEnabled(true);
  averager_curve_dialog->setModal(false);
  averager_curve_dialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  averager_curve_dialog->setWindowIcon(QIcon(":/images/edf.png"));

  strcpy(str, "Averaging  ");
  strcat(str, signalcomp->signallabel);
  averager_curve_dialog->setWindowTitle(str);

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::green);
  curve1->setBackgroundColor(Qt::black);
  curve1->setRasterColor(Qt::gray);
  curve1->setCrosshairColor(Qt::red);
  curve1->setTraceWidth(0);
  curve1->setH_label("sec");
  curve1->setLowerLabel("Time");
  curve1->setV_label(signalcomp->physdimension);
  curve1->setDashBoardEnabled(false);
  curve1->setMarker1Color(Qt::yellow);
  curve1->create_button("to EDF/BDF");

  sprintf(str, "Averaging %i triggers \"%s\"", avg_cnt, avg_annotation);
  curve1->setUpperLabel1(str);

  flywheel1 = new UI_Flywheel;
  flywheel1->setMinimumSize(20, 85);

  amplitudeSlider = new QSlider;
  amplitudeSlider->setOrientation(Qt::Vertical);
  amplitudeSlider->setMinimum(1);
  amplitudeSlider->setMaximum(2000);
  amplitudeSlider->setValue(1100);
  amplitudeSlider->setInvertedAppearance(true);
  amplitudeSlider->setMinimumSize(15, 280);

  amplitudeLabel = new QLabel;
  amplitudeLabel->setText("Amplitude");
  amplitudeLabel->setMinimumSize(100, 15);
  amplitudeLabel->setAlignment(Qt::AlignHCenter);

  inversionCheckBox = new QCheckBox("Invert");
  inversionCheckBox->setMinimumSize(70, 25);
  inversionCheckBox->setTristate(false);
  if(mainwindow->average_upsidedown == 1)
  {
    inversionCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    inversionCheckBox->setCheckState(Qt::Unchecked);
  }

  BWCheckBox = new QCheckBox("B/W");
  BWCheckBox->setMinimumSize(70, 25);
  BWCheckBox->setTristate(false);
  if(mainwindow->average_bw == 1)
  {
    BWCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    BWCheckBox->setCheckState(Qt::Unchecked);
  }

  vlayout3 = new QVBoxLayout;
  vlayout3->addStretch(100);
  vlayout3->addWidget(flywheel1, 100);
  vlayout3->addStretch(100);

  hlayout4 = new QHBoxLayout;
  hlayout4->addStretch(100);
  hlayout4->addLayout(vlayout3, 100);
  hlayout4->addStretch(100);
  hlayout4->addWidget(amplitudeSlider, 300);

  vlayout2 = new QVBoxLayout;
  vlayout2->setSpacing(20);
  vlayout2->addStretch(100);
  vlayout2->addWidget(amplitudeLabel, 0, Qt::AlignHCenter);
  vlayout2->addLayout(hlayout4, 200);
  vlayout2->addWidget(inversionCheckBox);
  vlayout2->addWidget(BWCheckBox);

  spanSlider = new QSlider;
  spanSlider->setOrientation(Qt::Horizontal);
  spanSlider->setMinimum(10);
  spanSlider->setMaximum(1000);
  spanSlider->setValue(1000);
  spanSlider->setMinimumSize(500, 15);

  spanLabel = new QLabel;
  spanLabel->setText("Span");
  spanLabel->setMinimumSize(110, 15);
  spanLabel->setAlignment(Qt::AlignHCenter);

  centerSlider = new QSlider;
  centerSlider->setOrientation(Qt::Horizontal);
  centerSlider->setMinimum(0);
  centerSlider->setMaximum(1000);
  centerSlider->setValue(250);
  centerSlider->setMinimumSize(500, 15);

  centerLabel = new QLabel;
  centerLabel->setText("Center");
  centerLabel->setMinimumSize(110, 15);
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

  averager_curve_dialog->setLayout(vlayout1);

  QObject::connect(amplitudeSlider,       SIGNAL(valueChanged(int)),      this, SLOT(sliderMoved(int)));
  QObject::connect(inversionCheckBox,     SIGNAL(stateChanged(int)),      this, SLOT(sliderMoved(int)));
  QObject::connect(BWCheckBox,            SIGNAL(stateChanged(int)),      this, SLOT(sliderMoved(int)));
  QObject::connect(spanSlider,            SIGNAL(valueChanged(int)),      this, SLOT(sliderMoved(int)));
  QObject::connect(centerSlider,          SIGNAL(valueChanged(int)),      this, SLOT(sliderMoved(int)));
  QObject::connect(averager_curve_dialog, SIGNAL(destroyed(QObject *)),   this, SLOT(averager_curve_dialogDestroyed(QObject *)));
  QObject::connect(curve1,                SIGNAL(extra_button_clicked()), this, SLOT(export_edf()));
  QObject::connect(flywheel1,             SIGNAL(dialMoved(int)),         this, SLOT(update_flywheel(int)));

  sliderMoved(0);

  averager_curve_dialog->show();
}


void UI_AverageCurveWindow::export_edf(void)
{
  int i, j, k, p,
      type,
      edf_hdl,
      smp_per_record,
      datarecords,
      smpls_left;

  char path[MAX_PATH_LENGTH],
       str[512];

  double *buf,
         frequency,
         frequency2;


  smp_per_record = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  datarecords = avg_samples_on_screen / smp_per_record;

  smpls_left = avg_samples_on_screen % smp_per_record;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  get_filename_from_path(path + strlen(path), signalcomp->edfhdr->filename, 512);
  remove_extension_from_filename(path);
  sprintf(path + strlen(path), " averaging %s %i triggers [%s]",
          signalcomp->signallabel,
          avg_cnt,
          avg_annotation);

  if(signalcomp->edfhdr->edf)
  {
    strcat(path, ".edf");

    strcpy(path, QFileDialog::getSaveFileName(0, "Save as EDF", QString::fromLocal8Bit(path), "EDF files (*.edf *.EDF)").toLocal8Bit().data());
  }
  else
  {
    strcat(path, ".bdf");

    strcpy(path, QFileDialog::getSaveFileName(0, "Save as BDF", QString::fromLocal8Bit(path), "BDF files (*.bdf *.BDF)").toLocal8Bit().data());
  }

  if(!strcmp(path, ""))
  {
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(signalcomp->edfhdr->edf)
  {
    edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, 1);
  }
  else
  {
    edf_hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_BDFPLUS, 1);
  }

  if(edf_hdl < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open output file for writing.");
    messagewindow.exec();
    return;
  }

  edf_set_samplefrequency(edf_hdl, 0, smp_per_record);

  if(edf_set_datarecord_duration(edf_hdl, signalcomp->edfhdr->long_data_record_duration / 100LL))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Datarecordduration out of range.");
    messagewindow.exec();
    return;
  }

  if(signalcomp->edfhdr->edf)
  {
    edf_set_digital_maximum(edf_hdl, 0, 32767);
    edf_set_digital_minimum(edf_hdl, 0, -32768);
  }
  else
  {
    edf_set_digital_maximum(edf_hdl, 0, 8388607);
    edf_set_digital_minimum(edf_hdl, 0, -8388608);
  }

  edf_set_physical_maximum(edf_hdl, 0, avg_max_value);
  edf_set_physical_minimum(edf_hdl, 0, avg_min_value);
  edf_set_label(edf_hdl, 0, signalcomp->signallabel);
  edf_set_physical_dimension(edf_hdl, 0, signalcomp->physdimension);

  p = 0;

  for(j=0; j<signalcomp->fidfilter_cnt; j++)
  {
    type = signalcomp->fidfilter_type[j];

    frequency = signalcomp->fidfilter_freq[j];

    frequency2 = signalcomp->fidfilter_freq2[j];

    if(type == 0)
    {
      p += sprintf(str + p, "HP:%f", frequency);
    }

    if(type == 1)
    {
      p += sprintf(str + p, "LP:%f", frequency);
    }

    if(type == 2)
    {
      p += sprintf(str + p, "N:%f", frequency);
    }

    if(type == 3)
    {
      p += sprintf(str + p, "BP:%f", frequency);
    }

    if(type == 4)
    {
      p += sprintf(str + p, "BS:%f", frequency);
    }

    for(k=(p-1); k>0; k--)
    {
      if(str[k]!='0')  break;
    }

    if(str[k]=='.')  str[k] = 0;
    else  str[k+1] = 0;

    p = strlen(str);

    if((type == 3) || (type == 4))
    {
      p += sprintf(str + p, "-%f", frequency2);

      for(k=(p-1); k>0; k--)
      {
        if(str[k]!='0')  break;
      }

      if(str[k]=='.')  str[k] = 0;
      else  str[k+1] = 0;
    }

    strcat(str, "Hz ");

    p = strlen(str);

    if(p>80)  break;
  }

  for(j=0; j<signalcomp->ravg_filter_cnt; j++)
  {
    if(signalcomp->ravg_filter_type[j] == 0)
    {
      p += sprintf(str + p, "HP:%iSmpls ", signalcomp->ravg_filter[j]->size);
    }

    if(signalcomp->ravg_filter_type[j] == 1)
    {
      p += sprintf(str + p, "LP:%iSmpls ", signalcomp->ravg_filter[j]->size);
    }

    p = strlen(str);

    if(p>80)  break;
  }

  strcat(str, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].prefilter);
  edf_set_prefilter(edf_hdl, 0, str);
  edf_set_transducer(edf_hdl, 0, signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].transducer);

  if((signalcomp->edfhdr->edfplus) || (signalcomp->edfhdr->bdfplus))
  {
    edf_set_patientname(edf_hdl, signalcomp->edfhdr->plus_patient_name);
    sprintf(str, "%i triggers \"%s\" averaged. %s", avg_cnt, avg_annotation, signalcomp->edfhdr->plus_recording_additional);
    edf_set_recording_additional(edf_hdl, str);
    edf_set_patientcode(edf_hdl, signalcomp->edfhdr->plus_patientcode);
    if(signalcomp->edfhdr->plus_gender[0] == 'M')
    {
      edf_set_gender(edf_hdl, 1);
    }
    if(signalcomp->edfhdr->plus_gender[0] == 'F')
    {
      edf_set_gender(edf_hdl, 0);
    }
    edf_set_patient_additional(edf_hdl, signalcomp->edfhdr->plus_patient_additional);
    edf_set_admincode(edf_hdl, signalcomp->edfhdr->plus_admincode);
    edf_set_technician(edf_hdl, signalcomp->edfhdr->plus_technician);
    edf_set_equipment(edf_hdl, signalcomp->edfhdr->plus_equipment);
  }
  else
  {
    edf_set_patientname(edf_hdl, signalcomp->edfhdr->patient);
    sprintf(str, "%i triggers \"%s\" averaged. %s", avg_cnt, avg_annotation, signalcomp->edfhdr->recording);
    edf_set_recording_additional(edf_hdl, str);
  }

  for(i=0; i<datarecords; i++)
  {
    edfwrite_physical_samples(edf_hdl, avgbuf + (i * smp_per_record));
  }

  if(smpls_left)
  {
    buf = (double *)calloc(1, smp_per_record * sizeof(double));
    if(buf == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error (buf).");
      messagewindow.exec();
    }
    else
    {
      for(i=0; i<smpls_left; i++)
      {
        buf[i] = avgbuf[(datarecords * smp_per_record) + i];
      }

      edfwrite_physical_samples(edf_hdl, buf);

      free(buf);
    }
  }

  edfwrite_annotation_latin1(edf_hdl,
                             (avg_period * 10) / avg_trigger_position_ratio,
                             -1,
                             avg_annotation);

  edfclose_file(edf_hdl);
}


void UI_AverageCurveWindow::sliderMoved(int)
{
  int startstep,
      stopstep,
      precision,
      spanstep,
      steps,
      markerstep;

  double max_sec,
         start_sec,
         avg_mid_value,
         avg_peak_value,
         dtmp1,
         dtmp2;


  steps = avg_samples_on_screen;

  spanstep = spanSlider->value() * steps / 1000;

  startstep = centerSlider->value() * (steps - spanstep) / 1000;

  stopstep = startstep + spanstep;

  avg_mid_value = (avg_max_value + avg_min_value) / 2.0;

  avg_peak_value = (avg_max_value - avg_min_value) / 2.0;

  avg_peak_value = (avg_peak_value * (double)amplitudeSlider->value() / 1000.0) * ((double)flywheel_value / 1000.0);

  dtmp1 = avg_mid_value + avg_peak_value;

  dtmp2 = avg_mid_value - avg_peak_value;

  if(inversionCheckBox->isChecked() == true)
  {
    curve1->setUpsidedownEnabled(true);

    mainwindow->average_upsidedown = 1;
  }
  else
  {
    curve1->setUpsidedownEnabled(false);

    mainwindow->average_upsidedown = 0;
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

    mainwindow->average_bw = 1;
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

    mainwindow->average_bw = 0;
  }

  curve1->drawCurve(avgbuf + startstep, stopstep - startstep, dtmp1, dtmp2);

  max_sec = pagetime * stopstep / steps - (pagetime * (1.0 / avg_trigger_position_ratio));

  precision = 0;
  if(max_sec < 100.0)
  {
    precision = 1;
  }
  if(max_sec < 10.0)
  {
    precision = 2;
  }
  if(max_sec < 1.0)
  {
    precision = 3;
  }
  if(max_sec < 0.1)
  {
    precision = 4;
  }

  start_sec = pagetime * startstep / steps - (pagetime * (1.0 / avg_trigger_position_ratio));

  curve1->setH_RulerValues(start_sec, max_sec);

  centerLabel->setText(QString::number(start_sec + ((max_sec - start_sec) / 2.0), 'f', precision).append(" sec").prepend("Center "));

  spanLabel->setText(QString::number(max_sec - start_sec, 'f', precision).append(" sec").prepend("Span "));

  if(avg_trigger_position_ratio > 0)
  {
    markerstep = steps / avg_trigger_position_ratio;

    if((markerstep > startstep) && (markerstep < stopstep))
    {
      curve1->setMarker1Enabled(true);

      curve1->setMarker1Position((((double)steps / (double)avg_trigger_position_ratio) - (double)startstep) / (double)spanstep);
    }
    else
    {
      curve1->setMarker1Enabled(false);
    }
  }
  else
  {
    curve1->setMarker1Enabled(false);
  }
}


void UI_AverageCurveWindow::averager_curve_dialogDestroyed(QObject *)
{
  averager_curve_dialog_is_destroyed = 1;

  mainwindow->averagecurvedialog[averagecurvedialognumber] = NULL;

  if(!class_is_deleted)
  {
    delete this;
  }
}


UI_AverageCurveWindow::~UI_AverageCurveWindow()
{
  int i;

  class_is_deleted = 1;

  if(!averager_curve_dialog_is_destroyed)
  {
    averager_curve_dialog->close();
  }

  if(avgbuf != NULL)
  {
    free(avgbuf);
  }

  mainwindow->averagecurvedialog[averagecurvedialognumber] = NULL;

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(signalcomp->avg_dialog[i] == (averagecurvedialognumber + 1))
    {
      signalcomp->avg_dialog[i] = 0;
    }
  }
}


void UI_AverageCurveWindow::update_flywheel(int new_value)
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



























