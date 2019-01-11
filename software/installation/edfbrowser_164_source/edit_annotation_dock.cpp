/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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



#include "annotations_dock.h"



UI_AnnotationEditwindow::UI_AnnotationEditwindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  file_num = 0;

  dockedit = new QDockWidget("Annotation editor", w_parent);
  dockedit->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
  dockedit->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

  annot_edit_dialog = new QDialog;
  annot_edit_dialog->setMinimumSize(640, 45);

  dockedit->setWidget(annot_edit_dialog);

  descriptionLabel = new QLabel(annot_edit_dialog);
  descriptionLabel->setGeometry(10, 10, 78, 25);
  descriptionLabel->setText("Description");

  annot_descript_lineEdit = new QLineEdit(annot_edit_dialog);
  annot_descript_lineEdit->setGeometry(90, 10, 150, 25);

  onsetLabel = new QLabel(annot_edit_dialog);
  onsetLabel->setGeometry(250, 10, 48, 25);
  onsetLabel->setText("Onset");

  posNegTimebox = new QComboBox(annot_edit_dialog);
  posNegTimebox->setGeometry(300, 10, 35, 25);
  posNegTimebox->setEditable(false);
  posNegTimebox->addItem("+");
  posNegTimebox->addItem("-");

  onset_daySpinbox = new QSpinBox(annot_edit_dialog);
  onset_daySpinbox->setGeometry(335, 10, 45, 25);
  onset_daySpinbox->setRange(0, 99);
  onset_daySpinbox->setSingleStep(1);
  onset_daySpinbox->setValue(0);

  onset_timeEdit = new QTimeEdit(annot_edit_dialog);
  onset_timeEdit->setGeometry(380, 10, 100, 25);
  onset_timeEdit->setDisplayFormat("hh:mm:ss.zzz");
  onset_timeEdit->setMinimumTime(QTime(-1, 0, 0, 0));

  durationLabel = new QLabel(annot_edit_dialog);
  durationLabel->setGeometry(490, 10, 58, 25);
  durationLabel->setText("Duration");

  duration_spinbox = new QDoubleSpinBox(annot_edit_dialog);
  duration_spinbox->setGeometry(550, 10, 120, 25);
  duration_spinbox->setRange(-1.0, 10000.0);
  duration_spinbox->setSingleStep(1.0);
  duration_spinbox->setDecimals(3);
  duration_spinbox->setSuffix(" sec");
  duration_spinbox->setValue(-1.0);

  modifybutton = new QPushButton(annot_edit_dialog);
  modifybutton->setGeometry(720, 10, 100, 25);
  modifybutton->setText("Modify");
  modifybutton->setEnabled(false);

  deletebutton = new QPushButton(annot_edit_dialog);
  deletebutton->setGeometry(840, 10, 100, 25);
  deletebutton->setText("Delete");
  deletebutton->setShortcut(QKeySequence::Delete);
  deletebutton->setEnabled(false);

  createbutton = new QPushButton(annot_edit_dialog);
  createbutton->setGeometry(960, 10, 100, 25);
  createbutton->setText("Create");

  QObject::connect(modifybutton, SIGNAL(clicked()),               this, SLOT(modifyButtonClicked()));
  QObject::connect(deletebutton, SIGNAL(clicked()),               this, SLOT(deleteButtonClicked()));
  QObject::connect(createbutton, SIGNAL(clicked()),               this, SLOT(createButtonClicked()));
  QObject::connect(dockedit,     SIGNAL(visibilityChanged(bool)), this, SLOT(open_close_dock(bool)));
}


void UI_AnnotationEditwindow::open_close_dock(bool visible)
{
  if(mainwindow->files_open != 1)
  {
    dockedit->hide();

    return;
  }

  if(visible==true)
  {
    mainwindow->annot_editor_active = 1;

    mainwindow->show_annot_markers = 1;

    if(mainwindow->annotationlist_backup==NULL)
    {
      mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
    }
  }
  else
  {
    modifybutton->setEnabled(false);

    deletebutton->setEnabled(false);

    mainwindow->annot_editor_active = 0;
  }
}



