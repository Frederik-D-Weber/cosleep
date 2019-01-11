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



#include "import_annotations.h"


#define XML_FORMAT      0
#define ASCIICSV_FORMAT 1
#define DCEVENT_FORMAT  2
#define EDFPLUS_FORMAT  3
#define MITWFDB_FORMAT  4

#define TAB_CNT         5

#define NOTQRS  0 /* not-QRS (not a getann/putann code) */
#define NORMAL  1 /* normal beat */
#define LBBB  2 /* left bundle branch block beat */
#define RBBB  3 /* right bundle branch block beat */
#define ABERR 4 /* aberrated atrial premature beat */
#define PVC 5 /* premature ventricular contraction */
#define FUSION  6 /* fusion of ventricular and normal beat */
#define NPC 7 /* nodal (junctional) premature beat */
#define APC 8 /* atrial premature contraction */
#define SVPB  9 /* premature or ectopic supraventricular beat */
#define VESC  10  /* ventricular escape beat */
#define NESC  11  /* nodal (junctional) escape beat */
#define PACE  12  /* paced beat */
#define UNKNOWN 13  /* unclassifiable beat */
#define NOISE 14  /* signal quality change */
#define ARFCT 16  /* isolated QRS-like artifact */
#define STCH  18  /* ST change */
#define TCH 19  /* T-wave change */
#define SYSTOLE 20  /* systole */
#define DIASTOLE 21 /* diastole */
#define NOTE  22  /* comment annotation */
#define MEASURE 23  /* measurement annotation */
#define PWAVE 24  /* P-wave peak */
#define BBB 25  /* left or right bundle branch block */
#define PACESP  26  /* non-conducted pacer spike */
#define TWAVE 27  /* T-wave peak */
#define RHYTHM  28  /* rhythm change */
#define UWAVE 29  /* U-wave peak */
#define LEARN 30  /* learning */
#define FLWAV 31  /* ventricular flutter wave */
#define VFON  32  /* start of ventricular flutter/fibrillation */
#define VFOFF 33  /* end of ventricular flutter/fibrillation */
#define AESC  34  /* atrial escape beat */
#define SVESC 35  /* supraventricular escape beat */
#define LINK    36  /* link to external data (aux contains URL) */
#define NAPC  37  /* non-conducted P-wave (blocked APB) */
#define PFUS  38  /* fusion of paced and normal beat */
#define WFON  39  /* waveform onset */
#define PQ  WFON  /* PQ junction (beginning of QRS) */
#define WFOFF 40  /* waveform end */
#define JPT WFOFF /* J point (end of QRS) */
#define RONT  41  /* R-on-T premature ventricular contraction */

/* ... annotation codes between RONT+1 and ACMAX inclusive are user-defined */

#define ACMAX 49  /* value of largest valid annot code (must be < 50) */


static char annotdescrlist[42][48]=
  {"not-QRS","normal beat",
  "left bundle branch block beat", "right bundle branch block beat",
  "aberrated atrial premature beat", "premature ventricular contraction",
  "fusion of ventricular and normal beat", "nodal (junctional) premature beat",
  "atrial premature contraction", "premature or ectopic supraventricular beat",
  "ventricular escape beat", "nodal (junctional) escape beat",
  "paced beat", "unclassifiable beat",
  "signal quality change", "isolated QRS-like artifact",
  "ST change", "T-wave change",
  "systole", "diastole",
  "comment annotation", "measurement annotation",
  "P-wave peak", "left or right bundle branch block",
  "non-conducted pacer spike", "T-wave peak",
  "rhythm change", "U-wave peak",
  "learning", "ventricular flutter wave",
  "start of ventricular flutter/fibrillation", "end of ventricular flutter/fibrillation",
  "atrial escape beat", "supraventricular escape beat",
  "link to external data (aux contains URL)", "non-conducted P-wave (blocked APB)",
  "fusion of paced and normal beat", "waveform onset",
  "waveform end", "R-on-T premature ventricular contraction"};


#define ANNOT_EXT_CNT   8

//#define IMPORT_ANNOTS_DEBUG



