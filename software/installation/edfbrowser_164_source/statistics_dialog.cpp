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




#include "statistics_dialog.h"


#define STAT_JOB_SRC_SIGNAL 0
#define STAT_JOB_SRC_ECG    1
#define STAT_JOB_SRC_ANNOT  2

#define BEAT_IVAL_SIZE   262144




UI_StatisticWindow::UI_StatisticWindow(struct signalcompblock *signalcomp,
                                       long long pagetime,
                                       struct annotation_list *annot_list,
                                       struct annotationblock *annot)
{
  int i,
      tmp,
      NN20,
      pNN20,
      NN50,
      pNN50,
      job_src=0;

  char stat_str[2048]={""};

  double d_tmp,
         average_bpm,
         average_rr,
         sdnn_bpm,
         sdnn_rr,
         *buf_bpm,
         rmssd_rr,
         *beat_interval_list=NULL;

  long long l_tmp=0;

  struct annotationblock *tmp_annot;

  StatDialog = new QDialog;
  StatDialog->setWindowTitle("Statistics");
  StatDialog->setModal(true);
  StatDialog->setAttribute(Qt::WA_DeleteOnClose, true);
  StatDialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  StatDialog->setWindowIcon(QIcon(":/images/edf.png"));

  if(signalcomp != NULL)
  {
    if(signalcomp->ecg_filter != NULL)
    {
      job_src = STAT_JOB_SRC_ECG;
    }
    else
    {
      job_src = STAT_JOB_SRC_SIGNAL;
    }
  }
  else
  {
    job_src = STAT_JOB_SRC_ANNOT;

    beat_interval_list = (double *)malloc(sizeof(double) * BEAT_IVAL_SIZE);
  }

  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    StatDialog->setMinimumSize(600, 400);
    StatDialog->setSizeGripEnabled(true);

    startSlider = new QSlider;
    startSlider->setOrientation(Qt::Horizontal);
    startSlider->setMinimum(0);
    startSlider->setMaximum(295);
    startSlider->setValue(0);

    stopSlider = new QSlider;
    stopSlider->setOrientation(Qt::Horizontal);
    stopSlider->setMinimum(5);
    stopSlider->setMaximum(300);
    stopSlider->setValue(300);

    curve1 = new SignalCurve(StatDialog);
    curve1->setSignalColor(Qt::darkGreen);
    curve1->setBackgroundColor(Qt::black);
    curve1->setRasterColor(Qt::gray);
    curve1->setTraceWidth(0);
    if(job_src == STAT_JOB_SRC_ECG)
    {
      curve1->setH_label(signalcomp->physdimension);
    }
    curve1->setLowerLabel("HR (beats/min)");
    curve1->setDashBoardEnabled(false);
    if(job_src == STAT_JOB_SRC_ANNOT)
    {
      strcpy(stat_str, "Distribution ");
      strcat(stat_str, annot->annotation);
      curve1->setUpperLabel1(stat_str);
    }
    else
    {
      curve1->setUpperLabel1("Distribution");
    }
    curve1->setFillSurfaceEnabled(true);