void UI_AnnotationEditwindow::modifyButtonClicked()
{
  struct annotation_list *annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  annot->onset = annotEditGetOnset();

  if(posNegTimebox->currentIndex() == 1)
  {
    annot->onset = -(annot->onset);
  }

  annot->onset += mainwindow->edfheaderlist[file_num]->starttime_offset;

  if(dblcmp(duration_spinbox->value(), 0.0) > 0)
  {
    snprintf(annot->duration, 16, "%f", duration_spinbox->value());

    annot->long_duration = edfplus_annotation_get_long_from_number(annot->duration);
  }
  else
  {
    annot->duration[0] = 0;

    annot->long_duration = 0LL;
  }

  strncpy(annot->annotation, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annot->annotation[MAX_ANNOTATION_LEN] = 0;

  annot->modified = 1;

  annot->selected = 1;

  mainwindow->annotations_edited = 1;

  mainwindow->annotations_dock[file_num]->updateList();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::deleteButtonClicked()
{
  int sz;

  struct annotation_list *annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  edfplus_annotation_remove_item(annot_list, annot_num);

  sz = edfplus_annotation_size(annot_list);

  if(annot_num < sz)
  {
    annot->selected = 1;

    annot->jump = 1;
  }

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);

  mainwindow->annotations_edited = 1;

  mainwindow->save_act->setEnabled(true);

  mainwindow->annotations_dock[file_num]->updateList();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::createButtonClicked()
{
  struct annotation_list *annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

  struct annotationblock annotation;

  memset(&annotation, 0, sizeof(struct annotationblock));

  annotation.onset = annotEditGetOnset();

  if(posNegTimebox->currentIndex() == 1)
  {
    annotation.onset = -(annotation.onset);
  }

  annotation.onset += mainwindow->edfheaderlist[file_num]->starttime_offset;

  annotation.file_num = file_num;

  if(dblcmp(duration_spinbox->value(), 0.0) > 0)
  {
    snprintf(annotation.duration, 16, "%f", duration_spinbox->value());

    annotation.long_duration = edfplus_annotation_get_long_from_number(annotation.duration);
  }
  else
  {
    annotation.duration[0] = 0;

    annotation.long_duration = 0LL;
  }

  strncpy(annotation.annotation, annot_descript_lineEdit->text().toUtf8().data(), MAX_ANNOTATION_LEN);

  annotation.annotation[MAX_ANNOTATION_LEN] = 0;

  annotation.modified = 1;

  edfplus_annotation_add_item(annot_list, annotation);

  mainwindow->annotations_edited = 1;

  mainwindow->annotations_dock[file_num]->updateList();

  mainwindow->maincurve->update();
}



void UI_AnnotationEditwindow::annotEditSetOnset(long long onset)
{
  QTime ta;

  if(onset < 0LL)
  {
    onset_daySpinbox->setValue((-(onset)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(onset)) / TIME_DIMENSION / 3600) % 24,
            (((-(onset)) / TIME_DIMENSION) % 3600) / 60,
            ((-(onset)) / TIME_DIMENSION) % 60,
            ((-(onset)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(onset / TIME_DIMENSION / 86400);

    ta.setHMS((onset / TIME_DIMENSION / 3600) % 24,
            ((onset / TIME_DIMENSION) % 3600) / 60,
            (onset / TIME_DIMENSION) % 60,
            (onset % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }

  onset_timeEdit->setTime(ta);
}



long long UI_AnnotationEditwindow::annotEditGetOnset(void)
{
  long long tmp;

  tmp = onset_daySpinbox->value() * 86400;
  tmp += onset_timeEdit->time().hour() * 3600;
  tmp += onset_timeEdit->time().minute() * 60;
  tmp += onset_timeEdit->time().second();
  tmp *= TIME_DIMENSION;
  tmp += (onset_timeEdit->time().msec() * (TIME_DIMENSION / 1000));

  if(posNegTimebox->currentIndex() == 1)
  {
    tmp *= -1LL;
  }

  return tmp;
}



void UI_AnnotationEditwindow::annotEditSetDuration(long long duration)
{
  duration_spinbox->setValue(((double)duration) / TIME_DIMENSION);
}



void UI_AnnotationEditwindow::set_selected_annotation(int file_nr, int annot_nr)
{
  long long l_tmp;

  QTime ta;


  file_num = file_nr;

  annot_num = annot_nr;

  struct annotation_list *annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

  struct annotationblock *annot = edfplus_annotation_get_item(annot_list, annot_num);

  annot = edfplus_annotation_get_item(annot_list, annot_num);

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->annotation));

  l_tmp = annot->onset - mainwindow->edfheaderlist[annot->file_num]->starttime_offset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
            (((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
            ((-(l_tmp)) / TIME_DIMENSION) % 60,
            ((-(l_tmp)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

    ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
            ((l_tmp / TIME_DIMENSION) % 3600) / 60,
            (l_tmp / TIME_DIMENSION) % 60,
            (l_tmp % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(true);

  deletebutton->setEnabled(true);
}


void UI_AnnotationEditwindow::set_selected_annotation(struct annotationblock *annot)
{
  int n;

  long long l_tmp;

  QTime ta;

  struct annotation_list *annot_list = &mainwindow->edfheaderlist[file_num]->annot_list;

  file_num = annot->file_num;

  n = edfplus_annotation_get_index(annot_list, annot);

  if(n < 0)  return;

  annot_num = n;

  annot_descript_lineEdit->setText(QString::fromUtf8(annot->annotation));

  l_tmp = annot->onset - mainwindow->edfheaderlist[annot->file_num]->starttime_offset;

  if(l_tmp < 0LL)
  {
    onset_daySpinbox->setValue((-(l_tmp)) / TIME_DIMENSION / 86400);

    ta.setHMS(((-(l_tmp)) / TIME_DIMENSION / 3600) % 24,
            (((-(l_tmp)) / TIME_DIMENSION) % 3600) / 60,
            ((-(l_tmp)) / TIME_DIMENSION) % 60,
            ((-(l_tmp)) % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(1);
  }
  else
  {
    onset_daySpinbox->setValue(l_tmp / TIME_DIMENSION / 86400);

    ta.setHMS((l_tmp / TIME_DIMENSION / 3600) % 24,
            ((l_tmp / TIME_DIMENSION) % 3600) / 60,
            (l_tmp / TIME_DIMENSION) % 60,
            (l_tmp % TIME_DIMENSION) / 10000);

    posNegTimebox->setCurrentIndex(0);
  }
  onset_timeEdit->setTime(ta);

  if(strlen(annot->duration))
  {
    duration_spinbox->setValue(atof(annot->duration));
  }
  else
  {
    duration_spinbox->setValue(-1);
  }

  modifybutton->setEnabled(false);

  deletebutton->setEnabled(false);
}






















