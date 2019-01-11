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




#include "averager_dialog.h"



UI_AveragerWindow::UI_AveragerWindow(QWidget *w_parent, int annot_nr, int file_n)
{
  int i;

  long long recording_duration;

  struct annotationblock *annot_ptr;

  mainwindow = (UI_Mainwindow *)w_parent;

  file_num = file_n;

  averager_dialog = new QDialog(w_parent);

  averager_dialog->setMinimumSize(600, 400);
  averager_dialog->setMaximumSize(600, 400);
  averager_dialog->setWindowTitle("Average waveforms");
  averager_dialog->setModal(true);
  averager_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  annotLabel = new QLabel(averager_dialog);
  annotLabel->setGeometry(20, 20, 100, 25);
  annotLabel->setText("Use annotation:");

  annotNameLabel = new QLabel(averager_dialog);
  annotNameLabel->setGeometry(130, 20, 200, 25);

  signalLabel = new QLabel(averager_dialog);
  signalLabel->setGeometry(340, 20, 100, 25);
  signalLabel->setText("Select signal(s):");

  list = new QListWidget(averager_dialog);
  list->setGeometry(450, 20, 130, 360);
  list->setSelectionBehavior(QAbstractItemView::SelectRows);
  list->setSelectionMode(QAbstractItemView::ExtendedSelection);

  time1.setHMS(0, 0, 0, 0);

  recording_duration = (mainwindow->edfheaderlist[file_num]->datarecords * mainwindow->edfheaderlist[file_num]->long_data_record_duration) / TIME_DIMENSION;

  time2.setHMS((recording_duration / 3600) % 24, (recording_duration % 3600) / 60, recording_duration % 60, 0);

  time1Label = new QLabel(averager_dialog);
  time1Label->setGeometry(20, 65, 100, 25);
  time1Label->setText("From (hh:mm:ss)");

  timeEdit1 = new QTimeEdit(averager_dialog);
  timeEdit1->setGeometry(130, 65, 110, 25);
  timeEdit1->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit1->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit1->setMaximumTime(time2);

  time2Label = new QLabel(averager_dialog);
  time2Label->setGeometry(20, 110, 100, 25);
  time2Label->setText("To (hh:mm:ss)");

  timeEdit2 = new QTimeEdit(averager_dialog);
  timeEdit2->setGeometry(130, 110, 110, 25);
  timeEdit2->setDisplayFormat("hh:mm:ss.zzz");
  timeEdit2->setMinimumTime(QTime(0, 0, 0, 0));
  timeEdit2->setMaximumTime(time2);
  timeEdit2->setTime(time2);

  ratioLabel = new QLabel(averager_dialog);
  ratioLabel->setGeometry(20, 155, 100, 75);
  ratioLabel->setText("Ratio of time\n"
                      "before and after\n"
                      "trigger:");

  ratioBox = new QComboBox(averager_dialog);
  ratioBox->setGeometry(130, 180, 100, 25);
  ratioBox->addItem("10 / 90");
  ratioBox->addItem("25 / 75");
  ratioBox->addItem("50 / 50");
  ratioBox->setCurrentIndex(mainwindow->average_ratio);

  bufsizeLabel = new QLabel(averager_dialog);
  bufsizeLabel->setGeometry(20, 250, 100, 25);
  bufsizeLabel->setText("Average period:");

  avg_periodspinbox = new QSpinBox(averager_dialog);
  avg_periodspinbox->setGeometry(130, 250, 100, 25);
  avg_periodspinbox->setRange(10, 300000);
  avg_periodspinbox->setSuffix(" mSec");
  avg_periodspinbox->setValue(mainwindow->average_period);

  CloseButton = new QPushButton(averager_dialog);
  CloseButton->setGeometry(20, 355, 100, 25);
  CloseButton->setText("Cancel");

  StartButton = new QPushButton(averager_dialog);
  StartButton->setGeometry(310, 355, 100, 25);
  StartButton->setText("Start");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(mainwindow->signalcomp[i]->alias[0] != 0)
    {
      new QListWidgetItem(mainwindow->signalcomp[i]->alias, list);
    }
    else
    {
      new QListWidgetItem(mainwindow->signalcomp[i]->signallabel, list);
    }
  }

  list->setCurrentRow(0, QItemSelectionModel::Select);

  annot_ptr = edfplus_annotation_get_item_visible_only(&mainwindow->edfheaderlist[file_num]->annot_list, annot_nr);

  strcpy(annot_str, annot_ptr->annotation);
  remove_leading_spaces(annot_str);
  remove_trailing_spaces(annot_str);

  annotNameLabel->setText(annot_str);

  QObject::connect(CloseButton, SIGNAL(clicked()), averager_dialog, SLOT(close()));
  QObject::connect(StartButton, SIGNAL(clicked()), this,            SLOT(startButtonClicked()));

  averager_dialog->exec();
}



