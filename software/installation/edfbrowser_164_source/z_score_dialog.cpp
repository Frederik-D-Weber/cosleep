/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#include "z_score_dialog.h"


#define ZSCORE_EPOCH_LEN 2
#define ZSCORE_F0 0.5
#define ZSCORE_F1 3.0
#define ZSCORE_F3 12.0
#define ZSCORE_F4 25.0

#define ZSCORE_ERRORLEVEL 5.0




UI_ZScoreWindow::UI_ZScoreWindow(QWidget *w_parent, UI_ZScoreWindow **w_zscoredialog, int w_zscore_dialognumber, int w_signalnr)
{
  zscore_epoch_buf = NULL;
  zscore_page_buf = NULL;
  zscore_sleepstage_buf = NULL;

  crossoverfreq = 7.0;

  z_threshold = 0.0;

  zscore_page_len = 15;

  zscore_pages = 0;

  epoch_cntr = 0;

  zscore_wake = 1;

  mainwindow = (UI_Mainwindow *)w_parent;

  zscore_dialognumber = w_zscore_dialognumber;

  signalnr = w_signalnr;

  zscoredialog = w_zscoredialog;

  zscore_dialog_is_destroyed = 0;

  zscore_dialog = new QDialog();
  zscore_dialog->setMinimumSize(600, 550);
  zscore_dialog->setWindowTitle("Z-EEG");
  zscore_dialog->setModal(false);
  zscore_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
  zscore_dialog->setWindowIcon(QIcon(":/images/edf.png"));
  zscore_dialog->setSizeGripEnabled(true);
  zscore_dialog->setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

  crossoverbinLabel = new QLabel;
  crossoverbinLabel->setMinimumSize(100, 25);
  crossoverbinLabel->setMaximumSize(100, 25);
  crossoverbinLabel->setText("Cross-over");

  crossoverSpinbox = new QDoubleSpinBox;
  crossoverSpinbox->setMinimumSize(100, 25);
  crossoverSpinbox->setMaximumSize(100, 25);
  crossoverSpinbox->setRange(5.0, 9.5);
  crossoverSpinbox->setDecimals(1);
  crossoverSpinbox->setSingleStep(0.5);
  crossoverSpinbox->setSuffix(" Hz");
  crossoverSpinbox->setValue(mainwindow->z_score_var.crossoverfreq);
  crossoverSpinbox->setToolTip("Threshold between low-frequency and high-frequency");

  thresholdLabel = new QLabel;
  thresholdLabel->setMinimumSize(100, 25);
  thresholdLabel->setMaximumSize(100, 25);
  thresholdLabel->setText("Z-threshold");

  thresholdSpinbox = new QDoubleSpinBox;
  thresholdSpinbox->setMinimumSize(100, 25);
  thresholdSpinbox->setMaximumSize(100, 25);
  thresholdSpinbox->setRange(-0.5, 0.5);
  thresholdSpinbox->setDecimals(2);
  thresholdSpinbox->setSingleStep(0.05);
  thresholdSpinbox->setValue(mainwindow->z_score_var.z_threshold);

  pagelenLabel = new QLabel;
  pagelenLabel->setMinimumSize(100, 25);
  pagelenLabel->setMaximumSize(100, 25);
  pagelenLabel->setText("Z-page");

  pagelenSpinbox = new QSpinBox;
  pagelenSpinbox->setMinimumSize(100, 25);
  pagelenSpinbox->setMaximumSize(100, 25);
  pagelenSpinbox->setRange(10, 60);
  pagelenSpinbox->setSingleStep(2);
  pagelenSpinbox->setSuffix(" sec");
  pagelenSpinbox->setValue(mainwindow->z_score_var.zscore_page_len);

//   errordetectionLabel = new QLabel;
//   errordetectionLabel->setMinimumSize(100, 25);
//   errordetectionLabel->setMaximumSize(100, 25);
//   errordetectionLabel->setText("Error detection");
//
//   errordetectionSpinbox = new QSpinBox;
//   errordetectionSpinbox->setMinimumSize(100, 25);
//   errordetectionSpinbox->setMaximumSize(100, 25);
//   errordetectionSpinbox->setRange(50, 100);
//   errordetectionSpinbox->setSingleStep(1);
//   errordetectionSpinbox->setSuffix(" \%");
//   errordetectionSpinbox->setValue(mainwindow->z_score_var.zscore_error_detection);
//   errordetectionSpinbox->setToolTip("Enter value of \% total power when any of the four\n"
//                                     "standard bandwidths: delta, theta, alpha, beta\n"
//                                     "exceeds this percent of total power.\n"
//                                     "This value will be evaluated when annotating.");

  hysteresisLabel = new QLabel;
  hysteresisLabel->setMinimumSize(100, 25);
  hysteresisLabel->setMaximumSize(100, 25);
  hysteresisLabel->setText("Sleep/Wake\ndifferentiation");

  hysteresisSpinbox = new QDoubleSpinBox;
  hysteresisSpinbox->setMinimumSize(100, 25);
  hysteresisSpinbox->setMaximumSize(100, 25);
  hysteresisSpinbox->setDecimals(2);
  hysteresisSpinbox->setRange(0.0, 0.25);
  hysteresisSpinbox->setSingleStep(0.01);
  hysteresisSpinbox->setValue(mainwindow->z_score_var.z_hysteresis);
  hysteresisSpinbox->setToolTip("Hysteresis applied to the z-threshold.");

  curve1 = new SignalCurve;
  curve1->setSignalColor(Qt::green);
  curve1->setBackgroundColor(Qt::black);
  curve1->setRasterColor(Qt::gray);
  curve1->setCrosshairColor(Qt::red);
  curve1->setTraceWidth(0);
  curve1->setDashBoardEnabled(false);
  curve1->setUpsidedownEnabled(true);

  StartButton = new QPushButton;
  StartButton->setMinimumSize(100, 25);
  StartButton->setMaximumSize(100, 25);
  StartButton->setText("Calculate");
  StartButton->setToolTip("Plot the Z-ratio / Z-pages");

  jumpButton = new QPushButton;
  jumpButton->setMinimumSize(100, 25);
  jumpButton->setMaximumSize(100, 25);
  jumpButton->setText("Jump");
  jumpButton->setToolTip("Jump to cursor-position");

  get_annotButton = new QPushButton;
  get_annotButton->setMinimumSize(100, 25);
  get_annotButton->setMaximumSize(100, 25);
  get_annotButton->setText("Annotate");
  get_annotButton->setToolTip("Create annotations from the Z-EEG");

  defaultButton = new QPushButton;
  defaultButton->setMinimumSize(100, 25);
  defaultButton->setMaximumSize(100, 25);
  defaultButton->setText("Default");
  defaultButton->setToolTip("Set parameters to default values");

  CloseButton = new QPushButton;
  CloseButton->setMinimumSize(100, 25);
  CloseButton->setMaximumSize(100, 25);
  CloseButton->setText("Close");
  CloseButton->setToolTip("Close this window");

  addTraceButton = new QPushButton;
  addTraceButton->setMinimumSize(100, 25);
  addTraceButton->setMaximumSize(100, 25);
  addTraceButton->setText("Add Trace");
  addTraceButton->setToolTip("Add this Z-ratio trace to the mainwindow");

  epochRadioButton = new QRadioButton;
  epochRadioButton->setMinimumSize(100, 25);
  epochRadioButton->setMaximumSize(100, 25);
  epochRadioButton->setText("Epochs");

  pageRadioButton = new QRadioButton;
  pageRadioButton->setMinimumSize(100, 25);
  pageRadioButton->setMaximumSize(100, 25);
  pageRadioButton->setText("Pages");

  wakesleepRadioButton = new QRadioButton;
  wakesleepRadioButton->setMinimumSize(100, 25);
  wakesleepRadioButton->setMaximumSize(100, 25);
  wakesleepRadioButton->setText("Sleep/Wake");

  pageRadioButton->setChecked(true);

  plot_type = 1;

  hlayout1 = new QHBoxLayout;
  hlayout1->setSpacing(20);
  hlayout1->addWidget(crossoverbinLabel);
  hlayout1->addWidget(crossoverSpinbox);
  hlayout1->addStretch(100);
  hlayout1->addWidget(epochRadioButton);
  hlayout1->addStretch(100);
  hlayout1->addWidget(StartButton);

  hlayout2 = new QHBoxLayout;
  hlayout2->setSpacing(20);
  hlayout2->addWidget(thresholdLabel);
  hlayout2->addWidget(thresholdSpinbox);
  hlayout2->addStretch(100);
  hlayout2->addWidget(pageRadioButton);
  hlayout2->addStretch(100);
  hlayout2->addWidget(jumpButton);

  hlayout3 = new QHBoxLayout;
  hlayout3->setSpacing(20);
  hlayout3->addWidget(pagelenLabel);
  hlayout3->addWidget(pagelenSpinbox);
  hlayout3->addStretch(100);
  hlayout3->addWidget(wakesleepRadioButton);
  hlayout3->addStretch(100);
  hlayout3->addWidget(get_annotButton);

//   hlayout4 = new QHBoxLayout;
//   hlayout4->setSpacing(20);
//   hlayout4->addWidget(errordetectionLabel);
//   hlayout4->addWidget(errordetectionSpinbox);
//   hlayout4->addStretch(100);

  hlayout4 = new QHBoxLayout;
  hlayout4->setSpacing(20);
  hlayout4->addStretch(100);
  hlayout4->addWidget(addTraceButton);

  hlayout5 = new QHBoxLayout;
  hlayout5->setSpacing(20);
  hlayout5->addWidget(hysteresisLabel);
  hlayout5->addWidget(hysteresisSpinbox);
  hlayout5->addStretch(100);
  hlayout5->addWidget(defaultButton);
  hlayout5->addStretch(100);
  hlayout5->addWidget(CloseButton);

  vlayout2 = new QVBoxLayout;
  vlayout2->setSpacing(20);
  vlayout2->addLayout(hlayout1);
  vlayout2->addLayout(hlayout2);
  vlayout2->addLayout(hlayout3);
  vlayout2->addLayout(hlayout4);
  vlayout2->addLayout(hlayout5);

  vlayout3 = new QVBoxLayout;
  vlayout3->setSpacing(20);
  vlayout3->setContentsMargins(20, 20, 20, 20);
  vlayout3->addWidget(curve1);
  vlayout3->addLayout(vlayout2);

  zscore_dialog->setLayout(vlayout3);

  shift_page_left_Act = new QAction(this);
  shift_page_left_Act->setShortcut(QKeySequence::MoveToPreviousChar);
  connect(shift_page_left_Act, SIGNAL(triggered()), this, SLOT(shift_page_left()));
  zscore_dialog->addAction(shift_page_left_Act);

  shift_page_right_Act = new QAction(this);
  shift_page_right_Act->setShortcut(QKeySequence::MoveToNextChar);
  connect(shift_page_right_Act, SIGNAL(triggered()), this, SLOT(shift_page_right()));
  zscore_dialog->addAction(shift_page_right_Act);

  QObject::connect(CloseButton,          SIGNAL(clicked()),            zscore_dialog, SLOT(close()));
  QObject::connect(StartButton,          SIGNAL(clicked()),            this,          SLOT(startButtonClicked()));
  QObject::connect(get_annotButton,      SIGNAL(clicked()),            this,          SLOT(get_annotationsButtonClicked()));
  QObject::connect(jumpButton,           SIGNAL(clicked()),            this,          SLOT(jumpButtonClicked()));
  QObject::connect(zscore_dialog,        SIGNAL(destroyed(QObject *)), this,          SLOT(ZscoreDialogDestroyed(QObject *)));
  QObject::connect(epochRadioButton,     SIGNAL(clicked(bool)),        this,          SLOT(RadioButtonsClicked(bool)));
  QObject::connect(pageRadioButton,      SIGNAL(clicked(bool)),        this,          SLOT(RadioButtonsClicked(bool)));
  QObject::connect(wakesleepRadioButton, SIGNAL(clicked(bool)),        this,          SLOT(RadioButtonsClicked(bool)));
  QObject::connect(defaultButton,        SIGNAL(clicked()),            this,          SLOT(defaultButtonClicked()));
  QObject::connect(curve1,               SIGNAL(markerHasMoved()),     this,          SLOT(markersMoved()));
  QObject::connect(addTraceButton,       SIGNAL(clicked()),            this,          SLOT(addTraceButtonClicked()));

  zscore_dialog->show();
}