    vlayout2_1 = new QVBoxLayout;
    vlayout2_1->setSpacing(20);
    vlayout2_1->addWidget(curve1);
    vlayout2_1->addWidget(startSlider);
    vlayout2_1->addWidget(stopSlider);
  }

  if(job_src == STAT_JOB_SRC_SIGNAL)
  {
    StatDialog->setMinimumSize(300, 400);
    StatDialog->setMaximumSize(300, 400);
  }

  Label1 = new QLabel(StatDialog);
  Label1->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  pushButton1 = new QPushButton(StatDialog);
  pushButton1->setMinimumSize(100, 25);
  pushButton1->setText("&Close");

  hlayout1_1_1 = new QHBoxLayout;
  hlayout1_1_1->addWidget(pushButton1);
  hlayout1_1_1->addStretch(100);

  vlayout1_1 = new QVBoxLayout;
  vlayout1_1->setSpacing(20);
  vlayout1_1->addWidget(Label1);
  vlayout1_1->addStretch(100);
  vlayout1_1->addLayout(hlayout1_1_1);

  hlayout1 = new QHBoxLayout;
  hlayout1->addLayout(vlayout1_1, 1);
  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    hlayout1->addLayout(vlayout2_1, 100);
  }

  StatDialog->setLayout(hlayout1);

  QObject::connect(pushButton1, SIGNAL(clicked()), StatDialog, SLOT(close()));

  for(i=0; i<300; i++)
  {
    bpm_distribution[i] = 0;
  }

  if(job_src == STAT_JOB_SRC_SIGNAL)
  {
    if((signalcomp->stat_cnt < 1) || (pagetime < 10LL))
    {
      if(signalcomp->alias[0] != 0)
      {
        sprintf(stat_str, "Signal:  %s\n\nSamples:   0\n\nSum:       0 %s\n\nMean:      0 %s\n\nRMS:       0 %s\n\nMRS:       0 %s\n\nZero crossings:  0\n\nFrequency:  0 Hz",
                signalcomp->alias,
                signalcomp->physdimension,
                signalcomp->physdimension,
                signalcomp->physdimension,
                signalcomp->physdimension);
      }
      else
      {
        sprintf(stat_str, "Signal:  %s\n\nSamples:   0\n\nSum:       0 %s\n\nMean:      0 %s\n\nRMS:       0 %s\n\nMRS:       0 %s\n\nZero crossings:  0\n\nFrequency:  0 Hz",
                signalcomp->signallabel,
                signalcomp->physdimension,
                signalcomp->physdimension,
                signalcomp->physdimension,
                signalcomp->physdimension);
      }
    }
    else
    {
      if(signalcomp->alias[0] != 0)
      {
        sprintf(stat_str, "Signal:  %s\n\nSamples:   %i\n\nSum:       %f %s\n\nMean:      %f %s\n\nRMS:       %f %s\n\nMRS:       %f %s\n\nZero crossings:  %i\n\nFrequency:  %f Hz",
                signalcomp->alias,
                signalcomp->stat_cnt,
                signalcomp->stat_sum * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                (signalcomp->stat_sum * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue) / signalcomp->stat_cnt,
                signalcomp->physdimension,
                sqrt(signalcomp->stat_sum_sqr / signalcomp->stat_cnt) * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                (signalcomp->stat_sum_rectified / signalcomp->stat_cnt) * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                signalcomp->stat_zero_crossing_cnt,
                (((double)signalcomp->stat_zero_crossing_cnt / 2.0)) / ((double)pagetime / (double)TIME_DIMENSION)
              );
      }
      else
      {
        sprintf(stat_str, "Signal:  %s\n\nSamples:   %i\n\nSum:       %f %s\n\nMean:      %f %s\n\nRMS:       %f %s\n\nMRS:       %f %s\n\nZero crossings:  %i\n\nFrequency:  %f Hz",
                signalcomp->signallabel,
                signalcomp->stat_cnt,
                signalcomp->stat_sum * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                (signalcomp->stat_sum * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue) / signalcomp->stat_cnt,
                signalcomp->physdimension,
                sqrt(signalcomp->stat_sum_sqr / signalcomp->stat_cnt) * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                (signalcomp->stat_sum_rectified / signalcomp->stat_cnt) * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue,
                signalcomp->physdimension,
                signalcomp->stat_zero_crossing_cnt,
                (((double)signalcomp->stat_zero_crossing_cnt / 2.0)) / ((double)pagetime / (double)TIME_DIMENSION)
              );
      }
    }
  }

  if((job_src == STAT_JOB_SRC_ECG) || (job_src == STAT_JOB_SRC_ANNOT))
  {
    if(job_src == STAT_JOB_SRC_ECG)
    {
      beat_cnt = ecg_filter_get_beat_cnt(signalcomp->ecg_filter);

      beat_interval_list = ecg_filter_get_interval_beatlist(signalcomp->ecg_filter);
    }

    if(job_src == STAT_JOB_SRC_ANNOT)
    {
      QProgressDialog progress("Processing...", "Abort", 0, 1);
      progress.setWindowModality(Qt::WindowModal);
      progress.setMinimumDuration(200);
      progress.reset();

      progress.setRange(0, edfplus_annotation_size(annot_list));
      progress.setValue(0);

      int p_i=0, p_j=0;

      for(i=0, beat_cnt=0; beat_cnt<BEAT_IVAL_SIZE; i++)
      {
        tmp_annot = edfplus_annotation_get_item_visible_only_cached(annot_list, i, &p_i, &p_j);

        if(tmp_annot == NULL)  break;

        if(!strcmp(tmp_annot->annotation, annot->annotation))
        {
          if(beat_cnt)
          {
            beat_interval_list[beat_cnt - 1] = ((double)(tmp_annot->onset - l_tmp)) / (double)TIME_DIMENSION;
          }

          l_tmp = tmp_annot->onset;

          beat_cnt++;
        }

        if(!(i%1000))
        {
          progress.setValue(i);

          qApp->processEvents();

          if(progress.wasCanceled() == true)
          {
            break;
          }
        }
      }

      if(beat_cnt)  beat_cnt--;

      progress.reset();
    }

    if(beat_cnt < 3)
    {
      sprintf(stat_str, "Not enough beats.");
    }
    else
    {
      average_bpm = 0.0;
      average_rr = 0.0;
      sdnn_bpm = 0.0;
      sdnn_rr = 0.0;
      rmssd_rr = 0.0;
      NN20 = 0;
      NN50 = 0;

      buf_bpm = (double *)malloc(sizeof(double) * beat_cnt);
      if(buf_bpm == NULL)
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
        messagewindow.exec();
        return;
      }

      for(i=0; i<beat_cnt; i++)
      {
        buf_bpm[i] = 60.0 / beat_interval_list[i];

        average_bpm += buf_bpm[i];
        average_rr += beat_interval_list[i];

        if(i < (beat_cnt - 1))
        {
          d_tmp = (beat_interval_list[i] - beat_interval_list[i + 1]) * 1000.0;

          rmssd_rr += (d_tmp * d_tmp);

          if(((beat_interval_list[i] - beat_interval_list[i + 1]) > 0.02 ) || ((beat_interval_list[i + 1] - beat_interval_list[i]) > 0.02 ))
          {
            NN20++;
          }

          if(((beat_interval_list[i] - beat_interval_list[i + 1]) > 0.05 ) || ((beat_interval_list[i + 1] - beat_interval_list[i]) > 0.05 ))
          {
            NN50++;
          }
        }
      }

      average_bpm /= beat_cnt;
      average_rr /= beat_cnt;
      rmssd_rr /= beat_cnt;
      rmssd_rr = sqrt(rmssd_rr);

      pNN20 = (NN20 * 100) / (beat_cnt - 1);
      pNN50 = (NN50 * 100) / (beat_cnt - 1);

      for(i=0; i<beat_cnt; i++)
      {
        sdnn_bpm += (buf_bpm[i] - average_bpm) * (buf_bpm[i] - average_bpm);
        sdnn_rr += (beat_interval_list[i] - average_rr) * (beat_interval_list[i] - average_rr);
      }

      sdnn_bpm = sqrt(sdnn_bpm / beat_cnt);
      sdnn_rr = sqrt(sdnn_rr / beat_cnt);

      sprintf(stat_str,
              "Heart Rate\n\n"
              "Beats:    %3i\n\n"
              "Mean RR:  %3i ms\n\n"
              "SDNN RR:  %3i ms\n\n"
              "RMSSD RR: %3i ms\n\n"
              "Mean HR:  %3.3f bpm\n\n"
              "SDNN HR:  %3.3f bpm\n\n"
              "NN20:     %3i\n\n"
              "pNN20:    %3i %%\n\n"
              "NN50:     %3i\n\n"
              "pNN50:    %3i %%\n\n",
              beat_cnt,
              (int)(average_rr * 1000.0),
              (int)(sdnn_rr * 1000.0),
              (int)rmssd_rr,
              average_bpm,
              sdnn_bpm,
              NN20,
              pNN20,
              NN50,
              pNN50);

      free(buf_bpm);

      for(i=0; i<beat_cnt; i++)
      {
        tmp = 60.0 / beat_interval_list[i];

        if((tmp > 0) && (tmp < 301))
        {
          bpm_distribution[tmp-1]++;
        }
      }

      max_val = 1;

      for(i=0; i<300; i++)
      {
        if(bpm_distribution[i] > max_val)
        {
          max_val = bpm_distribution[i];
        }
      }

      for(i=0; i<300; i++)
      {
        if(bpm_distribution[i] > (max_val / 70))
        {
          start_ruler = i;

          break;
        }
      }

      for(i=299; i>=0; i--)
      {
        if(bpm_distribution[i] > (max_val / 70))
        {
          end_ruler = i + 1;

          if(end_ruler > 300)
          {
            end_ruler = 300;
          }

          break;
        }
      }

      if(start_ruler >= end_ruler)
      {
        start_ruler = 0;

        end_ruler = 300;
      }

      startSlider->setValue(start_ruler);
      stopSlider->setValue(end_ruler);

      curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

      curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

      QObject::connect(startSlider, SIGNAL(valueChanged(int)), this, SLOT(startSliderMoved(int)));
      QObject::connect(stopSlider,  SIGNAL(valueChanged(int)), this, SLOT(stopSliderMoved(int)));
    }
  }

  if(job_src == STAT_JOB_SRC_ANNOT)
  {
    free(beat_interval_list);
  }

  Label1->setText(stat_str);

  StatDialog->exec();
}


void UI_StatisticWindow::startSliderMoved(int)
{
  startSlider->blockSignals(true);
  stopSlider->blockSignals(true);

  start_ruler = startSlider->value();
  end_ruler = stopSlider->value();

  if(end_ruler < (start_ruler + 5))
  {
    end_ruler = start_ruler + 5;

    stopSlider->setValue(end_ruler);
  }

  curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

  curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

  startSlider->blockSignals(false);
  stopSlider->blockSignals(false);
}


void UI_StatisticWindow::stopSliderMoved(int)
{
  startSlider->blockSignals(true);
  stopSlider->blockSignals(true);

  start_ruler = startSlider->value();
  end_ruler = stopSlider->value();

  if(start_ruler > (end_ruler - 5))
  {
    start_ruler = end_ruler - 5;

    startSlider->setValue(start_ruler);
  }

  curve1->setH_RulerValues(start_ruler + 1, end_ruler + 1);

  curve1->drawCurve(bpm_distribution + start_ruler, end_ruler - start_ruler, (int)(max_val * 1.1) + 1, 0.0);

  startSlider->blockSignals(false);
  stopSlider->blockSignals(false);
}