UI_ImportAnnotationswindow::UI_ImportAnnotationswindow(QWidget *w_parent)
{
  int i;


  mainwindow = (UI_Mainwindow *)w_parent;

  if(mainwindow->files_open < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not import annotations without opening an EDF/BDF-file first.");
    messagewindow.exec();
    return;
  }

  if(mainwindow->files_open > 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not import annotations when multiple files are opened.\n"
                                                              "Make sure only one EDF/BDF is opened.");
    messagewindow.exec();
    return;
  }

  ImportAnnotsDialog = new QDialog;

  ImportAnnotsDialog->setMinimumSize(520, 500);
  ImportAnnotsDialog->setMaximumSize(520, 500);
  ImportAnnotsDialog->setWindowTitle("Import annotations/events");
  ImportAnnotsDialog->setModal(true);
  ImportAnnotsDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  tabholder = new QTabWidget;

  tab_index_array[ASCIICSV_FORMAT] = 0;
  tab_index_array[DCEVENT_FORMAT] = 1;
  tab_index_array[XML_FORMAT] = 2;
  tab_index_array[EDFPLUS_FORMAT] = 3;
  tab_index_array[MITWFDB_FORMAT] = 4;

  for(i=0; i<TAB_CNT; i++)
  {
    tab[i] = new QWidget;
  }

  SeparatorLabel = new QLabel(ImportAnnotsDialog);
  SeparatorLabel->setText("Column separator");
  SeparatorLabel->setMinimumSize(200, 25);

  OnsetColumnLabel = new QLabel(ImportAnnotsDialog);
  OnsetColumnLabel->setText("Onset column");
  OnsetColumnLabel->setMinimumSize(200, 25);

  DatastartLabel = new QLabel(ImportAnnotsDialog);
  DatastartLabel->setText("Data starts at line");
  DatastartLabel->setMinimumSize(200, 25);

  OnsetTimeLabel = new QLabel(ImportAnnotsDialog);
  OnsetTimeLabel->setText("Onset time coding is");
  OnsetTimeLabel->setMinimumSize(200, 25);

  SeparatorLineEdit = new QLineEdit(ImportAnnotsDialog);
  SeparatorLineEdit->setMaxLength(3);
  SeparatorLineEdit->setText("tab");
  SeparatorLineEdit->setMinimumSize(80, 25);

  DescriptionLineEdit = new QLineEdit(ImportAnnotsDialog);
  DescriptionLineEdit->setMaxLength(20);
  DescriptionLineEdit->setEnabled(false);
  DescriptionLineEdit->setMinimumSize(80, 25);

  OnsetColumnSpinBox = new QSpinBox(ImportAnnotsDialog);
  OnsetColumnSpinBox->setRange(1,256);
  OnsetColumnSpinBox->setValue(1);
  OnsetColumnSpinBox->setMinimumSize(80, 25);

  DurationColumnSpinBox = new QSpinBox(ImportAnnotsDialog);
  DurationColumnSpinBox->setRange(1,256);
  DurationColumnSpinBox->setValue(3);
  DurationColumnSpinBox->setMinimumSize(80, 25);

  DescriptionColumnSpinBox = new QSpinBox(ImportAnnotsDialog);
  DescriptionColumnSpinBox->setRange(1,256);
  DescriptionColumnSpinBox->setValue(2);
  DescriptionColumnSpinBox->setMinimumSize(80, 25);

  DatastartSpinbox = new QSpinBox(ImportAnnotsDialog);
  DatastartSpinbox->setRange(1,100);
  DatastartSpinbox->setValue(1);
  DatastartSpinbox->setMinimumSize(80, 25);

  RelativeTimeComboBox = new QComboBox(ImportAnnotsDialog);
  RelativeTimeComboBox->addItem("in seconds, relative to start of file");
  RelativeTimeComboBox->addItem("hh:mm:ss");
  RelativeTimeComboBox->addItem("hh:mm:ss.xxx");
  RelativeTimeComboBox->addItem("yyyy-mm-ddThh:mm:ss");
  RelativeTimeComboBox->addItem("yyyy-mm-ddThh:mm:ss.xxx");
  RelativeTimeComboBox->setMinimumSize(200, 25);

  DescriptionColumnRadioButton = new QRadioButton("Description column");
  DescriptionColumnRadioButton->setMinimumSize(198, 25);
  DescriptionColumnRadioButton->setChecked(true);
  UseManualDescriptionRadioButton = new QRadioButton("Manual description");
  UseManualDescriptionRadioButton->setMinimumSize(198, 25);

  DurationCheckBox = new QCheckBox(" Duration column", ImportAnnotsDialog);
  DurationCheckBox->setTristate(false);
  DurationCheckBox->setCheckState(Qt::Unchecked);
  DurationCheckBox->setMinimumSize(198, 25);

  asciiSettingsHBoxLayout1 = new QHBoxLayout;
  asciiSettingsHBoxLayout1->addWidget(SeparatorLabel);
  asciiSettingsHBoxLayout1->addWidget(SeparatorLineEdit);
  asciiSettingsHBoxLayout1->addStretch(100);

  asciiSettingsHBoxLayout2 = new QHBoxLayout;
  asciiSettingsHBoxLayout2->addWidget(OnsetColumnLabel);
  asciiSettingsHBoxLayout2->addWidget(OnsetColumnSpinBox);
  asciiSettingsHBoxLayout2->addStretch(100);

  asciiSettingsHBoxLayout3 = new QHBoxLayout;
  asciiSettingsHBoxLayout3->addWidget(DescriptionColumnRadioButton);
  asciiSettingsHBoxLayout3->addWidget(DescriptionColumnSpinBox);
  asciiSettingsHBoxLayout3->addStretch(100);

  asciiSettingsHBoxLayout4 = new QHBoxLayout;
  asciiSettingsHBoxLayout4->addWidget(UseManualDescriptionRadioButton);
  asciiSettingsHBoxLayout4->addWidget(DescriptionLineEdit);
  asciiSettingsHBoxLayout4->addStretch(100);

  asciiSettingsHBoxLayout7 = new QHBoxLayout;
  asciiSettingsHBoxLayout7->addWidget(DurationCheckBox);
  asciiSettingsHBoxLayout7->addWidget(DurationColumnSpinBox);
  asciiSettingsHBoxLayout7->addStretch(100);

  asciiSettingsHBoxLayout5 = new QHBoxLayout;
  asciiSettingsHBoxLayout5->addWidget(DatastartLabel);
  asciiSettingsHBoxLayout5->addWidget(DatastartSpinbox);
  asciiSettingsHBoxLayout5->addStretch(100);

  asciiSettingsHBoxLayout6 = new QHBoxLayout;
  asciiSettingsHBoxLayout6->addWidget(OnsetTimeLabel);
  asciiSettingsHBoxLayout6->addWidget(RelativeTimeComboBox);
  asciiSettingsHBoxLayout6->addStretch(100);

  asciiSettingsVBoxLayout = new QVBoxLayout;
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout1);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout2);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout3);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout4);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout7);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout5);
  asciiSettingsVBoxLayout->addLayout(asciiSettingsHBoxLayout6);
  tab[tab_index_array[ASCIICSV_FORMAT]]->setLayout(asciiSettingsVBoxLayout);

  DCEventSignalLabel = new QLabel(ImportAnnotsDialog);
  DCEventSignalLabel->setText("Signal");
  DCEventSignalLabel->setMinimumSize(130, 25);

  DCEventBitTimeLabel = new QLabel(ImportAnnotsDialog);
  DCEventBitTimeLabel->setText("Bit Time");
  DCEventBitTimeLabel->setMinimumSize(130, 25);

  DCEventTriggerLevelLabel = new QLabel(ImportAnnotsDialog);
  DCEventTriggerLevelLabel->setText("Trigger Level");
  DCEventTriggerLevelLabel->setMinimumSize(130, 25);

  DCEventSignalComboBox = new QComboBox(ImportAnnotsDialog);
  for(i=0; i<mainwindow->signalcomps; i++)
  {
    DCEventSignalComboBox->addItem(mainwindow->signalcomp[i]->signallabel);
  }
  DCEventSignalComboBox->setMinimumSize(130, 25);

  BitTimeSpinbox = new QSpinBox(ImportAnnotsDialog);
  BitTimeSpinbox->setRange(1,1000);
  BitTimeSpinbox->setSuffix(" mS");
  BitTimeSpinbox->setValue(10);
  BitTimeSpinbox->setMinimumSize(130, 25);

  DCEventTriggerLevelSpinBox = new QDoubleSpinBox(ImportAnnotsDialog);
  DCEventTriggerLevelSpinBox->setDecimals(3);
  DCEventTriggerLevelSpinBox->setRange(-10000.0, 10000.0);
  DCEventTriggerLevelSpinBox->setValue(500.0);
  DCEventTriggerLevelSpinBox->setMinimumSize(130, 25);

  if(mainwindow->signalcomps)
  {
    DCEventSignalChanged(0);
  }

  DCEventHBoxLayout1 = new QHBoxLayout;
  DCEventHBoxLayout1->addWidget(DCEventSignalLabel);
  DCEventHBoxLayout1->addWidget(DCEventSignalComboBox);
  DCEventHBoxLayout1->addStretch(2);

  DCEventHBoxLayout2 = new QHBoxLayout;
  DCEventHBoxLayout2->addWidget(DCEventBitTimeLabel);
  DCEventHBoxLayout2->addWidget(BitTimeSpinbox);
  DCEventHBoxLayout2->addStretch(2);

  DCEventHBoxLayout3 = new QHBoxLayout;
  DCEventHBoxLayout3->addWidget(DCEventTriggerLevelLabel);
  DCEventHBoxLayout3->addWidget(DCEventTriggerLevelSpinBox);
  DCEventHBoxLayout3->addStretch(2);

  DCEventVBoxLayout = new QVBoxLayout;
  DCEventVBoxLayout->addStretch(10);
  DCEventVBoxLayout->addLayout(DCEventHBoxLayout1);
  DCEventVBoxLayout->addStretch(2);
  DCEventVBoxLayout->addLayout(DCEventHBoxLayout2);
  DCEventVBoxLayout->addStretch(2);
  DCEventVBoxLayout->addLayout(DCEventHBoxLayout3);
  DCEventVBoxLayout->addStretch(10);

  tab[tab_index_array[DCEVENT_FORMAT]]->setLayout(DCEventVBoxLayout);

  SampleTimeLabel = new QLabel(ImportAnnotsDialog);
  SampleTimeLabel->setText("Samplefrequency:");
  SampleTimeLabel->setMinimumSize(130, 25);

  SampleTimeSpinbox = new QSpinBox(ImportAnnotsDialog);
  SampleTimeSpinbox->setRange(0,100000);
  SampleTimeSpinbox->setSuffix(" Hz");
  SampleTimeSpinbox->setMinimumSize(130, 25);
  SampleTimeSpinbox->setValue(get_samplefreq_inf());
  if(SampleTimeSpinbox->value() > 0)
  {
    SampleTimeSpinbox->setEnabled(false);
  }

  importStandardLabel  = new QLabel(ImportAnnotsDialog);
  importStandardLabel->setText("Import Standard Annotations:");
  importStandardLabel->setMinimumSize(130, 25);

  importStandardCheckBox = new QCheckBox;
  importStandardCheckBox->setMinimumSize(30, 30);
  importStandardCheckBox->setTristate(false);
  importStandardCheckBox->setCheckState(Qt::Checked);

  importAuxLabel  = new QLabel(ImportAnnotsDialog);
  importAuxLabel->setText("Import Auxiliary Info:");
  importAuxLabel->setMinimumSize(130, 25);

  importAuxCheckBox = new QCheckBox;
  importAuxCheckBox->setMinimumSize(30, 30);
  importAuxCheckBox->setTristate(false);
  importAuxCheckBox->setCheckState(Qt::Checked);

  mitwfdbHBoxLayout1 = new QHBoxLayout;
  mitwfdbHBoxLayout1->addWidget(SampleTimeLabel);
  mitwfdbHBoxLayout1->addWidget(SampleTimeSpinbox);
  mitwfdbHBoxLayout1->addStretch(2);

  mitwfdbHBoxLayout2 = new QHBoxLayout;
  mitwfdbHBoxLayout2->addWidget(importStandardLabel);
  mitwfdbHBoxLayout2->addWidget(importStandardCheckBox);
  mitwfdbHBoxLayout2->addStretch(2);

  mitwfdbHBoxLayout3 = new QHBoxLayout;
  mitwfdbHBoxLayout3->addWidget(importAuxLabel);
  mitwfdbHBoxLayout3->addWidget(importAuxCheckBox);
  mitwfdbHBoxLayout3->addStretch(2);

  mitwfdbVBoxLayout1 = new QVBoxLayout;
  mitwfdbVBoxLayout1->addStretch(100);
  mitwfdbVBoxLayout1->addLayout(mitwfdbHBoxLayout1);
  mitwfdbVBoxLayout1->addStretch(30);
  mitwfdbVBoxLayout1->addLayout(mitwfdbHBoxLayout2);
  mitwfdbVBoxLayout1->addStretch(30);
  mitwfdbVBoxLayout1->addLayout(mitwfdbHBoxLayout3);
  mitwfdbVBoxLayout1->addStretch(100);

  tab[tab_index_array[MITWFDB_FORMAT]]->setLayout(mitwfdbVBoxLayout1);

  tabholder->addTab(tab[tab_index_array[ASCIICSV_FORMAT]], "ASCII / CSV");
  tabholder->addTab(tab[tab_index_array[DCEVENT_FORMAT]],  "DC-event (8-bit serial code)");
  tabholder->addTab(tab[tab_index_array[XML_FORMAT]],      "XML");
  tabholder->addTab(tab[tab_index_array[EDFPLUS_FORMAT]],  "EDF+ / BDF+");
  tabholder->addTab(tab[tab_index_array[MITWFDB_FORMAT]],  "MIT / WFDB");

  IgnoreConsecutiveCheckBox = new QCheckBox(" Ignore consecutive events with the\n same description");
  IgnoreConsecutiveCheckBox->setMinimumSize(300, 40);
  IgnoreConsecutiveCheckBox->setTristate(false);
  IgnoreConsecutiveCheckBox->setCheckState(Qt::Unchecked);

  ImportButton = new QPushButton(ImportAnnotsDialog);
  ImportButton->setMinimumSize(80, 25);
  ImportButton->setText("Import");

  CloseButton = new QPushButton(ImportAnnotsDialog);
  CloseButton->setMinimumSize(80, 25);
  CloseButton->setText("Cancel");

  helpButton = new QPushButton(ImportAnnotsDialog);
  helpButton->setMinimumSize(80, 25);
  helpButton->setText("Help");

  horLayout = new QHBoxLayout;
  horLayout->addSpacing(30);
  horLayout->addWidget(ImportButton, 1);
  horLayout->addSpacing(120);
  horLayout->addWidget(helpButton, 1);
  horLayout->addSpacing(120);
  horLayout->addWidget(CloseButton, 1);
  horLayout->addSpacing(30);

  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabholder, 300);
  mainLayout->addSpacing(30);
  mainLayout->addWidget(IgnoreConsecutiveCheckBox, 30);
  mainLayout->addSpacing(30);
  mainLayout->addLayout(horLayout, 30);

  ImportAnnotsDialog->setLayout(mainLayout);

  SeparatorLineEdit->setText(mainwindow->import_annotations_var->separator);
  OnsetColumnSpinBox->setValue(mainwindow->import_annotations_var->onsetcolumn);
  DescriptionColumnSpinBox->setValue(mainwindow->import_annotations_var->descriptioncolumn);
  DescriptionLineEdit->setText(mainwindow->import_annotations_var->description);
  DurationColumnSpinBox->setValue(mainwindow->import_annotations_var->durationcolumn);
  DatastartSpinbox->setValue(mainwindow->import_annotations_var->datastartline);
  RelativeTimeComboBox->setCurrentIndex(mainwindow->import_annotations_var->onsettimeformat);
  BitTimeSpinbox->setValue(mainwindow->import_annotations_var->dceventbittime);
  DCEventTriggerLevelSpinBox->setValue(mainwindow->import_annotations_var->triggerlevel);

  if(mainwindow->import_annotations_var->manualdescription == 0)
  {
    DescriptionColumnRadioButton->setChecked(true);
    DescriptionColumnSpinBox->setEnabled(true);
    DescriptionLineEdit->setEnabled(false);
  }
  else
  {
    UseManualDescriptionRadioButton->setChecked(true);
    DescriptionColumnSpinBox->setEnabled(false);
    DescriptionLineEdit->setEnabled(true);
    if(mainwindow->import_annotations_var->format == ASCIICSV_FORMAT)
    {
      IgnoreConsecutiveCheckBox->setEnabled(false);
    }
  }

  if(mainwindow->import_annotations_var->useduration == 1)
  {
    DurationCheckBox->setCheckState(Qt::Checked);
    DurationColumnSpinBox->setEnabled(true);
  }
  else
  {
    DurationCheckBox->setCheckState(Qt::Unchecked);
    DurationColumnSpinBox->setEnabled(false);
  }

  if(mainwindow->import_annotations_var->ignoreconsecutive == 1)
  {
    IgnoreConsecutiveCheckBox->setCheckState(Qt::Checked);
  }
  else
  {
    IgnoreConsecutiveCheckBox->setCheckState(Qt::Unchecked);
  }

  if(mainwindow->import_annotations_var->format == EDFPLUS_FORMAT)
  {
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }

  tabholder->setCurrentIndex(tab_index_array[mainwindow->import_annotations_var->format]);

  QObject::connect(CloseButton,                     SIGNAL(clicked()),                ImportAnnotsDialog, SLOT(close()));
  QObject::connect(ImportButton,                    SIGNAL(clicked()),                this,               SLOT(ImportButtonClicked()));
  QObject::connect(DCEventSignalComboBox,           SIGNAL(currentIndexChanged(int)), this,               SLOT(DCEventSignalChanged(int)));
  QObject::connect(DescriptionColumnRadioButton,    SIGNAL(toggled(bool)),            this,               SLOT(descriptionRadioButtonClicked(bool)));
  QObject::connect(UseManualDescriptionRadioButton, SIGNAL(toggled(bool)),            this,               SLOT(descriptionRadioButtonClicked(bool)));
  QObject::connect(DurationCheckBox,                SIGNAL(stateChanged(int)),        this,               SLOT(DurationCheckBoxChanged(int)));
  QObject::connect(tabholder,                       SIGNAL(currentChanged(int)),      this,               SLOT(TabChanged(int)));
  QObject::connect(helpButton,                      SIGNAL(clicked()),                this,               SLOT(helpbuttonpressed()));

  ImportAnnotsDialog->exec();
}