UI_ZScoreWindow::~UI_ZScoreWindow()
{
  if(!zscore_dialog_is_destroyed)
  {
    zscore_dialog->close();
  }

  free(zscore_epoch_buf);

  free(zscore_page_buf);

  free(zscore_sleepstage_buf);

  zscoredialog[zscore_dialognumber] = NULL;
}


void UI_ZScoreWindow::ZscoreDialogDestroyed(QObject *)
{
  zscore_dialog_is_destroyed = 1;

  zscoredialog[zscore_dialognumber] = NULL;

  delete this;
}


void UI_ZScoreWindow::addTraceButtonClicked()
{
  int i;

  struct edfhdrblock *hdr;

  struct signalcompblock *signalcomp,
                         *original_signalcomp;

  original_signalcomp = mainwindow->signalcomp[signalnr];

  hdr = original_signalcomp->edfhdr;

  if(original_signalcomp->ecg_filter != NULL)
  {
    return;
  }

  if(original_signalcomp->zratio_filter != NULL)
  {
    return;
  }

  signalcomp = mainwindow->create_signalcomp_copy(original_signalcomp);
  if(signalcomp == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: can not create a signalcomp copy.");
    messagewindow.exec();
    return;
  }

  signalcomp->zratio_filter = create_zratio_filter(hdr->edfparam[signalcomp->edfsignal[0]].smp_per_record,
                                                   hdr->long_data_record_duration,
                                                   crossoverSpinbox->value(),
    signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue);

  if(signalcomp->zratio_filter == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not create a Z-ratio-filter, malloc() error?");
    messagewindow.exec();
    free(signalcomp);
    return;
  }

  strcpy(signalcomp->signallabel_bu, signalcomp->signallabel);
  signalcomp->signallabellen_bu = signalcomp->signallabellen;
  strcpy(signalcomp->signallabel, "Z-ratio ");
  strcat(signalcomp->signallabel, signalcomp->signallabel_bu);
  signalcomp->signallabellen = strlen(signalcomp->signallabel);
  strcpy(signalcomp->physdimension_bu, signalcomp->physdimension);
  strcpy(signalcomp->physdimension, "");
  signalcomp->polarity = -1;

  if(dblcmp(signalcomp->edfhdr->edfparam[signalcomp->edfsignal[0]].bitvalue, 0.0) < 0)
  {
    for(i=0; i<signalcomp->num_of_signals; i++)
    {
      signalcomp->sensitivity[i] = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[i]].bitvalue / -1.0 / mainwindow->pixelsizefactor;
    }

    signalcomp->voltpercm = -1.0;
  }
  else
  {
    for(i=0; i<signalcomp->num_of_signals; i++)
    {
      signalcomp->sensitivity[i] = signalcomp->edfhdr->edfparam[signalcomp->edfsignal[i]].bitvalue / 1.0 / mainwindow->pixelsizefactor;
    }

    signalcomp->voltpercm = 1.0;
  }

  signalcomp->zratio_crossoverfreq = crossoverSpinbox->value();

  signalcomp->screen_offset = 0.0;

  mainwindow->setup_viewbuf();
}