void UI_AveragerWindow::startButtonClicked()
{
  int i, j, n,
      avg_cnt=0,
      samples_on_screen,
      trigger_position_ratio=4,
      dialoglistnumber,
      itemCnt,
      progress_steps;

  char str[MAX_ANNOTATION_LEN + 1],
       scratchpad[1024];

  double avg_max_value,
         avg_min_value;

  long long backup_viewtime,
            backup_timescale,
            l_time1,
            l_time2;

  struct annotationblock *annot_ptr;

  QList<QListWidgetItem *> itemList;


  mainwindow->average_period = avg_periodspinbox->value();
  mainwindow->average_ratio = ratioBox->currentIndex();

  itemList = list->selectedItems();

  itemCnt = itemList.size();

  if(itemCnt < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You have to select at least one signal.");
    messagewindow.exec();
    return;
  }

  if(itemCnt > MAXAVERAGECURVEDIALOGS)
  {
    sprintf(scratchpad,"You can not select more than %i signals.", MAXAVERAGECURVEDIALOGS);
    QMessageBox messagewindow(QMessageBox::Critical, "Error", scratchpad);
    messagewindow.exec();
    return;
  }

  time1 = timeEdit1->time();
  time2 = timeEdit2->time();

  if(time2.operator<=(time1) == true)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Starttime is higher or equal to stoptime.");
    messagewindow.exec();
    return;
  }

  l_time1 = (((time1.hour() * 3600) + (time1.minute() * 60) + (time1.second())) * TIME_DIMENSION) + (time1.msec() * TIME_DIMENSION / 1000LL);
  l_time2 = (((time2.hour() * 3600) + (time2.minute() * 60) + (time2.second())) * TIME_DIMENSION) + (time2.msec() * TIME_DIMENSION / 1000LL);

  n = ratioBox->currentIndex();

  switch(n)
  {
    case 0 : trigger_position_ratio = 10;
            break;
    case 1 : trigger_position_ratio = 4;
            break;
    case 2 : trigger_position_ratio = 2;
            break;
  }

  backup_viewtime = mainwindow->edfheaderlist[file_num]->viewtime;

  backup_timescale = mainwindow->pagetime;

  mainwindow->pagetime = avg_periodspinbox->value() * (TIME_DIMENSION / 1000LL);

  mainwindow->setup_viewbuf();

  mainwindow->signal_averaging_active = 1;

  n = edfplus_annotation_size(&mainwindow->edfheaderlist[file_num]->annot_list);

  avg_cnt = 0;

  for(i=0; i<n; i++)
  {
    annot_ptr = edfplus_annotation_get_item(&mainwindow->edfheaderlist[file_num]->annot_list, i);

    if(((annot_ptr->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) >= l_time1)
      && ((annot_ptr->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) <= l_time2))
    {
      strcpy(str, annot_ptr->annotation);

      remove_leading_spaces(str);
      remove_trailing_spaces(str);

      if(!strcmp(str, annot_str))
      {
        avg_cnt++;
      }
    }
  }

  QProgressDialog progress("Averaging ...", "Abort", 0, avg_cnt, averager_dialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);
  progress.reset();

  progress_steps = avg_cnt / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  for(j=0; j<itemCnt; j++)
  {
    for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
    {
      if(mainwindow->averagecurvedialog[i] == NULL)
      {
        dialoglistnumber = i;

        break;
      }
    }

    if(i >= MAXAVERAGECURVEDIALOGS)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many \"Average\" windows are open.\nClose some first.");
      messagewindow.exec();

      mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    signal_nr = list->row(itemList.at(j));

    if(mainwindow->signalcomp[signal_nr]->samples_on_screen > 2147483646LL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Too many samples in buf.");
      messagewindow.exec();

      mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    samples_on_screen = mainwindow->signalcomp[signal_nr]->samples_on_screen;

    avgbuf = (double *)calloc(1, sizeof(double) * samples_on_screen);
    if(avgbuf == NULL)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
      messagewindow.exec();

      mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;
      mainwindow->pagetime = backup_timescale;
      mainwindow->signal_averaging_active = 0;
      mainwindow->setup_viewbuf();

      return;
    }

    n = edfplus_annotation_size(&mainwindow->edfheaderlist[file_num]->annot_list);

    avg_cnt = 0;

    for(i=0; i<n; i++)
    {
      annot_ptr = edfplus_annotation_get_item(&mainwindow->edfheaderlist[file_num]->annot_list, i);

      if(annot_ptr->hided_in_list)
      {
        continue;
      }

      if(((annot_ptr->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) >= l_time1)
        && ((annot_ptr->onset - mainwindow->edfheaderlist[file_num]->starttime_offset) <= l_time2))
      {
        strcpy(str, annot_ptr->annotation);

        remove_leading_spaces(str);
        remove_trailing_spaces(str);

        if(!strcmp(str, annot_str))
        {
          if(!(avg_cnt % progress_steps))
          {
            progress.setValue(avg_cnt);

            qApp->processEvents();

            if(progress.wasCanceled() == true)
            {
              free(avgbuf);

              mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;

              mainwindow->pagetime = backup_timescale;

              mainwindow->signal_averaging_active = 0;

              mainwindow->setup_viewbuf();

              return;
            }
          }

          mainwindow->edfheaderlist[file_num]->viewtime = annot_ptr->onset;

          mainwindow->edfheaderlist[file_num]->viewtime -= mainwindow->edfheaderlist[file_num]->starttime_offset;

          mainwindow->edfheaderlist[file_num]->viewtime -= (mainwindow->pagetime / trigger_position_ratio);

          mainwindow->setup_viewbuf();

          process_avg(mainwindow->signalcomp[signal_nr]);

          avg_cnt++;
        }
      }
    }

    progress.reset();

    if(!avg_cnt)
    {
      sprintf(scratchpad, "The selected annotation/trigger \"%s\" is not in the selected timewindow\n"
                          "%i:%02i:%02i - %i:%02i:%02i",
                          annot_str,
                          timeEdit1->time().hour(), timeEdit1->time().minute(), timeEdit1->time().second(),
                          timeEdit2->time().hour(), timeEdit2->time().minute(), timeEdit2->time().second());

      QMessageBox messagewindow(QMessageBox::Critical, "Error", scratchpad);
      messagewindow.exec();

      free(avgbuf);

      mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;

      mainwindow->pagetime = backup_timescale;

      mainwindow->signal_averaging_active = 0;

      mainwindow->setup_viewbuf();

      return;
    }

    avg_max_value = -100000000.0;
    avg_min_value =  100000000.0;

    for(i=0; i<samples_on_screen; i++)
    {
      avgbuf[i] /= avg_cnt;

      if(avgbuf[i] > avg_max_value)
      {
        avg_max_value = avgbuf[i];
      }

      if(avgbuf[i] < avg_min_value)
      {
        avg_min_value = avgbuf[i];
      }
    }

    mainwindow->averagecurvedialog[dialoglistnumber] = new UI_AverageCurveWindow(mainwindow->signalcomp[signal_nr], mainwindow, dialoglistnumber,
                                                                                 avgbuf,
                                                                                 avg_max_value,
                                                                                 avg_min_value,
                                                                                 mainwindow->pagetime,
                                                                                 mainwindow->signalcomp[signal_nr]->samples_on_screen,
                                                                                 avg_cnt,
                                                                                 trigger_position_ratio,
                                                                                 annot_str,
                                                                                 avg_periodspinbox->value());

    for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
    {
      if(mainwindow->signalcomp[signal_nr]->avg_dialog[i] == 0)
      {
        mainwindow->signalcomp[signal_nr]->avg_dialog[i] = dialoglistnumber + 1;

        break;
      }
    }
  }

  mainwindow->edfheaderlist[file_num]->viewtime = backup_viewtime;

  mainwindow->pagetime = backup_timescale;

  mainwindow->signal_averaging_active = 0;

  mainwindow->setup_viewbuf();

  averager_dialog->close();
}


UI_AveragerWindow::~UI_AveragerWindow()
{
  int i;

  for(i=0; i<MAXAVERAGECURVEDIALOGS; i++)
  {
    if(mainwindow->averagecurvedialog[i] != NULL)
    {
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->move(((i % 15) * 30) + 200, ((i % 15) * 30) + 200);
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->raise();
      mainwindow->averagecurvedialog[i]->averager_curve_dialog->activateWindow();
    }
  }
}


void UI_AveragerWindow::process_avg(struct signalcompblock *signalcomp)
{
  int j, k;

  char *viewbuf;

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

  viewbuf = mainwindow->viewbuf;

  if(viewbuf == NULL)
  {
    return;
  }

  if(avgbuf == NULL)
  {
    return;
  }

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

    avgbuf[s] += (dig_value * signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue);
  }
}






