void UI_ImportAnnotationswindow::DurationCheckBoxChanged(int state)
{
  if(state == Qt::Unchecked)
  {
    DurationColumnSpinBox->setEnabled(false);
  }
  else
  {
    DurationColumnSpinBox->setEnabled(true);
  }
}


void UI_ImportAnnotationswindow::descriptionRadioButtonClicked(bool)
{
  if(DescriptionColumnRadioButton->isChecked() == true)
  {
    DescriptionColumnSpinBox->setEnabled(true);
    DescriptionLineEdit->setEnabled(false);
    IgnoreConsecutiveCheckBox->setEnabled(true);
  }

  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    DescriptionColumnSpinBox->setEnabled(false);
    DescriptionLineEdit->setEnabled(true);
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }
}


void UI_ImportAnnotationswindow::TabChanged(int index)
{
  if((index == tab_index_array[XML_FORMAT]) ||
     (index == tab_index_array[DCEVENT_FORMAT]) ||
     (index == tab_index_array[MITWFDB_FORMAT]))
  {
    IgnoreConsecutiveCheckBox->setEnabled(true);
  }

  if(index == tab_index_array[EDFPLUS_FORMAT])
  {
    IgnoreConsecutiveCheckBox->setEnabled(false);
  }

  if(index == tab_index_array[ASCIICSV_FORMAT])
  {
    if(UseManualDescriptionRadioButton->isChecked() == true)
    {
      IgnoreConsecutiveCheckBox->setEnabled(false);
    }
    else
    {
      IgnoreConsecutiveCheckBox->setEnabled(true);
    }
  }
}