void UI_ZScoreWindow::defaultButtonClicked()
{
  pagelenSpinbox->setValue(30);

//  errordetectionSpinbox->setValue(80);

  thresholdSpinbox->setValue(0.0);

  crossoverSpinbox->setValue(7.5);

  hysteresisSpinbox->setValue(0.0);
}


void UI_ZScoreWindow::startButtonClicked()
{
  int i,
      datrec_offset,
      total_datrecs,
      smpls,
      smp_per_record,
      smpls_in_epoch,
      dftblocksize,
      fft_outputbufsize,
      smpls_in_inputbuf,
      smpls_copied,
      epochs,
      f0,
      f1,
      f2,
      f3,
      f4,
      power_neg_cnt,
      power_pos_cnt,
      progress_steps;

  double *buf=NULL,
         *fft_inputbuf=NULL,
         *fft_outputbuf=NULL,
         samplefreq,
         freqstep,
         power_delta,
         power_theta,
         power_alpha,
         power_beta,
         power_total,
         power_neg,
         power_pos;

  struct edfhdrblock *hdr=NULL;

  struct signalcompblock *signalcomp=NULL;


  crossoverfreq = crossoverSpinbox->value();
  mainwindow->z_score_var.crossoverfreq = crossoverfreq;

  z_threshold = thresholdSpinbox->value();
  mainwindow->z_score_var.z_threshold = z_threshold;

  zscore_page_len = pagelenSpinbox->value() / 2;
  mainwindow->z_score_var.zscore_page_len = zscore_page_len * 2;

//   zscore_error_detection = errordetectionSpinbox->value();
//   mainwindow->z_score_var.zscore_error_detection = zscore_error_detection;

  z_hysteresis = hysteresisSpinbox->value();
  mainwindow->z_score_var.z_hysteresis = z_hysteresis;
  z_hysteresis /= 2.0;

  signalcomp = mainwindow->signalcomp[signalnr];

  hdr = signalcomp->edfhdr;

  total_datrecs = hdr->datarecords;

  smp_per_record = hdr->edfparam[signalcomp->edfsignal[0]].smp_per_record;

  dftblocksize = ((long long)smp_per_record * (ZSCORE_EPOCH_LEN * TIME_DIMENSION)) / hdr->long_data_record_duration;

  if(dftblocksize < (ZSCORE_EPOCH_LEN * 100))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplerate of selected signal is too low");
    messagewindow.exec();
    curve1->clear();
    return;
  }

  smpls_in_epoch = dftblocksize;

  epochs = (hdr->datarecords * (long long)smp_per_record) / (long long)smpls_in_epoch;

  samplefreq = (double)smp_per_record / ((double)hdr->long_data_record_duration / TIME_DIMENSION);
  if(dblcmp(samplefreq, 100.0) < 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Samplefrequency of the selected signal must be at least 100 Hz.");
    messagewindow.exec();
    curve1->clear();
    return;
  }

  fft_outputbufsize = dftblocksize / 2;

  freqstep = samplefreq / (double)dftblocksize;
  if(dblcmp(freqstep, 1.0) > 0)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Frequency bin of FFT is more than 1 Hz.");
    messagewindow.exec();
    curve1->clear();
    return;
  }

  f2 = crossoverfreq / freqstep;

  f0 = ZSCORE_F0 / freqstep;
  if(f0 < 1)
  {
    f0 = 1;
  }
  f1 = ZSCORE_F1 / freqstep;
  f3 = ZSCORE_F3 / freqstep;
  f4 = ZSCORE_F4 / freqstep;

  fft_inputbuf = (double *)malloc(sizeof(double) * dftblocksize);
  if(fft_inputbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    return;
  }

  fft_outputbuf = (double *)calloc(1, sizeof(double) * fft_outputbufsize);
  if(fft_outputbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    fft_inputbuf = NULL;
    return;
  }

  if(zscore_epoch_buf != NULL)
  {
    free(zscore_epoch_buf);
  }

  zscore_epoch_buf = (double *)calloc(1, sizeof(double) * epochs);
  if(zscore_epoch_buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    free(fft_outputbuf);
    fft_inputbuf = NULL;
    fft_outputbuf = NULL;
    return;
  }

  if(zscore_page_buf != NULL)
  {
    free(zscore_page_buf);
  }

  zscore_page_buf = (double *)calloc(1, sizeof(double) * ((epochs / zscore_page_len) + 1));
  if(zscore_page_buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    free(fft_outputbuf);
    fft_inputbuf = NULL;
    fft_outputbuf = NULL;
    return;
  }

  if(zscore_sleepstage_buf != NULL)
  {
    free(zscore_sleepstage_buf);
  }

  zscore_sleepstage_buf = (int *)calloc(1, sizeof(int) * ((epochs / zscore_page_len) + 1));
  if(zscore_sleepstage_buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    free(fft_outputbuf);
    fft_inputbuf = NULL;
    fft_outputbuf = NULL;
    return;
  }

