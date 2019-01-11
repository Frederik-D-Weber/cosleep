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



#include "edf_compat.h"



UI_EDFCompatwindow::UI_EDFCompatwindow(QWidget *w_parent)
{
  int i;

  mainwindow = (UI_Mainwindow *)w_parent;

  EDFCompatDialog = new QDialog;

  EDFCompatDialog->setMinimumSize(800, 180);
  EDFCompatDialog->setMaximumSize(800, 180);
  EDFCompatDialog->setWindowTitle("Check EDF(+) / BDF(+) compatibility");
  EDFCompatDialog->setModal(true);
  EDFCompatDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  filelist = new QListWidget(EDFCompatDialog);
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

  CheckButton = new QPushButton(EDFCompatDialog);
  CheckButton->setGeometry(10, 140, 100, 25);
  CheckButton->setText("Check");

  CloseButton = new QPushButton(EDFCompatDialog);
  CloseButton->setGeometry(690, 140, 100, 25);
  CloseButton->setText("Close");

  QObject::connect(CloseButton,  SIGNAL(clicked()), EDFCompatDialog, SLOT(close()));
  QObject::connect(CheckButton,  SIGNAL(clicked()), this,            SLOT(CheckButtonClicked()));

  filelist->setCurrentRow(mainwindow->files_open - 1);

  EDFCompatDialog->exec();
}