void UI_ImportAnnotationswindow::DCEventSignalChanged(int index)
{
  char scratchpad[64];


  if((index < 0) || (!mainwindow->signalcomps))
  {
    DCEventTriggerLevelSpinBox->setSuffix("");

    return;
  }

  strcpy(scratchpad, " ");
  strcat(scratchpad, mainwindow->signalcomp[index]->physdimension);

  DCEventTriggerLevelSpinBox->setSuffix(scratchpad);
}


void UI_ImportAnnotationswindow::ImportButtonClicked()
{
  int i,
      input_format,
      error=0;

  char str[4096]={""};

  mal_formatted_lines = 0;

  ImportAnnotsDialog->setEnabled(false);

  i = tabholder->currentIndex();

  for(input_format = 0; input_format < TAB_CNT; input_format++)
  {
    if(tab_index_array[input_format] == i)
    {
      break;
    }
  }

  if(input_format >= TAB_CNT)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error (input_format >= TAB_CNT).");
    messagewindow.exec();
    ImportAnnotsDialog->setEnabled(true);
    return;
  }

  mainwindow->import_annotations_var->format = input_format;

  if(input_format == MITWFDB_FORMAT)
  {
    error = import_from_mitwfdb();
  }

  if(input_format == DCEVENT_FORMAT)
  {
    error = import_from_dcevent();
  }

  if(input_format == EDFPLUS_FORMAT)
  {
    error = import_from_edfplus();
  }

  if(input_format == XML_FORMAT)
  {
    error = import_from_xml();
  }

  if(input_format == ASCIICSV_FORMAT)
  {
    error = import_from_ascii();
  }

  if(mainwindow->annotations_dock[0] == NULL)
  {
    mainwindow->annotations_dock[0] = new UI_Annotationswindow(0, mainwindow);

    mainwindow->addDockWidget(Qt::RightDockWidgetArea, mainwindow->annotations_dock[0]->docklist, Qt::Vertical);

    if(edfplus_annotation_size(&mainwindow->edfheaderlist[0]->annot_list) < 1)
    {
      mainwindow->annotations_dock[0]->docklist->hide();
    }
  }

  if(edfplus_annotation_size(&mainwindow->edfheaderlist[0]->annot_list) > 0)
  {
    mainwindow->annotations_dock[0]->docklist->show();

    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[0]->updateList();

    mainwindow->save_act->setEnabled(true);
  }

  mainwindow->maincurve->update();

  if(!error)
  {
    if((input_format == ASCIICSV_FORMAT) && (mal_formatted_lines > 0))
    {
      sprintf(str, "One or more lines were skipped because they were malformatted:\n"
                   "line(s):");
      for(i=0; i<mal_formatted_lines; i++)
      {
        sprintf(str + strlen(str), " %i,", mal_formatted_line_nrs[i]);
      }
      QMessageBox messagewindow(QMessageBox::Information, "Ready", str);
      messagewindow.exec();
    }
    else
    {
      QMessageBox messagewindow(QMessageBox::Information, "Ready", "Done.");
      messagewindow.setIconPixmap(QPixmap(":/images/ok.png"));
      messagewindow.exec();
    }
  }

  ImportAnnotsDialog->setEnabled(true);

  if(!error)
  {
    ImportAnnotsDialog->close();
  }
}