// printf("epochs / zscore_page_len is %i\n",
//        epochs / zscore_page_len);

  kiss_fftr_cfg cfg;

  kiss_fft_cpx *kiss_fftbuf;

  kiss_fftbuf = (kiss_fft_cpx *)malloc((fft_outputbufsize + 1) * sizeof(kiss_fft_cpx));
  if(kiss_fftbuf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "The system was not able to provide enough resources (memory) to perform the requested action.");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    free(fft_outputbuf);
    fft_inputbuf = NULL;
    fft_outputbuf = NULL;
    return;
  }

  cfg = kiss_fftr_alloc(dftblocksize, 0, NULL, NULL);

  FilteredBlockReadClass blockread;

  buf = blockread.init_signalcomp(signalcomp, 1, 0);
  if(buf == NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Internal error: init_signalcomp()");
    messagewindow.exec();
    curve1->clear();
    free(fft_inputbuf);
    free(fft_outputbuf);
    fft_inputbuf = NULL;
    fft_outputbuf = NULL;
    free(cfg);
    free(kiss_fftbuf);
    return;
  }

  QProgressDialog progress("Processing ...", "Abort", 0, (int)hdr->datarecords, zscore_dialog);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(200);

  progress_steps = total_datrecs / 100;
  if(progress_steps < 1)
  {
    progress_steps = 1;
  }

  if(progress_steps > 50)
  {
    progress_steps = 50;
  }