void UI_EDFCompatwindow::CheckButtonClicked()
{
  int i, j, k, p, r=0, n,
      len,
      edfsignals,
      datarecords,
      recordsize,
      edfplus,
      discontinuous,
      bdf,
      bdfplus,
      *annot_ch,
      nr_annot_chns,
      max,
      onset,
      duration,
      duration_start,
      zero,
      max_tal_ln,
      error,
      temp,
      samplesize,
      annots_in_tal,
      progress_steps;

  char *scratchpad,
       *cnv_buf,
       txt_string[2048];

  double data_record_duration,
         elapsedtime,
         time_tmp=0.0;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FILE *inputfile;

  struct edfparamblock *edfparam;



  if(!mainwindow->files_open)
  {
    EDFCompatDialog->close();
    return;
  }

  CheckButton->setEnabled(false);
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

  edfsignals = mainwindow->edfheaderlist[n]->edfsignals;
  recordsize = mainwindow->edfheaderlist[n]->recordsize;
  edfparam = mainwindow->edfheaderlist[n]->edfparam;
  nr_annot_chns = mainwindow->edfheaderlist[n]->nr_annot_chns;
  datarecords = mainwindow->edfheaderlist[n]->datarecords;
  data_record_duration = mainwindow->edfheaderlist[n]->data_record_duration;
  edfplus = mainwindow->edfheaderlist[n]->edfplus;
  bdfplus = mainwindow->edfheaderlist[n]->bdfplus;
  discontinuous = mainwindow->edfheaderlist[n]->discontinuous;
  annot_ch = mainwindow->edfheaderlist[n]->annot_ch;
  bdf = mainwindow->edfheaderlist[n]->bdf;

  if(bdf)  samplesize = 3;
  else  samplesize = 2;

  cnv_buf = (char *)calloc(1, recordsize);
  if(cnv_buf==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Malloc error. (cnv_buf).");
    messagewindow.exec();
    EDFCompatDialog->close();
    return;
  }

  max_tal_ln = 0;

  for(i=0; i<nr_annot_chns; i++)
  {
    if(max_tal_ln<edfparam[annot_ch[i]].smp_per_record * samplesize)  max_tal_ln = edfparam[annot_ch[i]].smp_per_record * samplesize;
  }

  if(max_tal_ln<128)  max_tal_ln = 128;

  scratchpad = (char *)calloc(1, max_tal_ln + 3);
  if(scratchpad==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "malloc error. (scratchpad).");
    messagewindow.exec();
    free(cnv_buf);
    EDFCompatDialog->close();
    return;
  }

  if(fseeko(inputfile, (long long)((edfsignals + 1) * 256), SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile.");
    messagewindow.exec();
    free(cnv_buf);
    free(scratchpad);
    EDFCompatDialog->close();
    return;
  }

/***************** check the maximum and minimum of samples of all signals ******************************/

  QProgressDialog progress("Checking...", "Abort", 0, datarecords, EDFCompatDialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = datarecords / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  elapsedtime = 0.0;

  for(i=0; i<datarecords; i++)
  {
    if(!(i%progress_steps))
    {
      progress.setValue(i);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        free(cnv_buf);
        free(scratchpad);
        EDFCompatDialog->close();
        return;
      }
    }

    if(fread(cnv_buf, recordsize, 1, inputfile)!=1)
    {
      progress.reset();
      snprintf(txt_string, 2048, "An error occurred while reading inputfile at datarecord %i.", i + 1);
      QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
      messagewindow.exec();
      free(cnv_buf);
      free(scratchpad);
      EDFCompatDialog->close();
      return;
    }

    for(j=0; j<edfsignals; j++)
    {
      for(k=0; k<edfparam[j].smp_per_record; k++)
      {
        if(bdf)
        {
          var.two[0] = *((unsigned short *)(cnv_buf + edfparam[j].buf_offset + (k * 3)));
          var.four[2] = *(cnv_buf + edfparam[j].buf_offset + (k * 3) + 2);

          if(var.four[2]&0x80)
          {
            var.four[3] = 0xff;
          }
          else
          {
            var.four[3] = 0x00;
          }

          temp = var.one_signed;
        }
        else
        {
          temp = *((signed short *)(cnv_buf + edfparam[j].buf_offset + (k * 2)));
        }

        if(temp > edfparam[j].dig_max)
        {
          progress.reset();
          snprintf(txt_string, 2048, "Error.\n\nIn datarecord %i -> signal %i -> sample %i is more than digital maximum.\n"
                 "Digital maximum for this signal as written in header is %i but sample is %i.\n"
                 "Offset from start of file: 0x%X\n",
                 i + 1,
                 j + 1,
                 k + 1,
                 edfparam[j].dig_max,
                 temp,
                 (i * recordsize) + edfparam[j].buf_offset + (k * samplesize) + (edfsignals * 256) + 256);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
          messagewindow.exec();
          free(scratchpad);
          free(cnv_buf);
          EDFCompatDialog->close();
          return;
        }

        if(temp < edfparam[j].dig_min)
        {
          progress.reset();
          snprintf(txt_string, 2048, "Error.\n\nIn datarecord %i -> signal %i -> sample %i is less than digital minimum.\n"
                 "Digital minimum for this signal as written in header is %i but sample is %i.\n"
                 "Offset from start of file: 0x%X\n",
                 i + 1,
                 j + 1,
                 k + 1,
                 edfparam[j].dig_min,
                 temp,
                 (i * recordsize) + edfparam[j].buf_offset + (k * samplesize) + (edfsignals * 256) + 256);
          QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
          messagewindow.exec();
          free(cnv_buf);
          free(scratchpad);
          EDFCompatDialog->close();
          return;
        }
      }
    }

/************** process annotationsignals (if any) **************/

    error = 0;

    for(r=0; r<nr_annot_chns; r++)
    {
      n = 0;
      zero = 0;
      onset = 0;
      duration = 0;
      duration_start = 0;
      scratchpad[0] = 0;
      annots_in_tal = 0;

      p = edfparam[annot_ch[r]].buf_offset;
      max = edfparam[annot_ch[r]].smp_per_record * samplesize;

/************** process one annotation signal ****************/

      if(cnv_buf[p + max - 1]!=0)
      {
        error = 5;
        goto END;
      }

      if(!r)  /* if it's the first annotation signal, then check */
      {       /* the timekeeping annotation */
        error = 1;

        for(k=0; k<(max-2); k++)
        {
          scratchpad[k] = cnv_buf[p + k];

          if(scratchpad[k]==20)
          {
            if(cnv_buf[p + k + 1]!=20)
            {
              error = 6;
              goto END;
            }
            scratchpad[k] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            else
            {
              time_tmp = atof(scratchpad);
              if(i)
              {
                if(discontinuous)
                {
                  if(time_tmp<elapsedtime)
                  {
                    error = 4;
                    goto END;
                  }
                }
                else
                {
                  if(((time_tmp-elapsedtime)>(data_record_duration+0.00000001))||((time_tmp-elapsedtime)<(data_record_duration-0.00000001)))
                  {
                    error = 3;
                    goto END;
                  }
                }
              }
              else
              {
                if(time_tmp>=1.0)
                {
                  error = 2;
                  goto END;
                }
              }
              elapsedtime = time_tmp;
              error = 0;
              break;
            }
          }
        }
      }

      for(k=0; k<max; k++)
      {
        scratchpad[n] = cnv_buf[p + k];

        if(!scratchpad[n])
        {
          if(!zero)
          {
            if(k)
            {
              if(cnv_buf[p + k - 1]!=20)
              {
                error = 33;
                goto END;
              }
            }
            else
            {
              zero++;
            }
            n = 0;
            onset = 0;
            duration = 0;
            duration_start = 0;
            scratchpad[0] = 0;
            annots_in_tal = 0;
          }
          zero++;
          continue;
        }
        if(zero>1)
        {
          error = 34;
          goto END;
        }
        zero = 0;

        if((scratchpad[n]==20)||(scratchpad[n]==21))
        {
          if(scratchpad[n]==21)
          {
            if(duration||duration_start||onset||annots_in_tal)
            {               /* it's not allowed to have multiple duration fields */
              error = 35;   /* in one TAL or to have a duration field which is   */
              goto END;     /* not immediately behind the onsetfield             */
            }
            duration_start = 1;
          }

          if((scratchpad[n]==20)&&onset&&(!duration_start))
          {
           annots_in_tal++;
           n = 0;
           continue;
          }

          if(!onset)
          {
            scratchpad[n] = 0;
            if(is_onset_number(scratchpad))
            {
              error = 36;
              goto END;
            }
            onset = 1;
            n = 0;
            continue;
          }

          if(duration_start)
          {
            scratchpad[n] = 0;
            if(is_duration_number(scratchpad))
            {
              error = 37;
              goto END;
            }
            duration = 1;
            duration_start = 0;
            n = 0;
            continue;
          }
        }

        n++;
      }

 END:

/****************** end ************************/

      if(error)
      {
        progress.reset();

        if(error==1)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i.\n",
                 i + 1);
        }
        if(error==2)
        {
          snprintf(txt_string, 1700, "Error.\n\n"
                 "Time keeping annotation in the first datarecord is %.12f\n"
                 "expected <1.0\n",
                 time_tmp);
        }
        if(error==3)
        {
          snprintf(txt_string, 1700, "Error.\n\n"
                 "Datarecord duration is %.12f but timestep between\n"
                 "datarecord %i and preceding datarecord is %.12f.\n",
                 data_record_duration,
                 i + 1,
                 time_tmp - elapsedtime);
        }
        if(error==4)
        {
          snprintf(txt_string, 1700, "Error.\n\nDatarecords are not in chronological order,\n"
                 "datarecord %i has timestamp %.12f and datarecord %i has timestamp %.12f.\n",
                 i,
                 elapsedtime,
                 i + 1,
                 time_tmp);
        }
        if(error==5)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "last byte is not a null-byte.\n",
                 i + 1);
        }
        if(error==6)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "timekeeping TAL (the first annotation in the first annotations signal)\n"
                 "should have two consecutive bytes with values 0x14 immediately after\n"
                 "the number.\n",
                 i + 1);
        }
        if(error==33)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "byte before the first null-byte should be equal to 0x14.\n",
                 i + 1);
        }
        if(error==34)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "after two consecutive null-bytes all of the remaining bytes should be null-bytes as well.\n"
                 "(it's not allowed to have two or more null-bytes between two TAL's)\n",
                 i + 1);
        }
        if(error==35)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "it's not allowed to have multiple duration fields in one TAL or\n"
                 "to have a duration field which is not immediately behind the onsetfield.\n",
                 i + 1);
        }
        if(error==36)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "onset has an invalid number.\n",
                 i + 1);
        }
        if(error==37)
        {
          snprintf(txt_string, 1700, "Error.\n\nInvalid annotation in datarecord %i,\n"
                 "duration has an invalid number.\n",
                 i + 1);
        }
        len = strlen(txt_string);
        p = (i * recordsize) + (edfsignals * 256) + 256 + (edfparam[annot_ch[r]].buf_offset);
        snprintf(txt_string + len, 348, "Offset from start of file: 0x%X\n\n"
                "Stopped at the first error, additional errors may be present.",
               p);
        QMessageBox messagewindow(QMessageBox::Critical, "Error", txt_string);
        messagewindow.exec();
        free(cnv_buf);
        free(scratchpad);
        EDFCompatDialog->close();
        return;
      }
    }
  }

  progress.reset();

  if(bdf)
  {
    if(bdfplus)
    {
      QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is BDF+ compliant.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is BDF compliant.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
  }
  else
  {
    if(edfplus)
    {
      QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is EDF+ compliant.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Information, "Result", "No errors found, file is EDF compliant.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
  }

  free(cnv_buf);
  free(scratchpad);
  EDFCompatDialog->close();
}



int UI_EDFCompatwindow::is_duration_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0] == '.')||(str[l-1] == '.'))  return 1;

  for(i=0; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



int UI_EDFCompatwindow::is_onset_number(char *str)
{
  int i, l, hasdot = 0;

  l = strlen(str);

  if(l<2)  return 1;

  if((str[0]!='+')&&(str[0]!='-'))  return 1;

  if((str[1] == '.')||(str[l-1] == '.'))  return 1;

  for(i=1; i<l; i++)
  {
    if(str[i]=='.')
    {
      if(hasdot)  return 1;
      hasdot++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))  return 1;
    }
  }

  return 0;
}