int UI_ImportAnnotationswindow::import_from_mitwfdb(void)
{
  int len,
      annot_code,
      tc=0,
      skip,
      total_annots=0,
      ignore_consecutive=0,
      import_std_annots=1,
      import_aux_info=1,
      last_std_code=-99;

  long long bytes_read,
            filesize,
            sampletime;

  char path[MAX_PATH_LENGTH],
       last_description_aux[256]={""},
       aux_str[256]={""};

  unsigned char a_buf[128];

  struct annotationblock annotation;

  FILE *inputfile=NULL;

  if(SampleTimeSpinbox->value() < 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Please set the samplefrequency.\n"
      "The onset time of the annotations in MIT/WFDB format are expressed in samples offset from the start of the recording.\n"
      "Because your file contains different samplerates, you need to specify which samplerate should be used to\n"
      "calculate the onset time of the annotations."
    );
    messagewindow.exec();
    return 1;
  }

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  if(importStandardCheckBox->checkState() == Qt::Checked)
  {
    import_std_annots = 1;
  }
  else
  {
    import_std_annots = 0;
  }

  if(importAuxCheckBox->checkState() == Qt::Checked)
  {
    import_aux_info = 1;
  }
  else
  {
    import_aux_info = 0;
  }

  sampletime = TIME_DIMENSION / SampleTimeSpinbox->value();

  strcpy(path, QFileDialog::getOpenFileName(0, "Open MIT WFDB annotation file", QString::fromLocal8Bit(mainwindow->recent_opendir), "MIT annotation files (*.ari *.ecg *.trigger *.qrs *.atr *.apn *.st *.pwave)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return 1;
  }

  fseeko(inputfile, 0LL, SEEK_END);
  filesize = ftello(inputfile);

  QProgressDialog progress("Converting annotations ...", "Abort", 0, filesize);

  fseeko(inputfile, 0LL, SEEK_SET);

  for(bytes_read=0LL; bytes_read < filesize; bytes_read += 2LL)
  {
    if(!(bytes_read % 100))
    {
      progress.setValue(bytes_read);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    skip = 0;

    if(fread(a_buf, 2, 1, inputfile) != 1)
    {
      break;
    }

#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    if(*((unsigned short *)a_buf) == 0)  // end of file
    {
      break;
    }

    annot_code = a_buf[1] >> 2;

    if(annot_code == 59)  /* SKIP */
    {
      if(fread(a_buf, 4, 1, inputfile) != 1)
      {
        break;
      }

      tc += (*((unsigned short *)a_buf) << 16);

      tc += *((unsigned short *)(a_buf + 2));
    }
    else if(annot_code == 63)  /* AUX */
      {
        skip = *((unsigned short *)a_buf) & 0x3ff;

        len = skip;
        if(len > 255)  len = 255;

        if(skip % 2) skip++;

        skip -= len;

        if(fread(aux_str, len, 1, inputfile) != 1)
        {
          break;
        }

        aux_str[len] = 0;

        if(len && import_aux_info)
        {
          if((!ignore_consecutive) || (strcmp(aux_str, last_description_aux)))
          {
            memset(&annotation, 0, sizeof(struct annotationblock));
            annotation.onset = (long long)tc * sampletime;
            strncpy(annotation.annotation, aux_str, MAX_ANNOTATION_LEN);

            annotation.annotation[MAX_ANNOTATION_LEN] = 0;

            if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
            {
              progress.reset();
              QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
              messagewindow.exec();
              fclose(inputfile);
              return 1;
            }

            total_annots++;

            strcpy(last_description_aux, aux_str);
          }
        }
      }
      else if((annot_code >= 0) && (annot_code <= ACMAX))
        {
          tc += *((unsigned short *)a_buf) & 0x3ff;

#pragma GCC diagnostic warning "-Wstrict-aliasing"

          if(import_std_annots)
          {
            if((!ignore_consecutive) || (annot_code != last_std_code))
            {
              memset(&annotation, 0, sizeof(struct annotationblock));
              annotation.onset = (long long)tc * sampletime;
              if(annot_code < 42)
              {
                strncpy(annotation.annotation, annotdescrlist[annot_code], MAX_ANNOTATION_LEN);
              }
              else
              {
                strncpy(annotation.annotation, "user-defined", MAX_ANNOTATION_LEN);
              }

              annotation.annotation[MAX_ANNOTATION_LEN] = 0;

              if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
              {
                progress.reset();
                QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
                messagewindow.exec();
                fclose(inputfile);
                return 1;
              }

              total_annots++;

              last_std_code = annot_code;
            }
          }
        }

    if(skip)
    {
      if(fseek(inputfile, skip, SEEK_CUR) < 0)
      {
        break;
      }

      bytes_read += skip;
    }
  }

  fclose(inputfile);

  progress.reset();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_xml(void)
{
  int i, j,
      digits,
      ignore_consecutive=0;

  char path[MAX_PATH_LENGTH]={""},
       last_description[256]={""},
       result[XML_STRBUFLEN]={""},
       duration[32]={""};

  long long onset=0LL,
            l_temp,
            utc_time=0LL;

  struct annotationblock annotation;

  struct date_time_struct date_time;

  struct xml_handle *xml_hdl;


  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  strcpy(path, QFileDialog::getOpenFileName(0, "Open XML file", QString::fromLocal8Bit(mainwindow->recent_opendir), "XML files (*.xml *.XML)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(path);
  if(xml_hdl==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return 1;
  }

  if((xml_hdl->encoding != 1) && (xml_hdl->encoding != 2))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Encoding of XML-file must be UTF-8 or ISO-8859-1.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return 1;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "annotationlist"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find root element \"annotationlist\".");
    messagewindow.exec();
    xml_close(xml_hdl);
    return 1;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(j=0; j<10; j++)  qApp->processEvents();

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  for(i=0; i<100000; i++)
  {
    if(xml_goto_nth_element_inside(xml_hdl, "annotation", i))
    {
      if(i == 0)
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not find child element \"annotation\".");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "onset", 0))
    {
      xml_go_up(xml_hdl);
      continue;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QApplication::restoreOverrideCursor();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not get content of element \"annotation\".");
      messagewindow.exec();
      xml_close(xml_hdl);
      return 1;
    }

    if(strlen(result) > 17)
    {
      if((result[4] == '-') && (result[7] == '-') && (result[10] == 'T') && (result[13] == ':') && (result[16] == ':'))
      {
        date_time.year = atoi(result);
        date_time.month = atoi(result + 5);
        date_time.day = atoi(result + 8);
        date_time.hour = atoi(result + 11);
        date_time.minute = atoi(result + 14);
        date_time.second = atoi(result + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_DIMENSION;

        if(strlen(result) > 19)
        {
          if(result[19] == '.')
          {
            for(digits=0; digits<32; digits++)
            {
              if((result[20 + digits] < '0') || (result[20 + digits] > '9'))
              {
                break;
              }
            }
            result[20 + digits] = 0;
            if(digits)
            {
              l_temp = (atoi(result + 20) * TIME_DIMENSION);
              for(; digits>0; digits--)
              {
                l_temp /= 10LL;
              }
              onset += l_temp;
            }
          }
          else
          {
            xml_go_up(xml_hdl);
            continue;
          }
        }
      }
    }
    else
    {
      xml_go_up(xml_hdl);
      continue;
    }

    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "duration", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not get content of element \"duration\".");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      strncpy(duration, result, 16);
      duration[15] = 0;
      if((!(is_number(duration))) && (duration[0] != '-'))
      {
        remove_trailing_zeros(duration);
      }
      else
      {
        duration[0] = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "description", 0))
    {
      xml_go_up(xml_hdl);
      continue;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QApplication::restoreOverrideCursor();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not get content of element \"description\".");
      messagewindow.exec();
      xml_close(xml_hdl);
      return 1;
    }

    if((!ignore_consecutive) || (strcmp(result, last_description)))
    {
      memset(&annotation, 0, sizeof(struct annotationblock));
      annotation.onset = onset;
      strncpy(annotation.annotation, result, MAX_ANNOTATION_LEN);
      if(xml_hdl->encoding == 1)
      {
        latin1_to_utf8(annotation.annotation, MAX_ANNOTATION_LEN);
      }
      annotation.annotation[MAX_ANNOTATION_LEN] = 0;
      strcpy(annotation.duration, duration);
      annotation.long_duration = edfplus_annotation_get_long_from_number(duration);
      if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
      {
        QApplication::restoreOverrideCursor();
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
        messagewindow.exec();
        xml_close(xml_hdl);
        return 1;
      }

      strcpy(last_description, result);
    }

    xml_go_up(xml_hdl);

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);

  QApplication::restoreOverrideCursor();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_ascii(void)
{
  int j,
      column,
      line_nr,
      startline=1,
      onset_column=1,
      descr_column=2,
      duration_column=3,
      onset_is_set,
      descr_is_set,
      duration_is_set,
      max_descr_length=40,
      onsettime_coding=0,
      ignore_consecutive=0,
      use_duration=0,
      manualdescription,
      len;

  char path[MAX_PATH_LENGTH]={""},
       line[4096]={""},
       str[4096]={""},
       separator[2]=";",
       description[256]={""},
       last_description[256]={""},
       duration[32]={""},
       *charpntr=NULL,
       *saveptr=NULL;

  long long onset=0LL,
            last_onset=0LL;

  FILE *inputfile=NULL;

  struct annotationblock annotation;


  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    manualdescription = 1;

    strcpy(description, DescriptionLineEdit->text().toLatin1().data());
  }
  else
  {
    manualdescription = 0;
  }

  strcpy(str, SeparatorLineEdit->text().toLatin1().data());

  if(!strcmp(str, "tab"))
  {
    separator[0] = '\t';
  }
  else
  {
    if(strlen(str)!=1)
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator must be one character or \"tab\".");
      messagewindow.exec();
      return 1;
    }

    if((str[0]<32)||(str[0]>126))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character is not a valid ASCII character.");
      messagewindow.exec();
      return 1;
    }

    if(str[0]=='.')
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character can not be a dot.");
      messagewindow.exec();
      return 1;
    }

    if((str[0]>='0')&&(str[0]<='9'))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Separator character can not be a number.");
      messagewindow.exec();
      return 1;
    }

    separator[0] = str[0];
  }

  strcpy(mainwindow->import_annotations_var->separator, str);

  startline = DatastartSpinbox->value();

  descr_column = DescriptionColumnSpinBox->value() - 1;

  onset_column = OnsetColumnSpinBox->value() - 1;

  duration_column = DurationColumnSpinBox->value() - 1;

  onsettime_coding = RelativeTimeComboBox->currentIndex();

  if(DurationCheckBox->checkState() == Qt::Checked)
  {
    use_duration = 1;
  }
  else
  {
    use_duration = 0;
  }

  if((descr_column == onset_column) && (!manualdescription))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Onset and Description can not be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((duration_column == onset_column) && use_duration)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Onset and Duration can not be in the same column.");
    messagewindow.exec();
    return 1;
  }

  if((descr_column == duration_column) && (!manualdescription) && use_duration)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Invalid input", "Duration and Description can not be in the same column.");
    messagewindow.exec();
    return 1;
  }

  mainwindow->import_annotations_var->onsettimeformat = onsettime_coding;
  mainwindow->import_annotations_var->onsetcolumn = onset_column + 1;
  mainwindow->import_annotations_var->descriptioncolumn = descr_column + 1;
  mainwindow->import_annotations_var->durationcolumn = duration_column + 1;
  mainwindow->import_annotations_var->useduration = use_duration;
  mainwindow->import_annotations_var->datastartline = startline;
  if(UseManualDescriptionRadioButton->isChecked() == true)
  {
    mainwindow->import_annotations_var->manualdescription = 1;
  }
  else
  {
    mainwindow->import_annotations_var->manualdescription = 0;
  }
  strcpy(mainwindow->import_annotations_var->description, DescriptionLineEdit->text().toLatin1().data());

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  strcpy(path, QFileDialog::getOpenFileName(0, "Open ASCII file", QString::fromLocal8Bit(mainwindow->recent_opendir), "ASCII files (*.txt *.TXT *.csv *.CSV)").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return 1;
  }

  rewind(inputfile);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(j=0; j<10; j++)  qApp->processEvents();

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  if(use_duration == 0)  duration_column = -1;

  mal_formatted_lines = 0;

  for(line_nr=1; !feof(inputfile); line_nr++)
  {
    if(line_nr == 0x7fffffff)  break;

    if(fgets(line, 4096, inputfile) == NULL)
    {
      break;
    }

    if(line_nr < startline)  continue;

    len = strlen(line);

    if(line[len-1] == '\n')
    {
      line[len-1] = 0;

      if(--len == 0)
      {
        continue;
      }
    }

    onset_is_set = 0;
    descr_is_set = 0;
    duration_is_set = 0;
    duration[0] = 0;

    for(column=0; column<32; column++)
    {
      if(column == 0)
      {
        charpntr = strtok_r_e(line, separator, &saveptr);
      }
      else
      {
        charpntr = strtok_r_e(NULL, separator, &saveptr);
      }

      if(charpntr == NULL)
      {
        break;
      }
      else
      {
        if(column == onset_column)
        {
          if(!strlen(charpntr))  continue;
#ifdef IMPORT_ANNOTS_DEBUG
          printf("  onset: ->%s<-", charpntr);
#endif
          if(get_onset_time_from_ascii(charpntr, &onset, &last_onset, onsettime_coding) == 0)
          {
            onset_is_set = 1;
          }
        }
        else if(column == descr_column)
          {
            if(!strlen(charpntr))  continue;
#ifdef IMPORT_ANNOTS_DEBUG
            printf("  description: ->%s<-", charpntr);
#endif
            strncpy(description, charpntr, max_descr_length);
            description[max_descr_length] = 0;
            latin1_to_utf8(description, max_descr_length);
            descr_is_set = 1;
          }
          else if(column == duration_column)
            {
#ifdef IMPORT_ANNOTS_DEBUG
              printf("  duration: ->%s<-", charpntr);
#endif
              strncpy(duration, charpntr, 15);
              duration[15] = 0;
              duration_is_set = 1;
            }
      }
    }
#ifdef IMPORT_ANNOTS_DEBUG
    printf("  line %i\n", line_nr);
#endif
    if(onset_is_set && descr_is_set)
    {
      if((!ignore_consecutive) || strcmp(description, last_description))
      {
        memset(&annotation, 0, sizeof(struct annotationblock));
        annotation.onset = onset;
        strncpy(annotation.annotation, description, MAX_ANNOTATION_LEN);
        annotation.annotation[MAX_ANNOTATION_LEN] = 0;
        if(use_duration && duration_is_set)
        {
          if((!(is_number(duration))) && (duration[0] != '-'))
          {
            remove_trailing_zeros(duration);
            if(duration[0] == '+')
            {
              strcpy(annotation.duration, duration + 1);
            }
            else
            {
              strcpy(annotation.duration, duration);
            }

            annotation.long_duration = edfplus_annotation_get_long_from_number(duration);
          }
        }
        if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
        {
          QApplication::restoreOverrideCursor();
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
          messagewindow.exec();
          fclose(inputfile);
          return 1;
        }

        strcpy(last_description, description);
      }
    }
    else
    {
      if(mal_formatted_lines < 32)
      {
        mal_formatted_line_nrs[mal_formatted_lines++] = line_nr;
      }
    }
  }

  QApplication::restoreOverrideCursor();

  fclose(inputfile);

  return 0;
}


