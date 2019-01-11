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



#include "save_montage_dialog.h"


UI_SaveMontagewindow::UI_SaveMontagewindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  if(mainwindow->files_open==1)
  {
    SaveMontageDialog = NULL;
    SaveButtonClicked();
    return;
  }

  SaveMontageDialog = new QDialog;

  SaveMontageDialog->setMinimumSize(800, 180);
  SaveMontageDialog->setMaximumSize(800, 180);
  SaveMontageDialog->setWindowTitle("Save montage");
  SaveMontageDialog->setModal(true);
  SaveMontageDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  label1 = new QLabel(SaveMontageDialog);
  label1->setGeometry(10, 10, 780, 25);
  label1->setText("Choose from which file you want to save the montage:");

  filelist = new QListWidget(SaveMontageDialog);
  filelist->setGeometry(10, 35, 780, 75);
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
  }

  SaveButton = new QPushButton(SaveMontageDialog);
  SaveButton->setGeometry(10, 140, 100, 25);
  SaveButton->setText("Save");

  CloseButton = new QPushButton(SaveMontageDialog);
  CloseButton->setGeometry(690, 140, 100, 25);
  CloseButton->setText("Close");

  QObject::connect(CloseButton,  SIGNAL(clicked()), SaveMontageDialog, SLOT(close()));
  QObject::connect(SaveButton,   SIGNAL(clicked()), this,              SLOT(SaveButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  SaveMontageDialog->exec();
}



void UI_SaveMontagewindow::SaveButtonClicked()
{
  int i, j, n;

  char mtg_path[MAX_PATH_LENGTH];

  FILE *mtgfile;

  if(mainwindow->files_open==1)  n = 0;
  else  n = filelist->currentRow();

  strcpy(mtg_path, mainwindow->recent_montagedir);
  strcat(mtg_path, "/my_montage.mtg");

  strcpy(mtg_path, QFileDialog::getSaveFileName(0, "Save montage", QString::fromLocal8Bit(mtg_path), "Montage files (*.mtg *.MTG)").toLocal8Bit().data());

  if(!strcmp(mtg_path, ""))
  {
    return;
  }

  if(strlen(mtg_path) > 4)
  {
    if(strcmp(mtg_path + strlen(mtg_path) - 4, ".mtg"))
    {
      strcat(mtg_path, ".mtg");
    }
  }

  get_directory_from_path(mainwindow->recent_montagedir, mtg_path, MAX_PATH_LENGTH);

  mtgfile = fopeno(mtg_path, "wb");
  if(mtgfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for writing.");
    messagewindow.exec();

    return;
  }

  fprintf(mtgfile, "<?xml version=\"1.0\"?>\n<" PROGRAM_NAME "_montage>\n");

  for(i=0; i<mainwindow->signalcomps; i++)
  {
    if(!strcmp(mainwindow->signalcomp[i]->edfhdr->filename, mainwindow->edfheaderlist[n]->filename))
    {
      fprintf(mtgfile, "  <signalcomposition>\n");

      fprintf(mtgfile, "    <alias>");

      xml_fwrite_encode_entity(mtgfile, mainwindow->signalcomp[i]->alias);

      fprintf(mtgfile, "</alias>\n");

      fprintf(mtgfile, "    <num_of_signals>%i</num_of_signals>\n", mainwindow->signalcomp[i]->num_of_signals);

      fprintf(mtgfile, "    <voltpercm>%f</voltpercm>\n", mainwindow->signalcomp[i]->voltpercm);

      fprintf(mtgfile, "    <screen_offset>%f</screen_offset>\n", mainwindow->signalcomp[i]->screen_offset);

      fprintf(mtgfile, "    <polarity>%i</polarity>\n", mainwindow->signalcomp[i]->polarity);

      fprintf(mtgfile, "    <color>%i</color>\n", mainwindow->signalcomp[i]->color);

      if(mainwindow->signalcomp[i]->spike_filter)
      {
        fprintf(mtgfile, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 1);
      }
      else
      {
        fprintf(mtgfile, "    <spike_filter_cnt>%i</spike_filter_cnt>\n", 0);
      }

      fprintf(mtgfile, "    <filter_cnt>%i</filter_cnt>\n", mainwindow->signalcomp[i]->filter_cnt);

      fprintf(mtgfile, "    <fidfilter_cnt>%i</fidfilter_cnt>\n", mainwindow->signalcomp[i]->fidfilter_cnt);

      fprintf(mtgfile, "    <ravg_filter_cnt>%i</ravg_filter_cnt>\n", mainwindow->signalcomp[i]->ravg_filter_cnt);

      for(j=0; j<mainwindow->signalcomp[i]->num_of_signals; j++)
      {
        fprintf(mtgfile, "    <signal>\n");

        fprintf(mtgfile, "      <label>");

        xml_fwrite_encode_entity(mtgfile, mainwindow->signalcomp[i]->edfhdr->edfparam[mainwindow->signalcomp[i]->edfsignal[j]].label);

        fprintf(mtgfile, "</label>\n");

        fprintf(mtgfile, "      <factor>%i</factor>\n", mainwindow->signalcomp[i]->factor[j]);

        fprintf(mtgfile, "    </signal>\n");
      }

      if(mainwindow->signalcomp[i]->spike_filter)
      {
        fprintf(mtgfile, "    <spike_filter>\n");

        fprintf(mtgfile, "      <velocity>%.16f</velocity>\n", mainwindow->signalcomp[i]->spike_filter_velocity);

        fprintf(mtgfile, "      <holdoff>%i</holdoff>\n", mainwindow->signalcomp[i]->spike_filter_holdoff);

        fprintf(mtgfile, "    </spike_filter>\n");
      }

      for(j=0; j<mainwindow->signalcomp[i]->filter_cnt; j++)
      {
        fprintf(mtgfile, "    <filter>\n");

        fprintf(mtgfile, "      <LPF>%i</LPF>\n", mainwindow->signalcomp[i]->filter[j]->is_LPF);

        fprintf(mtgfile, "      <frequency>%.16f</frequency>\n", mainwindow->signalcomp[i]->filter[j]->cutoff_frequency);

        fprintf(mtgfile, "    </filter>\n");
      }

      for(j=0; j<mainwindow->signalcomp[i]->ravg_filter_cnt; j++)
      {
        fprintf(mtgfile, "    <ravg_filter>\n");

        fprintf(mtgfile, "      <type>%i</type>\n", mainwindow->signalcomp[i]->ravg_filter[j]->type);

        fprintf(mtgfile, "      <size>%i</size>\n", mainwindow->signalcomp[i]->ravg_filter[j]->size);

        fprintf(mtgfile, "    </ravg_filter>\n");
      }

      for(j=0; j<mainwindow->signalcomp[i]->fidfilter_cnt; j++)
      {
        fprintf(mtgfile, "    <fidfilter>\n");

        fprintf(mtgfile, "      <type>%i</type>\n", mainwindow->signalcomp[i]->fidfilter_type[j]);

        fprintf(mtgfile, "      <frequency>%.16f</frequency>\n", mainwindow->signalcomp[i]->fidfilter_freq[j]);

        fprintf(mtgfile, "      <frequency2>%.16f</frequency2>\n", mainwindow->signalcomp[i]->fidfilter_freq2[j]);

        fprintf(mtgfile, "      <ripple>%.16f</ripple>\n", mainwindow->signalcomp[i]->fidfilter_ripple[j]);

        fprintf(mtgfile, "      <order>%i</order>\n", mainwindow->signalcomp[i]->fidfilter_order[j]);

        fprintf(mtgfile, "      <model>%i</model>\n", mainwindow->signalcomp[i]->fidfilter_model[j]);

        fprintf(mtgfile, "    </fidfilter>\n");
      }

      if(mainwindow->signalcomp[i]->plif_ecg_filter != NULL)
      {
        fprintf(mtgfile, "    <plif_ecg_filter>\n");

        fprintf(mtgfile, "      <plf>%i</plf>\n", mainwindow->signalcomp[i]->plif_ecg_subtract_filter_plf);

        fprintf(mtgfile, "    </plif_ecg_filter>\n");
      }

      if(mainwindow->signalcomp[i]->ecg_filter != NULL)
      {
        fprintf(mtgfile, "    <ecg_filter>\n");

        fprintf(mtgfile, "      <type>1</type>\n");

        fprintf(mtgfile, "    </ecg_filter>\n");
      }

      if(mainwindow->signalcomp[i]->zratio_filter != NULL)
      {
        fprintf(mtgfile, "    <zratio_filter>\n");

        fprintf(mtgfile, "      <type>1</type>\n");

        fprintf(mtgfile, "      <crossoverfreq>%f</crossoverfreq>\n", mainwindow->signalcomp[i]->zratio_crossoverfreq);

        fprintf(mtgfile, "    </zratio_filter>\n");
      }

      if(mainwindow->signalcomp[i]->hasruler)
      {
        fprintf(mtgfile, "    <floating_ruler>\n");

        fprintf(mtgfile, "      <hasruler>1</hasruler>\n");

        fprintf(mtgfile, "      <ruler_x_position>%i</ruler_x_position>\n", mainwindow->maincurve->ruler_x_position);

        fprintf(mtgfile, "      <ruler_y_position>%i</ruler_y_position>\n", mainwindow->maincurve->ruler_y_position);

        fprintf(mtgfile, "      <floating_ruler_value>%i</floating_ruler_value>\n", mainwindow->maincurve->floating_ruler_value);

        fprintf(mtgfile, "    </floating_ruler>\n");
      }

      fprintf(mtgfile, "  </signalcomposition>\n");
    }
  }

#ifdef Q_OS_WIN32
  __mingw_fprintf(mtgfile, "  <pagetime>%lli</pagetime>\n", mainwindow->pagetime);
#else
  fprintf(mtgfile, "  <pagetime>%lli</pagetime>\n", mainwindow->pagetime);
#endif

  struct spectrumdocksettings settings;

  for(i=0; i<MAXSPECTRUMDOCKS; i++)
  {
    if(mainwindow->spectrumdock[i]->dock->isVisible())
    {
      mainwindow->spectrumdock[i]->getsettings(&settings);

      if(settings.signalnr >= 0)
      {
        fprintf(mtgfile, "  <powerspectrumdock>\n");

        fprintf(mtgfile, "    <signalnum>%i</signalnum>\n", settings.signalnr);

        fprintf(mtgfile, "    <amp>%i</amp>\n", settings.amp);

        fprintf(mtgfile, "    <log_min_sl>%i</log_min_sl>\n", settings.log_min_sl);

        fprintf(mtgfile, "    <wheel>%i</wheel>\n", settings.wheel);

        fprintf(mtgfile, "    <span>%i</span>\n", settings.span);

        fprintf(mtgfile, "    <center>%i</center>\n", settings.center);

        fprintf(mtgfile, "    <log>%i</log>\n", settings.log);

        fprintf(mtgfile, "    <sqrt>%i</sqrt>\n", settings.sqrt);

        fprintf(mtgfile, "    <colorbar>%i</colorbar>\n", settings.colorbar);

        fprintf(mtgfile, "    <maxvalue>%.10f</maxvalue>\n", settings.maxvalue);

        fprintf(mtgfile, "    <maxvalue_sqrt>%.10f</maxvalue_sqrt>\n", settings.maxvalue_sqrt);

        fprintf(mtgfile, "    <maxvalue_vlog>%.10f</maxvalue_vlog>\n", settings.maxvalue_vlog);

        fprintf(mtgfile, "    <maxvalue_sqrt_vlog>%.10f</maxvalue_sqrt_vlog>\n", settings.maxvalue_sqrt_vlog);

        fprintf(mtgfile, "    <minvalue_vlog>%.10f</minvalue_vlog>\n", settings.minvalue_vlog);

        fprintf(mtgfile, "    <minvalue_sqrt_vlog>%.10f</minvalue_sqrt_vlog>\n", settings.minvalue_sqrt_vlog);

        fprintf(mtgfile, "  </powerspectrumdock>\n");
      }
    }
  }

  fprintf(mtgfile, "</" PROGRAM_NAME "_montage>\n");

  fclose(mtgfile);

  if(mainwindow->files_open == 1)
  {
    strcpy(&mainwindow->recent_file_mtg_path[0][0], mtg_path);
  }

  if(SaveMontageDialog!=NULL) SaveMontageDialog->close();
}