// printf("total_datrecs is %i\n"
//        "smp_per_record is %i\n"
//        "dftblocksize is %i\n"
//        "smpls_in_epoch is %i\n"
//        "epochs is %i\n"
//        "samplefreq is %f\n"
//        "fft_outputbufsize is %i\n"
//        "freqstep is %f\n"
//        "f0 is %i\n"
//        "f1 is %i\n"
//        "f2 is %i\n"
//        "f3 is %i\n"
//        "f4 is %i\n"
//        "z_hysteresis is %f\n",
//        total_datrecs,
//        smp_per_record,
//        dftblocksize,
//        smpls_in_epoch,
//        epochs,
//        samplefreq,
//        fft_outputbufsize,
//        freqstep,
//        f0,
//        f1,
//        f2,
//        f3,
//        f4,
//        z_hysteresis);

  epoch_cntr = 0;

  smpls_in_inputbuf = 0;

// FILE *test_file1 = fopen("testfile1.dat", "wb");
// if(test_file1 == NULL)
// {
//   printf("error opening file\n");
//   return;
// }
//
// FILE *test_file2 = fopen("testfile2.dat", "wb");
// if(test_file2 == NULL)
// {
//   printf("error opening file\n");
//   return;
// }




  for(datrec_offset=0; datrec_offset < total_datrecs; datrec_offset++)
//  for(datrec_offset=0; datrec_offset < 2; datrec_offset++)
  {

// printf("\ndatrec_offset is %i\n", datrec_offset);

// printf("smpls_in_inputbuf is %i\n", smpls_in_inputbuf);

    if(!(datrec_offset % progress_steps))
    {
      progress.setValue(datrec_offset);

      qApp->processEvents();

      if(progress.wasCanceled() == true)
      {
        break;
      }
    }

    if(blockread.process_signalcomp(datrec_offset))
    {
      progress.reset();
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "A read-error occurred.");
      messagewindow.exec();
      curve1->clear();
      free(fft_inputbuf);
      free(fft_outputbuf);
      fft_inputbuf = NULL;
      fft_outputbuf = NULL;
      free(cfg);
      free(kiss_fftbuf);
      return;
    }

    smpls = blockread.samples_in_buf();

// fwrite(buf, smpls * sizeof(double), 1, test_file1);

// printf("smpls is %i\n", smpls);