int UI_ImportAnnotationswindow::import_from_edfplus(void)
{
  int i,
      annotlist_size;

  char path[MAX_PATH_LENGTH],
       str[2048];

  long long starttime_diff;

  FILE *inputfile=NULL;

  struct edfhdrblock *edfhdr=NULL;

  struct annotationblock *annotation;


  strcpy(path, QFileDialog::getOpenFileName(0, "Open EDF+/BDF+ file", QString::fromLocal8Bit(mainwindow->recent_opendir), "EDF/BDF files (*.edf *.EDF *.bdf *.BDF )").toLocal8Bit().data());

  if(!strcmp(path, ""))
  {
    return 1;
  }

  get_directory_from_path(mainwindow->recent_opendir, path, MAX_PATH_LENGTH);

  inputfile = fopeno(path, "rb");
  if(inputfile==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return 1;
  }

  rewind(inputfile);

  EDFfileCheck EDFfilechecker;

  str[0] = 0;

  edfhdr = EDFfilechecker.check_edf_file(inputfile, str, 0);
  if(edfhdr==NULL)
  {
    strcat(str, "\n File is not a valid EDF or BDF file.");
    QMessageBox messagewindow(QMessageBox::Critical, "Error", str);
    messagewindow.exec();
    fclose(inputfile);
    return 1;
  }

  if(!(edfhdr->edfplus || edfhdr->bdfplus))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "File is not an EDF+ or BDF+ file.");
    messagewindow.exec();
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  strcpy(edfhdr->filename, path);

  edfhdr->file_hdl = inputfile;

  EDF_annotations annotations;

  annotations.get_annotations(edfhdr, mainwindow->read_nk_trigger_signal);
  if(edfhdr->annots_not_read)
  {
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  annotlist_size = edfplus_annotation_size(&edfhdr->annot_list);
  if(annotlist_size < 1)
  {
    QMessageBox messagewindow(QMessageBox::Information, "Import annotations", "No annotations found.");
    messagewindow.exec();
    edfplus_annotation_empty_list(&edfhdr->annot_list);
    free(edfhdr->edfparam);
    free(edfhdr);
    fclose(inputfile);
    return 1;
  }

  starttime_diff = edfhdr->utc_starttime - mainwindow->edfheaderlist[mainwindow->sel_viewtime]->utc_starttime;

  starttime_diff *= TIME_DIMENSION;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(mainwindow->annotationlist_backup==NULL)
  {
    mainwindow->annotationlist_backup = edfplus_annotation_create_list_copy(&mainwindow->edfheaderlist[0]->annot_list);
  }

  for(i=0; i<annotlist_size; i++)
  {
    annotation = edfplus_annotation_get_item(&edfhdr->annot_list, i);
    annotation->onset += starttime_diff;
    edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, *annotation);
  }

  edfplus_annotation_sort(&mainwindow->edfheaderlist[0]->annot_list, NULL);

  edfplus_annotation_empty_list(&edfhdr->annot_list);
  free(edfhdr->edfparam);
  free(edfhdr);
  fclose(inputfile);

  QApplication::restoreOverrideCursor();

  return 0;
}