int UI_EDFCompatwindow::is_integer_number(char *str)
{
  int i=0, l, hasspace = 0, hassign=0, digit=0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0]=='+')||(str[0]=='-'))
  {
    hassign++;
    i++;
  }

  for(; i<l; i++)
  {
    if(str[i]==' ')
    {
      if(!digit)
      {
        return 1;
      }
      hasspace++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))
      {
        return 1;
      }
      else
      {
        if(hasspace)
        {
          return 1;
        }
        digit++;
      }
    }
  }

  if(digit)  return 0;
  else  return 1;
}



int UI_EDFCompatwindow::is_number(char *str)
{
  int i=0, l, hasspace = 0, hassign=0, digit=0, hasdot=0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0]=='+')||(str[0]=='-'))
  {
    hassign++;
    i++;
  }

  if(str[l-1]=='.')
  {
    return 1;
  }

  for(; i<l; i++)
  {
    if(str[i]==' ')
    {
      if(!digit)
      {
        return 1;
      }
      if(str[i-1]=='.')
      {
        return 1;
      }
      hasspace++;
    }
    else
    {
      if(((str[i]<48)||(str[i]>57))&&str[i]!='.')
      {
        return 1;
      }
      else
      {
        if(hasspace)
        {
          return 1;
        }
        if(str[i]=='.')
        {
          if(hasdot||!i)  return 1;
          if(hassign&&(i<2))  return 1;
          hasdot++;
        }
        else
        {
          digit++;
        }
      }
    }
  }

  if(digit)  return 0;
  else  return 1;
}