// for(k=0; k<smpls; k++)
// {
//   printf("%4.4f ", buf[k]);
//   if((!(k%4)) && k)
//   {
//     putchar('\n');
//   }
// }
// putchar('\n');

    smpls_copied = 0;

    while(smpls > 0)
    {
// printf("while(smpls > 0)\n");

      if(smpls < (dftblocksize - smpls_in_inputbuf))
      {
        memcpy(fft_inputbuf + smpls_in_inputbuf, buf, smpls * sizeof(double));

//         for(j=0; j<smpls; j++)
//         {
//           fft_inputbuf[j + smpls_in_inputbuf] = buf[j];
//         }

// printf("copy:\n");
// for(k=0; k<smpls; k++)
// {
//   printf("%4.4f ", fft_inputbuf[k + smpls_in_inputbuf]);
//   if((!(k%4)) && k)
//   {
//     putchar('\n');
//   }
// }
// putchar('\n');

        smpls_in_inputbuf += smpls;

        smpls = 0;

// printf("break\n");

        break;
      }

      memcpy(fft_inputbuf + smpls_in_inputbuf, buf + smpls_copied, (dftblocksize - smpls_in_inputbuf) * sizeof(double));

//       for(j=0; j<(dftblocksize - smpls_in_inputbuf); j++)
//       {
//         fft_inputbuf[j + smpls_in_inputbuf] = buf[j + smpls_copied];
//       }

// printf("smpls_copied is %i\n", smpls_copied);

      smpls_copied += (dftblocksize - smpls_in_inputbuf);

// printf("smpls_copied is %i\n", smpls_copied);

      smpls -= (dftblocksize - smpls_in_inputbuf);

// printf("smpls is %i\n", smpls);

      smpls_in_inputbuf = 0;

// fwrite(fft_inputbuf, dftblocksize * sizeof(double), 1, test_file2);

      kiss_fftr(cfg, fft_inputbuf, kiss_fftbuf);

      power_delta = 0.0;
      power_theta = 0.0;
      power_alpha = 0.0;
      power_beta = 0.0;
      power_total = 0.0;

      for(i=0; i<fft_outputbufsize; i++)
      {
        fft_outputbuf[i] = (((kiss_fftbuf[i].r * kiss_fftbuf[i].r) + (kiss_fftbuf[i].i * kiss_fftbuf[i].i)) / fft_outputbufsize);

//         if(epoch_cntr == 0)
//         {
//           printf("fft_outputbuf[%i] is %.14f\n", i, fft_outputbuf[i]);
//         }

        if((i > f0) && (i < f1))
        {
          power_delta += fft_outputbuf[i];
        }

        if((i >= f1) && (i < f2))
        {
          power_theta += fft_outputbuf[i];
        }

        if((i >= f2) && (i < f3))
        {
          power_alpha += fft_outputbuf[i];
        }

        if((i >= f3) && (i <= f4))
        {
          power_beta += fft_outputbuf[i];
        }
      }

      power_total = power_delta + power_theta + power_alpha + power_beta;

      if(dblcmp(power_total, 0.0) > 0)
      {
        zscore_epoch_buf[epoch_cntr++] = ((power_delta + power_theta) - (power_alpha + power_beta)) / power_total;
      }
      else
      {
        zscore_epoch_buf[epoch_cntr++] = 0.0;
      }

//       if(epoch_cntr < 3)
//       {
//         printf("zscore_epoch_buf[%i] is %f\n", epoch_cntr - 1, zscore_epoch_buf[epoch_cntr - 1]);
//         printf("power_delta is %f\n"
//                "power_theta is %f\n"
//                "power_alpha is %f\n"
//                "power_beta is %f\n"
//                "power_total is %f\n\n",
//                power_delta, power_theta, power_alpha, power_beta, power_total);
//       }

      if(epoch_cntr >= epochs)
      {
        break;
      }
    }

    if(epoch_cntr >= epochs)
    {
      break;
    }
  }

  power_pos = 0.0;
  power_neg = 0.0;
  power_pos_cnt = 0;
  power_neg_cnt = 0;

  zscore_pages = 0;

  for(i=0; i<epoch_cntr; i++)
  {
    if(i && (!(i % zscore_page_len)))
    {
      if(power_neg_cnt)
      {
        power_neg /= power_neg_cnt;
      }

      if(power_pos_cnt)
      {
        power_pos /= power_pos_cnt;
      }

      if(power_neg_cnt >= power_pos_cnt)
      {
        zscore_page_buf[zscore_pages] = power_neg;

        zscore_sleepstage_buf[zscore_pages] = 0;
      }
      else
      {
        zscore_page_buf[zscore_pages] = power_pos;

        zscore_sleepstage_buf[zscore_pages] = 1;
      }

      zscore_pages++;

      power_pos = 0.0;
      power_neg = 0.0;
      power_pos_cnt = 0;
      power_neg_cnt = 0;
    }

    if(zscore_epoch_buf[i] < ZSCORE_ERRORLEVEL)
    {
      if(zscore_pages > 0)
      {
        if(zscore_sleepstage_buf[zscore_pages - 1] == 0)
        {

  // printf("if(zscore_sleepstage_buf[zscore_pages - 1] == 0) : %f  <  %f  +  %f\n",
  //        zscore_epoch_buf[i], z_threshold, z_hysteresis);

          if(zscore_epoch_buf[i] < (z_threshold + z_hysteresis))
          {
            power_neg += zscore_epoch_buf[i];

            power_neg_cnt++;
          }
          else
          {
            power_pos += zscore_epoch_buf[i];

            power_pos_cnt++;
          }
        }
        else
        {

  // printf("else : %f  <  %f  +  %f\n",
  //        zscore_epoch_buf[i], z_threshold, z_hysteresis);

          if(zscore_epoch_buf[i] < (z_threshold - z_hysteresis))
          {
            power_neg += zscore_epoch_buf[i];

            power_neg_cnt++;
          }
          else
          {
            power_pos += zscore_epoch_buf[i];

            power_pos_cnt++;
          }
        }
      }
      else
      {
        if(zscore_epoch_buf[i] < z_threshold)
        {
          power_neg += zscore_epoch_buf[i];

          power_neg_cnt++;
        }
        else
        {
          power_pos += zscore_epoch_buf[i];

          power_pos_cnt++;
        }
      }
    }
  }


// printf("epoch_cntr is %i     zscore_pages is %i\n",
//        epoch_cntr, zscore_pages);

//  Delta: 0.5 - 2.0 Hz
//  Theta: 2.5 - 7.0 Hz
//  Alpha: 7.5 - 11.5 Hz
//  Beta: 12.0 - 25.0 Hz

//  Stage    Z ratio (mean +-SD)
//  ----------------------------
//  Wake      -0.181 +- .055
//  REM        0.156 +- .043
//  1          0.066 +- .101
//  2          0.207 +- .067
//  3          0.443 +- .034
//  4          0.527 +- .039

//  ZRatio = ((Delta + Theta) - (Alpha + Beta)) / (Delta + Theta + Alpha + Beta)

  progress.reset();

  RadioButtonsClicked(true);

  free(fft_inputbuf);
  free(fft_outputbuf);
  free(cfg);
  free(kiss_fftbuf);

  mainwindow->setup_viewbuf();

// printf("epoch_cntr is %i    epochs is %i\n", epoch_cntr, epochs);

// fclose(test_file1);
// fclose(test_file2);

}