int UI_ImportAnnotationswindow::import_from_dcevent(void)
{
  int i,
      ignore_consecutive=0,
      signal_nr,
      smpls_per_datrec,
      bytes_per_datrec,
      recsize,
      jumpbytes,
      bufoffset,
      triggervalue,
      bitwidth,
      bitposition,
      trigger_sample,
      next_sample,
      eventcode,
      tmp_value=0,
      edfformat,
      annotations_found=0;

  char scratchpad[256],
       last_description[256],
       *buf;

  long long datrecs,
            trigger_datrec,
            time_per_sample,
            progress_steps;

  union {
          unsigned int one;
          signed int one_signed;
          unsigned short two[2];
          signed short two_signed[2];
          unsigned char four[4];
        } var;

  FILE *inputfile=NULL;

  struct annotationblock annotation;

  struct signalcompblock *signalcomp;


  last_description[0] = 0;

  if(IgnoreConsecutiveCheckBox->checkState() == Qt::Checked)
  {
    ignore_consecutive = 1;
  }
  else
  {
    ignore_consecutive = 0;
  }

  mainwindow->import_annotations_var->ignoreconsecutive = ignore_consecutive;

  signal_nr = DCEventSignalComboBox->currentIndex();

  if(signal_nr < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "You need to put at least one signal on the screen.");
    messagewindow.exec();
    return 1;
  }

  if(mainwindow->signalcomp[signal_nr]->num_of_signals > 1)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The signal can not be a derivation of multiple signals.");
    messagewindow.exec();
    return 1;
  }

  mainwindow->import_annotations_var->dceventbittime = BitTimeSpinbox->value();

  mainwindow->import_annotations_var->triggerlevel = DCEventTriggerLevelSpinBox->value();

  signalcomp = mainwindow->signalcomp[signal_nr];

  smpls_per_datrec = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  recsize = signalcomp->edfhdr->recordsize;

  bufoffset = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].buf_offset;

  time_per_sample = signalcomp->edfhdr->long_data_record_duration / smpls_per_datrec;

  if(signalcomp->edfhdr->edf)
  {
    edfformat = 1;

    bytes_per_datrec = smpls_per_datrec * 2;
  }
  else
  {
    bytes_per_datrec = smpls_per_datrec * 3;

    edfformat = 0;
  }

  jumpbytes = recsize - bytes_per_datrec;

  inputfile = signalcomp->edfhdr->file_hdl;

  bitwidth = ((long long)mainwindow->import_annotations_var->dceventbittime * 10000LL) / time_per_sample;

  if(bitwidth < 5)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Bit Time is set too low compared to the samplerate of the selected signal.");
    messagewindow.exec();
    return 1;
  }

  triggervalue = mainwindow->import_annotations_var->triggerlevel / signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue;

  triggervalue -= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].offset;

  if(triggervalue >= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].dig_max)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Trigger Level is equal or higher than physical maximum.");
    messagewindow.exec();
    return 1;
  }

  if(triggervalue <= signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].dig_min)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Trigger Level is equal or lower than physical minimum.");
    messagewindow.exec();
    return 1;
  }

  if(fseeko(inputfile, signalcomp->edfhdr->hdrsize + bufoffset, SEEK_SET))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile. (fseek)");
    messagewindow.exec();
    return 1;
  }

  buf = (char *)malloc(bytes_per_datrec);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred. (buf)");
    messagewindow.exec();
    return 1;
  }

  annotations_found = 0;

  QProgressDialog progress("Scanning file for DC-events...", "Abort", 0, (int)signalcomp->edfhdr->datarecords);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = signalcomp->edfhdr->datarecords / 100LL;
  if(progress_steps < 1LL)
  {
    progress_steps = 1LL;
  }

/*  BITPOSITION:
  0  nothing (idle)
  1  rising edge of startbit found
  2  middle of startbit found
  3  middle of bit 0 found
  ........................
  10 middle of bit 7 found
  11 middle of stopbit found
*/

  bitposition = 0;

  eventcode = 0;

  next_sample = 0;

  trigger_sample = 0;

  trigger_datrec = 0LL;

  for(datrecs=0LL; datrecs < signalcomp->edfhdr->datarecords; datrecs++)
  {
    if(annotations_found > 10000)
    {
      progress.reset();

      break;
    }

    if(!(datrecs % progress_steps))
    {
      progress.setValue((int)datrecs);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    if(datrecs)
    {
      fseek(inputfile, jumpbytes, SEEK_CUR);
    }

    if(fread(buf, bytes_per_datrec, 1, inputfile) != 1)
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "An error occurred while reading inputfile. (fread)");
      messagewindow.exec();
      free(buf);
      return 1;
    }

    for(i=0; i<smpls_per_datrec; i++)
    {
      if(edfformat)
      {
        tmp_value = *(((signed short *)buf) + i);
      }
      else
      {
        var.two[0] = *((unsigned short *)(buf + (i * 3)));
        var.four[2] = *((unsigned char *)(buf + (i * 3) + 2));

        if(var.four[2]&0x80)
        {
          var.four[3] = 0xff;
        }
        else
        {
          var.four[3] = 0x00;
        }

        tmp_value = var.one_signed;
      }

      if(bitposition)
      {
        if(i == next_sample)
        {
          if(bitposition == 1)
          {
            if(tmp_value < triggervalue)
            {
              bitposition = 0;

              continue;
            }
          }
          else
          {
            if(bitposition < 10)
            {
              if(tmp_value > triggervalue)
              {
                eventcode += (1 << (bitposition - 2));
              }
            }

            if(bitposition == 10)
            {
              if(tmp_value < triggervalue)
              {
                sprintf(scratchpad, "Trigger ID=%i", eventcode);

                if((!ignore_consecutive) || (strcmp(scratchpad, last_description)))
                {
                  memset(&annotation, 0, sizeof(struct annotationblock));
                  annotation.onset = ((trigger_datrec * signalcomp->edfhdr->long_data_record_duration) + (trigger_sample * time_per_sample));
                  annotation.onset += signalcomp->edfhdr->starttime_offset;
                  annotation.file_num = signalcomp->edfhdr->file_num;
                  strncpy(annotation.annotation, scratchpad, MAX_ANNOTATION_LEN);
                  annotation.annotation[MAX_ANNOTATION_LEN] = 0;
                  if(edfplus_annotation_add_item(&mainwindow->edfheaderlist[0]->annot_list, annotation))
                  {
                    progress.reset();
                    QMessageBox messagewindow(QMessageBox::Critical, "Error", "A memory allocation error occurred (annotation).");
                    messagewindow.exec();
                    free(buf);
                    return 1;
                  }

                  annotations_found++;

                  strcpy(last_description, scratchpad);
                }
              }

              bitposition = 0;

              continue;
            }
          }

          next_sample = (i + bitwidth) % smpls_per_datrec;

          bitposition++;
        }
      }
      else
      {
        if(tmp_value > triggervalue)
        {
          trigger_sample = i;

          trigger_datrec = datrecs;

          bitposition = 1;

          next_sample = (i + (bitwidth / 2)) % smpls_per_datrec;

          eventcode = 0;
        }
      }
    }
  }

  free(buf);

  inputfile = NULL;

  progress.reset();

  return 0;
}


