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



#include "ascii_export.h"



UI_AsciiExportwindow::UI_AsciiExportwindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  AsciiExportDialog = new QDialog;

  AsciiExportDialog->setMinimumSize(800, 180);
  AsciiExportDialog->setMaximumSize(800, 180);
  AsciiExportDialog->setWindowTitle("Export to ASCII");
  AsciiExportDialog->setModal(true);
  AsciiExportDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(AsciiExportDialog);
  filelist->setGeometry(10, 10, 780, 75);
  filelist->setSelectionBehavior(QAbstractItemView::SelectRows);
  filelist->setSelectionMode(QAbstractItemView::SingleSelection);
  for(i=0; i<mainwindow->files_open; i++)
  {
    if((mainwindow->edfheaderlist[i]->edf)||(mainwindow->edfheaderlist[i]->bdf))
    {
      new QListWidgetItem(QString::fromLocal8Bit(mainwindow->edfheaderlist[i]->filename), filelist);
    }
  }

  ExportButton = new QPushButton(AsciiExportDialog);
  ExportButton->setGeometry(10, 140, 100, 25);
  ExportButton->setText("Export");

  CloseButton = new QPushButton(AsciiExportDialog);
  CloseButton->setGeometry(690, 140, 100, 25);
  CloseButton->setText("Close");

  QObject::connect(CloseButton,  SIGNAL(clicked()), AsciiExportDialog, SLOT(close()));
  QObject::connect(ExportButton, SIGNAL(clicked()), this,              SLOT(ExportButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  AsciiExportDialog->exec();
}



void UI_AsciiExportwindow::ExportButtonClicked()
{
  int i, j, k, m, p, r, n,
      edfsignals,
      datarecords,
      datarecordswritten,
      recordsize,
      recordfull,
      edfplus,
      bdf,
      bdfplus=0,
      samplesize,
      annot_ch[256],
      nr_annot_chns=0,
      skip,
      max,
      onset,
      duration,
      zero,
      max_tal_ln,
      len,
      progress_steps;

  char path[MAX_PATH_LENGTH],
       ascii_path[MAX_PATH_LENGTH],
       *edf_hdr,
       *scratchpad,
       *cnv_buf,
       *time_in_txt,
       *duration_in_txt,
       str[1024];

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FILE *inputfile,
       *outputfile,
       *annotationfile;

  double data_record_duration,
         elapsedtime,
         time_tmp,
         d_tmp,
         value_tmp;


  if(!mainwindow->files_open)
  {
    AsciiExportDialog->close();
    return;
  }

  ExportButton->setEnabled(false);
  CloseButton->setEnabled(false);

  for(i=0; i<mainwindow->files_open; i++)
  {
    if(!strcmp(mainwindow->edfheaderlist[i]->filename, filelist->item(filelist->currentRow())->text().toLocal8Bit().data()))
    {
      break;
    }
  }

  if(i==mainwindow->files_open)  return;

  n = i;

  inputfile = mainwindow->edfheaderlist[n]->file_hdl;

  path[0] = 0;
  if(mainwindow->recent_savedir[0]!=0)
  {
    strcpy(path, mainwindow->recent_savedir);
    strcat(path, "/");
  }
  len = strlen(path);
  get_filename_from_path(path + len, mainwindow->edfheaderlist[n]->filename, MAX_PATH_LENGTH - len);
  remove_extension_from_filename(path);
  strcat(path, "_data.txt");

  strcpy(path, QFileDialog::getSaveFileName(0, "Export to ASCII", QString::fromLocal8Bit(path), "Text files (*.txt *.TXT)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    AsciiExportDialog->close();
    return;
  }

  get_directory_from_path(mainwindow->recent_savedir, path, MAX_PATH_LENGTH);

  if(mainwindow->file_is_opened(path))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Save file", "Error, selected file is in use.");
    messagewindow.exec();
    AsciiExportDialog->close();
    return;
  }

  if(!(strcmp(path + strlen(path) - 9, "_data.txt")))
  {
    path[strlen(path) - 9] = 0;
  }

  scratchpad = (char *)calloc(1, 128);
  if(scratchpad==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (scratchpad).");
    messagewindow.exec();
    AsciiExportDialog->close();
    return;
  }

  if(fseeko(inputfile, 0xfcLL, SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File read error.");
    messagewindow.exec();
    AsciiExportDialog->close();
    return;
  }

  if(fread(scratchpad, 4, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File read error.");
    messagewindow.exec();
    AsciiExportDialog->close();
    return;
  }

  scratchpad[4] = 0;
  edfsignals = atoi(scratchpad);

  edf_hdr = (char *)calloc(1, (edfsignals + 1) * 256);
  if(edf_hdr==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (edf_hdr)");
    messagewindow.exec();
    AsciiExportDialog->close();
    return;
  }

  rewind(inputfile);
  if(fread(edf_hdr, (edfsignals + 1) * 256, 1, inputfile)!=1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File read error.");
    messagewindow.exec();
    free(edf_hdr);
    AsciiExportDialog->close();
    return;
  }

  for(i=0; i<((edfsignals+1)*256); i++)
  {
    if(edf_hdr[i]==',') edf_hdr[i] = '\'';  /* replace all comma's in header by single quotes because they */
  }                                         /* interfere with the comma-separated txt-files                */

  strncpy(scratchpad, edf_hdr + 0xec, 8);
  scratchpad[8] = 0;
  datarecords = atoi(scratchpad);

  strncpy(scratchpad, edf_hdr + 0xf4, 8);
  scratchpad[8] = 0;
  data_record_duration = atof(scratchpad);

  nr_annot_chns = 0;

  if(((signed char *)edf_hdr)[0]==-1)
  {
    bdf = 1;
    samplesize = 3;
    edfplus = 0;
    if((strncmp(edf_hdr + 0xc0, "BDF+C     ", 10))&&(strncmp(edf_hdr + 0xc0, "BDF+D     ", 10)))
    {
      bdfplus = 0;
    }
    else
    {
      bdfplus = 1;
      for(i=0; i<edfsignals; i++)
      {
        if(!(strncmp(edf_hdr + 256 + i * 16, "BDF Annotations ", 16)))
        {
          annot_ch[nr_annot_chns] = i;
          nr_annot_chns++;
          if(nr_annot_chns>255)  break;
        }
      }
    }
  }
  else
  {
    bdf = 0;
    samplesize = 2;

    if((strncmp(edf_hdr + 0xc0, "EDF+C     ", 10))&&(strncmp(edf_hdr + 0xc0, "EDF+D     ", 10)))
    {
      edfplus = 0;
    }
    else
    {
      edfplus = 1;
      for(i=0; i<edfsignals; i++)
      {
        if(!(strncmp(edf_hdr + 256 + i * 16, "EDF Annotations ", 16)))
        {
          annot_ch[nr_annot_chns] = i;
          nr_annot_chns++;
          if(nr_annot_chns>255)  break;
        }
      }
    }
  }

  edfparamascii = (struct asciiedfparamblock *)calloc(1, sizeof(struct asciiedfparamblock[edfsignals]));
  if(edfparamascii==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (edfparam)");
    messagewindow.exec();
    free(edf_hdr);
    AsciiExportDialog->close();
    return;
  }

  recordsize = 0;

  for(i=0; i<edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + edfsignals * 216 + i * 8, 8);
    scratchpad[8] = 0;
    edfparamascii[i].smp_per_record = atoi(scratchpad);
    edfparamascii[i].buf_offset = recordsize;
    recordsize += edfparamascii[i].smp_per_record;
    strncpy(scratchpad, edf_hdr + 256 + edfsignals * 104 + i * 8, 8);
    scratchpad[8] = 0;
    edfparamascii[i].phys_min = atof(scratchpad);
    strncpy(scratchpad, edf_hdr + 256 + edfsignals * 112 + i * 8, 8);
    scratchpad[8] = 0;
    edfparamascii[i].phys_max = atof(scratchpad);
    strncpy(scratchpad, edf_hdr + 256 + edfsignals * 120 + i * 8, 8);
    scratchpad[8] = 0;
    edfparamascii[i].dig_min = atoi(scratchpad);
    strncpy(scratchpad, edf_hdr + 256 + edfsignals * 128 + i * 8, 8);
    scratchpad[8] = 0;
    edfparamascii[i].dig_max = atoi(scratchpad);
    edfparamascii[i].time_step = data_record_duration / edfparamascii[i].smp_per_record;
    edfparamascii[i].sense = (edfparamascii[i].phys_max - edfparamascii[i].phys_min) / (edfparamascii[i].dig_max - edfparamascii[i].dig_min);
    edfparamascii[i].offset = edfparamascii[i].phys_max / edfparamascii[i].sense - edfparamascii[i].dig_max;
  }

  cnv_buf = (char *)calloc(1, recordsize * samplesize);
  if(cnv_buf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (cnv_buf)");
    messagewindow.exec();
    free(edf_hdr);
    free(edfparamascii);
    AsciiExportDialog->close();
    return;
  }

  free(scratchpad);

  max_tal_ln = 0;

  for(r=0; r<nr_annot_chns; r++)
  {
    if(max_tal_ln<edfparamascii[annot_ch[r]].smp_per_record * samplesize)  max_tal_ln = edfparamascii[annot_ch[r]].smp_per_record * samplesize;
  }

  if(max_tal_ln<128)  max_tal_ln = 128;

  scratchpad = (char *)calloc(1, max_tal_ln + 3);
  if(scratchpad==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (scratchpad)");
    messagewindow.exec();
    free(cnv_buf);
    free(edf_hdr);
    free(edfparamascii);
    AsciiExportDialog->close();
    return;
  }

  duration_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(duration_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (duration_in_txt)");
    messagewindow.exec();
    free(scratchpad);
    free(cnv_buf);
    free(edf_hdr);
    free(edfparamascii);
    AsciiExportDialog->close();
    return;
  }

  time_in_txt = (char *)calloc(1, max_tal_ln + 3);
  if(time_in_txt==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error! (time_in_txt)");
    messagewindow.exec();
    free(duration_in_txt);
    free(scratchpad);
    free(cnv_buf);
    free(edf_hdr);
    free(edfparamascii);
    AsciiExportDialog->close();
    return;
  }

/***************** write header ******************************/

  strcpy(ascii_path, path);
  remove_extension_from_filename(ascii_path);
  strcat(ascii_path, "_header.txt");
  outputfile = fopeno(ascii_path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, can not open headerfile for writing.");
    messagewindow.exec();
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

  fprintf(outputfile, "Version,Subject,Recording,Startdate,Startime,Bytes,Reserved,NumRec,Duration,NumSig\n");

  if(bdf)
  {
    fprintf(outputfile, ".%.7s,", edf_hdr + 1);
  }
  else
  {
    fprintf(outputfile, "%.8s,", edf_hdr);
  }
  fprintf(outputfile, "%.80s,", edf_hdr + 8);
  fprintf(outputfile, "%.80s,", edf_hdr + 88);
  fprintf(outputfile, "%.8s,", edf_hdr + 168);
  fprintf(outputfile, "%.8s,", edf_hdr + 176);
  fprintf(outputfile, "%.8s,", edf_hdr + 184);
  fprintf(outputfile, "%.44s,", edf_hdr + 192);
  fprintf(outputfile, "%i,", datarecords);
  fprintf(outputfile, "%.8s,", edf_hdr + 244);
  snprintf(scratchpad, 128, "%.4s", edf_hdr + 252);
  fprintf(outputfile, "%i\n", atoi(scratchpad) - nr_annot_chns);

  fclose(outputfile);

/***************** write signals ******************************/

  strcpy(ascii_path, path);
  remove_extension_from_filename(ascii_path);
  strcat(ascii_path, "_signals.txt");
  outputfile = fopeno(ascii_path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, can not open signalfile for writing.");
    messagewindow.exec();
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

  fprintf(outputfile, "Signal,Label,Transducer,Units,Min,Max,Dmin,Dmax,PreFilter,Smp/Rec,Reserved\n");

  for(i=0; i<edfsignals; i++)
  {
    if(edfplus || bdfplus)
    {
      skip = 0;

      for(j=0; j<nr_annot_chns; j++)
      {
        if(i==annot_ch[j])  skip = 1;
      }

      if(skip) continue;
    }

    fprintf(outputfile, "%i,", i + 1);
    fprintf(outputfile, "%.16s,", edf_hdr + 256 + i * 16);
    fprintf(outputfile, "%.80s,", edf_hdr + 256 + edfsignals * 16 + i * 80);
    fprintf(outputfile, "%.8s,", edf_hdr + 256 + edfsignals * 96 + i * 8);
    fprintf(outputfile, "%f,", edfparamascii[i].phys_min);
    fprintf(outputfile, "%f,", edfparamascii[i].phys_max);
    fprintf(outputfile, "%i,", edfparamascii[i].dig_min);
    fprintf(outputfile, "%i,", edfparamascii[i].dig_max);
    fprintf(outputfile, "%.80s,", edf_hdr + 256 + edfsignals * 136 + i * 80);
    fprintf(outputfile, "%i,", edfparamascii[i].smp_per_record);
    fprintf(outputfile, "%.32s\n", edf_hdr + 256 + edfsignals * 224 + i * 32);
  }

  fclose(outputfile);

/***************** open annotation file ******************************/

  strcpy(ascii_path, path);
  remove_extension_from_filename(ascii_path);
  strcat(ascii_path, "_annotations.txt");
  annotationfile = fopeno(ascii_path, "wb");
  if(annotationfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, can not open annotationfile for writing.");
    messagewindow.exec();
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

  fprintf(annotationfile, "Onset,Duration,Annotation\n");

/***************** write data ******************************/

  strcpy(ascii_path, path);
  remove_extension_from_filename(ascii_path);
  strcat(ascii_path, "_data.txt");
  outputfile = fopeno(ascii_path, "wb");
  if(outputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Error, can not open datafile for writing.");
    messagewindow.exec();
    fclose(annotationfile);
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

  fprintf(outputfile, "Time");

  for(i=0; i<(edfsignals-nr_annot_chns); i++)
  {
    fprintf(outputfile, ",%i", i + 1);
  }

  if(fputc('\n', outputfile)==EOF)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when writing to outputfile.");
    messagewindow.exec();
    fclose(annotationfile);
    fclose(outputfile);
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

  if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading from inputfile.");
    messagewindow.exec();
    fclose(annotationfile);
    fclose(outputfile);
    free(edf_hdr);
    free(edfparamascii);
    free(cnv_buf);
    free(time_in_txt);
    free(duration_in_txt);
    free(scratchpad);
    AsciiExportDialog->close();
    return;
  }

/***************** start data conversion ******************************/

  QProgressDialog progress("Writing file...", "Abort", 0, datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  datarecordswritten = 0;

  for(i=0; i<datarecords; i++)
  {
    if(!(i%progress_steps))
    {
      progress.setValue(i);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    for(j=0; j<edfsignals; j++)  edfparamascii[j].smp_written = 0;

    if(fread(cnv_buf, recordsize * samplesize, 1, inputfile)!=1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when reading inputfile during export.");
      messagewindow.exec();
      fclose(annotationfile);
      fclose(outputfile);
      free(edf_hdr);
      free(edfparamascii);
      free(cnv_buf);
      free(time_in_txt);
      free(duration_in_txt);
      free(scratchpad);
      AsciiExportDialog->close();
      return;
    }

    if(edfplus || bdfplus)
    {
      max = edfparamascii[annot_ch[0]].smp_per_record * samplesize;
      p = edfparamascii[annot_ch[0]].buf_offset * samplesize;

/* extract time from datarecord */

      for(k=0; k<max; k++)
      {
        if(k>max_tal_ln)
        {
          progress.reset();
          snprintf(str, 256, "Error, TAL in record %i exceeds my buffer.", datarecordswritten + 1);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
          messagewindow.exec();
          fclose(annotationfile);
          fclose(outputfile);
          free(edf_hdr);
          free(edfparamascii);
          free(cnv_buf);
          free(time_in_txt);
          free(duration_in_txt);
          free(scratchpad);
          AsciiExportDialog->close();
          return;
        }
        scratchpad[k] = cnv_buf[p + k];
        if(scratchpad[k]==20) break;
      }
      scratchpad[k] = 0;
      elapsedtime = atof(scratchpad);

/* process annotations */

      for(r=0; r<nr_annot_chns; r++)
      {
        p = edfparamascii[annot_ch[r]].buf_offset * samplesize;
        max = edfparamascii[annot_ch[r]].smp_per_record * samplesize;
        n = 0;
        zero = 0;
        onset = 0;
        duration = 0;
        time_in_txt[0] = 0;
        duration_in_txt[0] = 0;
        scratchpad[0] = 0;

        for(k=0; k<max; k++)
        {
          if(k>max_tal_ln)
          {
            progress.reset();
            snprintf(str, 256, "Error, TAL in record %i exceeds my buffer.", datarecordswritten + 1);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
            messagewindow.exec();
            fclose(annotationfile);
            fclose(outputfile);
            free(edf_hdr);
            free(edfparamascii);
            free(cnv_buf);
            free(time_in_txt);
            free(duration_in_txt);
            free(scratchpad);
            AsciiExportDialog->close();
            return;
          }

          scratchpad[n] = cnv_buf[p + k];

          if(scratchpad[n]==0)
          {
            n = 0;
            onset = 0;
            duration = 0;
            time_in_txt[0] = 0;
            duration_in_txt[0] = 0;
            scratchpad[0] = 0;
            zero++;
            continue;
          }
          else  zero = 0;

          if(zero>1)  break;

          if(scratchpad[n]==20)
          {
            if(duration)
            {
              scratchpad[n] = 0;
              strcpy(duration_in_txt, scratchpad);
              n = 0;
              duration = 0;
              scratchpad[0] = 0;
              continue;
            }
            else if(onset)
                 {
                   scratchpad[n] = 0;
                   if(n)
                   {
                     utf8_to_latin1(scratchpad);
                     for(m=0; m<n; m++)
                     {
                       if(scratchpad[m] == 0)
                       {
                         break;
                       }

                       if((((unsigned char *)scratchpad)[m] < 32) || (((unsigned char *)scratchpad)[m] == ','))
                       {
                         scratchpad[m] = '.';
                       }
                     }
                     fprintf(annotationfile, "%s,%s,%s\n", time_in_txt, duration_in_txt, scratchpad);
                   }
                   n = 0;
                   duration = 0;
                   duration_in_txt[0] = 0;
                   scratchpad[0] = 0;
                   continue;
                 }
                 else
                 {
                   scratchpad[n] = 0;
                   strcpy(time_in_txt, scratchpad);
                   n = 0;
                   onset = 1;
                   duration = 0;
                   duration_in_txt[0] = 0;
                   scratchpad[0] = 0;
                   continue;
                 }
          }

          if(scratchpad[n]==21)
          {
            if(!onset)
            {
              scratchpad[n] = 0;
              strcpy(time_in_txt, scratchpad);
              onset = 1;
            }
            n = 0;
            duration = 1;
            duration_in_txt[0] = 0;
            scratchpad[0] = 0;
            continue;
          }

          if(++n>max_tal_ln)
          {
            progress.reset();
            snprintf(str, 256, "Error, TAL in record %i exceeds my buffer.", datarecordswritten + 1);
            QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
            messagewindow.exec();
            fclose(annotationfile);
            fclose(outputfile);
            free(edf_hdr);
            free(edfparamascii);
            free(cnv_buf);
            free(time_in_txt);
            free(duration_in_txt);
            free(scratchpad);
            AsciiExportDialog->close();
            return;
          }
        }
      }
    }
    else elapsedtime = datarecordswritten * data_record_duration;

/* done with timekeeping and annotations, continue with the data */

    do
    {
      time_tmp = 10000000000.0;
      for(j=0; j<edfsignals; j++)
      {
        if((edfplus)||(bdfplus))
        {
          skip = 0;

          for(p=0; p<nr_annot_chns; p++)
          {
            if(j==annot_ch[p])
            {
              skip = 1;
              break;
            }
          }

          if(skip) continue;
        }

        d_tmp = edfparamascii[j].smp_written * edfparamascii[j].time_step;
        if(d_tmp<time_tmp) time_tmp = d_tmp;
      }
      fprintf(outputfile, "%f", elapsedtime + time_tmp);

      for(j=0; j<edfsignals; j++)
      {
        if((edfplus)||(bdfplus))
        {
          skip = 0;

          for(p=0; p<nr_annot_chns; p++)
          {
            if(j==annot_ch[p])
            {
              skip = 1;
              break;
            }
          }

          if(skip) continue;
        }

        d_tmp = edfparamascii[j].smp_written * edfparamascii[j].time_step;

        if((d_tmp<(time_tmp+0.00000000000001))&&(d_tmp>(time_tmp-0.00000000000001))&&(edfparamascii[j].smp_written<edfparamascii[j].smp_per_record))
        {
          if(bdf)
          {
            var.two[0] = *((unsigned short *)(cnv_buf + ((edfparamascii[j].buf_offset + edfparamascii[j].smp_written) * 3)));
            var.four[2] = *(cnv_buf + ((edfparamascii[j].buf_offset + edfparamascii[j].smp_written) * 3) + 2);

            if(var.four[2]&0x80)
            {
              var.four[3] = 0xff;
            }
            else
            {
              var.four[3] = 0x00;
            }

            value_tmp = ((double)var.one_signed + edfparamascii[j].offset) * edfparamascii[j].sense;
          }
          else
          {
            value_tmp = ((double)(*(((signed short *)cnv_buf) + edfparamascii[j].buf_offset + edfparamascii[j].smp_written)) + edfparamascii[j].offset) * edfparamascii[j].sense;
          }
          fprintf(outputfile, ",%f", value_tmp);
          edfparamascii[j].smp_written++;
        }
        else fputc(',', outputfile);
      }

      if(fputc('\n', outputfile)==EOF)
      {
        progress.reset();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred when writing to outputfile during conversion.");
        messagewindow.exec();
        fclose(annotationfile);
        fclose(outputfile);
        free(edf_hdr);
        free(edfparamascii);
        free(cnv_buf);
        free(time_in_txt);
        free(duration_in_txt);
        free(scratchpad);
        AsciiExportDialog->close();
        return;
      }

      recordfull = 1;
      for(j=0; j<edfsignals; j++)
      {
        if(edfparamascii[j].smp_written<edfparamascii[j].smp_per_record)
        {
          if((edfplus)||(bdfplus))
          {
            skip = 0;

            for(p=0; p<nr_annot_chns; p++)
            {
              if(j==annot_ch[p])
              {
                skip = 1;
                break;
              }
            }

            if(skip) continue;
          }

          recordfull = 0;
          break;
        }
      }
    }
    while(!recordfull);

    datarecordswritten++;
  }

  progress.reset();

  fclose(annotationfile);
  fclose(outputfile);
  free(edf_hdr);
  free(edfparamascii);
  free(cnv_buf);
  free(time_in_txt);
  free(duration_in_txt);
  free(scratchpad);

  AsciiExportDialog->close();
}