void UI_ZScoreWindow::get_annotationsButtonClicked()
{
  int i,
      awake = 1,
      filenum,
      marker_start,
      marker_end;

  struct annotationblock annotation;

  if((epoch_cntr < 2) || (zscore_pages < 1))
  {
    return;
  }

  marker_start = curve1->getMarker1Position() * zscore_pages;
  if(marker_start < 0)
  {
    marker_start = 0;
  }

  marker_end = curve1->getMarker2Position() * zscore_pages;
  if(marker_end > zscore_pages)
  {
    marker_end = zscore_pages;
  }

  filenum = mainwindow->signalcomp[signalnr]->edfhdr->file_num;

  for(i=0; i<marker_end; i++)
  {
    if(zscore_sleepstage_buf[i] != awake)
    {
      awake = zscore_sleepstage_buf[i];

      if(i >= marker_start)
      {
        memset(&annotation, 0, sizeof(struct annotationblock));

        annotation.onset = i * zscore_page_len * 2LL * TIME_DIMENSION;
        if(awake)
        {
          strcpy(annotation.annotation, "Wake");
        }
        else
        {
          strcpy(annotation.annotation, "Sleep");
        }
        edfplus_annotation_add_item(&mainwindow->edfheaderlist[filenum]->annot_list, annotation);
      }
    }
  }

  if(mainwindow->annotations_dock[filenum] == NULL)
  {
    mainwindow->annotations_dock[filenum] = new UI_Annotationswindow(filenum, mainwindow);

    mainwindow->addDockWidget(Qt::RightDockWidgetArea, mainwindow->annotations_dock[filenum]->docklist, Qt::Vertical);

    if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) < 1)
    {
      mainwindow->annotations_dock[filenum]->docklist->hide();
    }
  }

  if(edfplus_annotation_size(&mainwindow->edfheaderlist[filenum]->annot_list) > 0)
  {
    mainwindow->annotations_dock[filenum]->docklist->show();

    mainwindow->annotations_edited = 1;

    mainwindow->annotations_dock[filenum]->updateList();

    mainwindow->save_act->setEnabled(true);
  }

   mainwindow->maincurve->update();
}


void UI_ZScoreWindow::RadioButtonsClicked(bool)
{
  int i,
      marker_start,
      marker_end,
      regression_samples,
      sleep_cnt,
      sleep_pct;

  double b0,
         b1,
         sum_xy=0.0,
         sum_x2=0.0,
         avg_x=0.0,
         avg_y=0.0,
         shiftfactor=2.0;

  char str[1024];


  if(epochRadioButton->isChecked() == true)
  {
    plot_type = 0;

    if((epoch_cntr < 2) || (zscore_pages < 1))
    {
      return;
    }

    marker_start = curve1->getMarker1Position() * epoch_cntr;
    if(marker_start < 0)
    {
      marker_start = 0;
    }

    marker_end = curve1->getMarker2Position() * epoch_cntr;
    if(marker_end > epoch_cntr)
    {
      marker_end = epoch_cntr;
    }

    regression_samples = marker_end - marker_start;
    if(regression_samples < 60)
    {
      return;
    }

    for(i=0; i<regression_samples; i++)  // calculate regression
    {
      sum_xy += ((i + 1) * (zscore_epoch_buf[i + marker_start] + shiftfactor));

      sum_x2 += ((i + 1) * (i + 1));

      avg_x += (i + 1);

      avg_y += (zscore_epoch_buf[i + marker_start] + shiftfactor);
    }

    avg_x /= regression_samples;

    avg_y /= regression_samples;

//    printf("epoch_cntr is %i   sum_xy is %f   sum_x2 is %f   avg_x is %f    avg_y is %f\n",
//           epoch_cntr, sum_xy, sum_x2, avg_x, avg_y);

    b1 = (sum_xy - (regression_samples * avg_x * avg_y)) / (sum_x2 - (regression_samples * avg_x * avg_x));  // x-coefficient

    b0 = (avg_y - (b1 * avg_x)) - shiftfactor;  // intercept (or constant)

//    printf("b0 is %f    b1 is %f\n", b0, b1);

    sprintf(str, "%s   coefficient: %f   intercept: %f", mainwindow->signalcomp[signalnr]->signallabel, b1, b0);

    curve1->drawCurve(zscore_epoch_buf, epoch_cntr, 1.0, -1.0);
    curve1->setH_RulerValues(0, epoch_cntr);
    curve1->setLowerLabel("Epochs");
    curve1->setV_label("Z ratio");
    curve1->setUpperLabel1(str);
    curve1->drawLine(marker_start, b0 + b1, marker_end, b0 + (b1 * regression_samples), Qt::yellow);
    curve1->setMarker1Enabled(true);
    curve1->setMarker1MovableEnabled(true);
    curve1->setMarker2Enabled(true);
    curve1->setMarker2MovableEnabled(true);
  }

  if(pageRadioButton->isChecked() == true)
  {
    plot_type = 1;

    if((epoch_cntr < 2) || (zscore_pages < 1))
    {
      return;
    }

    marker_start = curve1->getMarker1Position() * zscore_pages;
    if(marker_start < 0)
    {
      marker_start = 0;
    }

    marker_end = curve1->getMarker2Position() * zscore_pages;
    if(marker_end > zscore_pages)
    {
      marker_end = zscore_pages;
    }

    regression_samples = marker_end - marker_start;
    if(regression_samples < 2)
    {
      return;
    }

    for(i=0; i<regression_samples; i++)  // calculate regression
    {
      sum_xy += ((i + 1) * (zscore_page_buf[i + marker_start] + shiftfactor));

      sum_x2 += ((i + 1) * (i + 1));

      avg_x += (i + 1);

      avg_y += (zscore_page_buf[i + marker_start] + shiftfactor);
    }

    avg_x /= regression_samples;

    avg_y /= regression_samples;

//     printf("sum_xy is %f   sum_x2 is %f   avg_x is %f    avg_y is %f    regression_samples is %i\n",
//            sum_xy, sum_x2, avg_x, avg_y, regression_samples);

    b1 = (sum_xy - (regression_samples * avg_x * avg_y)) / (sum_x2 - (regression_samples * avg_x * avg_x));  // x-coefficient

    b0 = (avg_y - (b1 * avg_x)) - shiftfactor;  // intercept (or constant)

//    printf("b0 is %f    b1 is %f\n", b0, b1);

    sprintf(str, "%s   coefficient: %f   intercept: %f", mainwindow->signalcomp[signalnr]->signallabel, b1, b0);

    curve1->drawCurve(zscore_page_buf, zscore_pages, 1.0, -1.0);
    curve1->setH_RulerValues(0, zscore_pages);
    curve1->setLowerLabel("Pages");
    curve1->setV_label("Z ratio");
    curve1->setUpperLabel1(str);
    curve1->drawLine(marker_start, b0 + b1, marker_end, b0 + (b1 * regression_samples), Qt::yellow);
    curve1->setMarker1Enabled(true);
    curve1->setMarker1MovableEnabled(true);
    curve1->setMarker2Enabled(true);
    curve1->setMarker2MovableEnabled(true);
  }

  if(wakesleepRadioButton->isChecked() == true)
  {
    plot_type = 2;

    if((epoch_cntr < 2) || (zscore_pages < 1))
    {
      return;
    }

    marker_start = curve1->getMarker1Position() * zscore_pages;
    if(marker_start < 0)
    {
      marker_start = 0;
    }

    marker_end = curve1->getMarker2Position() * zscore_pages;
    if(marker_end > zscore_pages)
    {
      marker_end = zscore_pages;
    }

    regression_samples = marker_end - marker_start;
    if(regression_samples < 2)
    {
      return;
    }

    sleep_cnt = 0;

    for(i=marker_start; i<marker_end; i++)
    {
      if(zscore_sleepstage_buf[i] == 1)
      {
        sleep_cnt++;
      }
    }

    sleep_pct = (sleep_cnt * 100) / regression_samples;

    sprintf(str, "%s    sleep: %i%%", mainwindow->signalcomp[signalnr]->signallabel, sleep_pct);

    curve1->drawCurve(zscore_sleepstage_buf, zscore_pages, 3.0, -3.0);
    curve1->setH_RulerValues(0, zscore_pages);
    curve1->setLowerLabel("Pages");
    curve1->setV_label("Sleep/Wake");
    curve1->setUpperLabel1(str);
    curve1->setLineEnabled(false);
    curve1->setMarker1Enabled(true);
    curve1->setMarker1MovableEnabled(true);
    curve1->setMarker2Enabled(true);
    curve1->setMarker2MovableEnabled(true);
  }
}