void UI_ImportAnnotationswindow::helpbuttonpressed()
{
#ifdef Q_OS_LINUX
  QDesktopServices::openUrl(QUrl("file:///usr/share/doc/edfbrowser/manual.html#Import_annotations"));
#endif

#ifdef Q_OS_WIN32
  char p_path[MAX_PATH_LENGTH];

  strcpy(p_path, "file:///");
  strcat(p_path, mainwindow->specialFolder(CSIDL_PROGRAM_FILES).toLocal8Bit().data());
  strcat(p_path, "\\EDFbrowser\\manual.html#Import_annotations");
  QDesktopServices::openUrl(QUrl(p_path));
#endif
}


int UI_ImportAnnotationswindow::get_samplefreq_inf(void)
{
  int i, smps=0;

  if(mainwindow->files_open != 1)  return 0;

  for(i=0; i<mainwindow->edfheaderlist[0]->edfsignals; i++)
  {
    if(mainwindow->edfheaderlist[0]->edfparam[i].annotation)  continue;

    if(i == 0)
    {
      smps = mainwindow->edfheaderlist[0]->edfparam[i].smp_per_record;
    }
    else
    {
      if(smps != mainwindow->edfheaderlist[0]->edfparam[i].smp_per_record)  return 0;
    }
  }

  if(!smps)  return 0;

  return ((long long)smps * TIME_DIMENSION) / mainwindow->edfheaderlist[0]->long_data_record_duration;
}


int UI_ImportAnnotationswindow::get_onset_time_from_ascii(const char *str, long long *onset_time, long long *last_onset, int encoding)
{
  int digits;

  long long l_temp, onset = 0LL, utc_time=0LL;

  char scratchpad[64];

  struct date_time_struct date_time;

  strncpy(scratchpad, str, 30);
  scratchpad[30] = 0;

  if(encoding == 0)
  {
    onset = atoll_x(scratchpad, TIME_DIMENSION);
    onset += mainwindow->edfheaderlist[0]->starttime_offset;

    *onset_time = onset;

    return 0;
  }

  if(encoding == 1)
  {
    if(strlen(scratchpad) > 6)
    {
      if((scratchpad[2] == ':') && (scratchpad[5] == ':'))
      {
        scratchpad[8] = 0;
        onset = atoi(scratchpad) * 3600LL;
        onset += (atoi(scratchpad + 3) * 60LL);
        onset += (long long)(atoi(scratchpad + 6));
        onset *= TIME_DIMENSION;
        onset -= mainwindow->edfheaderlist[0]->l_starttime;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_DIMENSION);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
    if(strlen(scratchpad) > 5)
    {
      if((scratchpad[1] == ':') && (scratchpad[4] == ':'))
      {
        scratchpad[7] = 0;
        onset = atoi(scratchpad) * 3600LL;
        onset += (atoi(scratchpad + 2) * 60LL);
        onset += (long long)(atoi(scratchpad + 5));
        onset *= TIME_DIMENSION;
        onset -= mainwindow->edfheaderlist[0]->l_starttime;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_DIMENSION);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(encoding == 2)
  {
    if(strlen(scratchpad) > 8)
    {
      if((scratchpad[2] == ':') && (scratchpad[5] == ':') && ((scratchpad[8] == '.') || (scratchpad[8] == ',')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad[9 + digits] < '0') || (scratchpad[9 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad[9 + digits] = 0;
        onset = atoi(scratchpad) * 3600LL;
        onset += (atoi(scratchpad + 3) * 60LL);
        onset += (long long)(atoi(scratchpad + 6));
        onset *= TIME_DIMENSION;
        if(digits)
        {
          l_temp = (atoi(scratchpad + 9) * TIME_DIMENSION);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }
        onset -= mainwindow->edfheaderlist[0]->l_starttime;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_DIMENSION);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
    if(strlen(scratchpad) > 7)
    {
      if((scratchpad[1] == ':') && (scratchpad[4] == ':') && ((scratchpad[7] == '.') || (scratchpad[7] == ',')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad[8 + digits] < '0') || (scratchpad[8 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad[8 + digits] = 0;
        onset = atoi(scratchpad) * 3600LL;
        onset += (atoi(scratchpad + 2) * 60LL);
        onset += (long long)(atoi(scratchpad + 5));
        onset *= TIME_DIMENSION;
        if(digits)
        {
          l_temp = (atoi(scratchpad + 8) * TIME_DIMENSION);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }
        onset -= mainwindow->edfheaderlist[0]->l_starttime;
        if(onset < *last_onset)
        {
          onset += (86400LL * TIME_DIMENSION);
          *last_onset = onset;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(encoding == 3)
  {
    if(strlen(scratchpad) > 17)
    {
      if((scratchpad[4] == '-') && (scratchpad[7] == '-') && (scratchpad[13] == ':') && (scratchpad[16] == ':'))
      {
        scratchpad[19] = 0;
        date_time.year = atoi(scratchpad);
        date_time.month = atoi(scratchpad + 5);
        date_time.day = atoi(scratchpad + 8);
        date_time.hour = atoi(scratchpad + 11);
        date_time.minute = atoi(scratchpad + 14);
        date_time.second = atoi(scratchpad + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_DIMENSION;

        *onset_time = onset;

        return 0;
      }
    }
  }

  if(encoding == 4)
  {
    if(strlen(scratchpad) > 19)
    {
      if((scratchpad[4] == '-') && (scratchpad[7] == '-') && (scratchpad[13] == ':') && (scratchpad[16] == ':') && ((scratchpad[19] == ',') || (scratchpad[19] == '.')))
      {
        for(digits=0; digits<32; digits++)
        {
          if((scratchpad[20 + digits] < '0') || (scratchpad[20 + digits] > '9'))
          {
            break;
          }
        }
        scratchpad[20 + digits] = 0;
        date_time.year = atoi(scratchpad);
        date_time.month = atoi(scratchpad + 5);
        date_time.day = atoi(scratchpad + 8);
        date_time.hour = atoi(scratchpad + 11);
        date_time.minute = atoi(scratchpad + 14);
        date_time.second = atoi(scratchpad + 17);
        date_time_to_utc(&utc_time, date_time);
        onset = utc_time - mainwindow->edfheaderlist[0]->utc_starttime;
        onset *= TIME_DIMENSION;
        if(digits)
        {
          l_temp = (atoi(scratchpad + 20) * TIME_DIMENSION);
          for(; digits>0; digits--)
          {
            l_temp /= 10LL;
          }
          onset += l_temp;
        }

        *onset_time = onset;

        return 0;
      }
    }
  }

  return -1;
}