void UI_ZScoreWindow::markersMoved()
{
  RadioButtonsClicked(true);
}


void UI_ZScoreWindow::jumpButtonClicked()
{
  int i, file_num;

  long long onset, temp;

  double position;

  if((epoch_cntr < 2) || (zscore_pages < 1))
  {
    return;
  }

  if(curve1->isCursorActive() == false)
  {
    return;
  }

  position = curve1->getCursorPosition();

  if(plot_type)
  {
    onset = position * zscore_page_len * 2 * TIME_DIMENSION;
  }
  else
  {
    onset = position * ZSCORE_EPOCH_LEN * TIME_DIMENSION;
  }

  file_num = mainwindow->signalcomp[signalnr]->edfhdr->file_num;

  if(mainwindow->viewtime_sync==VIEWTIME_SYNCED_OFFSET)
  {
    for(i=0; i<mainwindow->files_open; i++)
    {
      mainwindow->edfheaderlist[i]->viewtime = onset;

      mainwindow->edfheaderlist[i]->viewtime -= (mainwindow->pagetime / 2);

      mainwindow->edfheaderlist[i]->viewtime -= mainwindow->edfheaderlist[file_num]->starttime_offset;
    }
  }

  if(mainwindow->viewtime_sync==VIEWTIME_UNSYNCED)
  {
    mainwindow->edfheaderlist[file_num]->viewtime = onset;

    mainwindow->edfheaderlist[file_num]->viewtime -= (mainwindow->pagetime / 2);

    mainwindow->edfheaderlist[file_num]->viewtime -= mainwindow->edfheaderlist[file_num]->starttime_offset;
  }

  if((mainwindow->viewtime_sync==VIEWTIME_SYNCED_ABSOLUT)||(mainwindow->viewtime_sync==VIEWTIME_USER_DEF_SYNCED))
  {
    temp = onset - mainwindow->edfheaderlist[file_num]->viewtime;

    temp -= mainwindow->edfheaderlist[file_num]->starttime_offset;

    temp -= (mainwindow->pagetime / 2);

    for(i=0; i<mainwindow->files_open; i++)
    {
      mainwindow->edfheaderlist[i]->viewtime += temp;
    }
  }

  mainwindow->setup_viewbuf();
}


void UI_ZScoreWindow::shift_page_right()
{
  if((epoch_cntr < 2) || (zscore_pages < 1))
  {
    return;
  }

  if(curve1->isCursorActive() == false)
  {
    return;
  }

  curve1->shiftCursorIndexRight(1);
}


void UI_ZScoreWindow::shift_page_left()
{
  if((epoch_cntr < 2) || (zscore_pages < 1))
  {
    return;
  }

  if(curve1->isCursorActive() == false)
  {
    return;
  }

  curve1->shiftCursorIndexLeft(1);
}





