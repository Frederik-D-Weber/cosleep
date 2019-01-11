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


#ifndef UI_VIEW_MAINFORM_H
#define UI_VIEW_MAINFORM_H

#include <QtGlobal>
#include <QApplication>
#include <QEventLoop>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QFont>
#include <QToolBar>
#include <QSlider>
#include <QStyle>
#include <QCloseEvent>
#include <QDialog>

#if QT_VERSION < 0x050000
  #include <QPlastiqueStyle>
  #include <QGtkStyle>
  #include <QWindowsStyle>
  #ifdef Q_OS_MAC
    #include <QMacStyle>
  #endif
#else
  #include <QStyleFactory>
#endif

#ifdef Q_OS_WIN32
  #include <windows.h>
  #include <io.h>
  #ifndef CSIDL_COMMON_APPDATA
    #define CSIDL_COMMON_APPDATA 0x0023 // All Users\Application Data
  #endif
  #ifndef CSIDL_APPDATA
    #define CSIDL_APPDATA 0x001a // <username>\Application Data
  #endif
  #ifndef CSIDL_PROGRAM_FILES
    #define CSIDL_PROGRAM_FILES 0x0026 // C:\Program Files
  #endif
  #if QT_VERSION >= 0x050000
    #define QT_WA(unicode, ansi) unicode
  #endif
#else
  #include <sys/types.h>
  #include <sys/stat.h>
#endif

#include <QFileDialog>
#include <QAction>
#include <QActionGroup>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QLibrary>
#include <QString>
#include <QLocale>
#include <QMessageBox>
#include <QProgressDialog>
#include <QList>
#include <QProcess>
#include <QProcessEnvironment>
#include <QVariant>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDir>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#include "global.h"
#include "viewcurve.h"
#include "check_edf_file.h"
#include "show_edf_hdr.h"
#include "signals_dialog.h"
#include "signal_chooser.h"
#include "ascii_export.h"
#include "edf_compat.h"
#include "filter.h"
#include "filter_dialog.h"
#include "jump_dialog.h"
#include "about_dialog.h"
#include "edf_annotations.h"
#include "annotations_dock.h"
#include "options_dialog.h"
#include "nk2edf.h"
#include "xml.h"
#include "save_montage_dialog.h"
#include "load_montage_dialog.h"
#include "view_montage_dialog.h"
#include "show_actual_montage_dialog.h"
#include "print_to_edf.h"
#include "pagetime_dialog.h"
#include "print_to_bdf.h"
#include "ascii2edf.h"
#include "fino2edf.h"
#include "nexfin2edf.h"
#include "edfplusd_cnv.h"
#include "emsa2edf.h"
#include "bdf2edf.h"
#include "edit_annotation_dock.h"
#include "popup_save_cancelwindow.h"
#include "save_annots.h"
#include "utils.h"
#include "spectrumanalyzer.h"
#include "bi9800.h"
#include "export_annotations.h"
#include "edit_predefined_mtg.h"
#include "spectrum_dock.h"
#include "signalcurve.h"
#include "edf_annot_list.h"
#include "reduce_signals.h"
#include "header_editor.h"
#include "biosemi2bdfplus.h"
#include "bdf_triggers.h"
#include "import_annotations.h"
#include "ravg_filter.h"
#include "wav2edf.h"
#include "fma_ecg2edf.h"
#include "mortara2edf.h"
#include "averager_curve_wnd.h"
#include "ecg_filter.h"
#include "ecg_export.h"
#include "z_score_dialog.h"
#include "z_ratio_filter.h"
#include "raw2edf.h"
#include "check_for_updates.h"
#include "manscan2edf.h"
#include "scp_ecg2edf.h"
#include "unisens2edf.h"
#include "date_time_stamp_parser.h"
#include "spike_filter.h"
#include "spike_filter_dialog.h"
#include "mit2edf.h"
#include "biox2edf.h"
#include "plif_ecg_subtract_filter.h"
#include "plif_ecg_subtract_filter_dialog.h"
#include "export_filtered_signals.h"

#include "third_party/fidlib/fidlib.h"




class ViewCurve;
class UI_Signalswindow;
class UI_Annotationswindow;
class UI_AnnotationEditwindow;
class UI_SpectrumDockWindow;
class UI_FreqSpectrumWindow;
class UI_AverageCurveWindow;
class UI_ZScoreWindow;


class UI_Mainwindow : public QMainWindow
{
  Q_OBJECT

public:
  UI_Mainwindow();
  ~UI_Mainwindow();

  int files_open,
      signalcomps,
      totalviewbufsize,
      sel_viewtime,
      viewtime_sync,
      print_to_edf_active,
      annot_editor_active,
      show_annot_markers,
      show_baselines,
      annotations_edited,
      annotations_onset_relative,
      annotations_show_duration,
      annotations_duration_background_type,
      exit_in_progress,
      maxdftblocksize,
      dpix,
      dpiy,
      auto_dpi,
      clip_to_pane,
      auto_reload_mtg,
      read_biosemi_status_signal,
      read_nk_trigger_signal,
      live_stream_active,
      playback_realtime_active,
      live_stream_update_interval,
      signal_averaging_active,
      powerlinefreq,
      mousewheelsens,
      average_ratio,
      average_upsidedown,
      average_bw,
      spectrum_bw,
      spectrum_sqrt,
      spectrum_vlog,
      spectrumdock_sqrt,
      spectrumdock_vlog,
      spectrumdock_colorbars,
      use_threads,
      check_for_updates,
      amplitude_doubler,
      timescale_doubler,
      viewtime_indicator_type,
      mainwindow_title_type,
      linear_interpol,
      average_period;

  long long pagetime,
            maxfilesize_to_readin_annotations;

  char *viewbuf,
       viewtime_string[128],
       pagetime_string[64],
       recent_montagedir[MAX_PATH_LENGTH],
       recent_savedir[MAX_PATH_LENGTH],
       recent_opendir[MAX_PATH_LENGTH],
       predefined_mtg_path[MAXPREDEFINEDMONTAGES][MAX_PATH_LENGTH],
       recent_file_mtg_path[MAX_RECENTFILES][MAX_PATH_LENGTH],
       recent_colordir[MAX_PATH_LENGTH],
       cfg_app_version[17];

  double pixelsizefactor,
         x_pixelsizefactor,
         default_amplitude;

  struct{
          double crossoverfreq;
          double z_threshold;
          double z_hysteresis;
          int zscore_page_len;
          int zscore_error_detection;
        } z_score_var;

  struct raw2edf_var_struct raw2edf_var;

  struct edfhdrblock *edfheaderlist[MAXFILES];

  struct signalcompblock *signalcomp[MAXSIGNALS];

  struct zoomhistoryblock *zoomhistory;

  struct annotation_list *annotationlist_backup;

  struct spectrum_markersblock *spectrum_colorbar;

  struct import_annotations_var_block *import_annotations_var;

  struct export_annotations_var_block *export_annotations_var;

  struct video_player_struct *video_player;

  struct annot_filter_struct *annot_filter;

  UI_Annotationswindow *annotations_dock[MAXFILES];

  UI_AnnotationEditwindow *annotationEditDock;

  UI_FreqSpectrumWindow *spectrumdialog[MAXSPECTRUMDIALOGS];

  UI_SpectrumDockWindow *spectrumdock[MAXSPECTRUMDOCKS];

  UI_AverageCurveWindow *averagecurvedialog[MAXAVERAGECURVEDIALOGS];

  UI_ZScoreWindow *zscoredialog[MAXZSCOREDIALOGS];

  void setup_viewbuf();

  void setMainwindowTitle(struct edfhdrblock *);

  int file_is_opened(const char *);

  void remove_recent_file_mtg_path(const char *);

  void get_rgbcolor_settings(struct xml_handle *, const char *, int, QColor *);

  struct signalcompblock * create_signalcomp_copy(struct signalcompblock *);

#ifdef Q_OS_WIN32
  QString specialFolder(int);
#endif

  ViewCurve    *maincurve;

  QFont        *myfont,
               *monofont;

  QAction      *save_act,
               *load_predefined_mtg_act[MAXPREDEFINEDMONTAGES],
               *Escape_act,
               *video_act;

  QActionGroup *timelock_act_group,
               *sel_viewtime_act_group;


private:

  QMenuBar     *menubar;

  QMenu        *filemenu,
               *signalmenu,
               *displaymenu,
               *amplitudemenu,
               *toolsmenu,
               *settingsmenu,
               *helpmenu,
               *printmenu,
               *filtermenu,
//               *math_func_menu,
               *timemenu,
               *windowmenu,
               *recent_filesmenu,
               *close_filemenu,
               *montagemenu,
               *patternmenu,
               *print_img_menu;

  char path[MAX_PATH_LENGTH],
       videopath[MAX_PATH_LENGTH],
       montagepath[MAX_PATH_LENGTH],
       recent_file_path[MAX_RECENTFILES][MAX_PATH_LENGTH],
       option_str[MAX_PATH_LENGTH];

  QAction  *former_page_Act,
           *shift_page_left_Act,
           *shift_page_right_Act,
           *playback_file_Act,
           *stop_playback_realtime_Act,
           *next_page_Act,
           *shift_page_up_Act,
           *shift_page_down_Act,
           *page_div2,
           *page_mult2,
           *page_10u,
           *page_20u,
           *page_50u,
           *page_100u,
           *page_200u,
           *page_500u,
           *page_1m,
           *page_2m,
           *page_5m,
           *page_10m,
           *page_20m,
           *page_50m,
           *page_100m,
           *page_200m,
           *page_500m,
           *page_1,
           *page_2,
           *page_5,
           *page_10,
           *page_15,
           *page_20,
           *page_30,
           *page_60,
           *page_300,
           *page_1200,
           *page_3600,
           *page_user_defined,
           *page_whole_rec,
           *fit_to_pane,
           *fit_to_dc,
           *amp_00001,
           *amp_00002,
           *amp_00005,
           *amp_0001,
           *amp_0002,
           *amp_0005,
           *amp_001,
           *amp_002,
           *amp_005,
           *amp_01,
           *amp_02,
           *amp_05,
           *amp_1,
           *amp_2,
           *amp_5,
           *amp_10,
           *amp_20,
           *amp_50,
           *amp_100,
           *amp_200,
           *amp_500,
           *amp_1000,
           *amp_2000,
           *amp_5000,
           *amp_10000,
           *amp_20000,
           *amp_50000,
           *amp_plus,
           *amp_minus,
           *zoomback_Act,
           *zoomforward_Act,
           *slower_Act,
           *faster_Act,
           *recent_file[MAX_RECENTFILES],
           *offset_timesync_act,
           *absolut_timesync_act,
           *no_timesync_act,
           *user_def_sync_act,
           *sel_viewtime_act[MAXFILES];

  QActionGroup *AmplitudeGroup,
               *DisplayGroup,
               *load_predefined_mtg_group;

  QTimer   *live_stream_timer,
           *video_poll_timer,
           *playback_realtime_timer;

  QTime    *playback_realtime_time;

  QSplashScreen *splash;

  QPixmap  *pixmap;

  QToolBar *slidertoolbar,
           *navtoolbar;

  QSlider  *positionslider;

  QProcess *video_process;

  QTcpSocket *vlc_sock;

  int cmdlineargument,
      cmdlineoption,
      oldwindowheight;

  Check_for_updates *update_checker;

  void read_color_settings();
  void read_recent_file_settings();
  void read_general_settings();
  void write_settings();
  long long check_edf_file_datarecords(struct edfhdrblock *);
  void mpr_write(const char *);
  int mpr_read(char *, int);

public slots:
  void remove_all_signals();
  void edfplus_remove_duplicate_annotations();
  void video_player_seek(int);
  void video_player_toggle_pause();

private slots:
  void open_new_file();
  void show_file_info();
  void close_file_action_func(QAction *);
  void close_all_files();
  void exit_program();
  void signalproperties_dialog();
  void filterproperties_dialog();
  void add_signals_dialog();
  void show_about_dialog();
  void set_display_time(QAction *);
  void set_page_div2();
  void set_page_mult2();
  void set_display_time_whole_rec();
  void set_amplitude(QAction *);
  void set_amplitude_div2();
  void set_amplitude_mult2();
  void fit_signals_to_pane();
  void fit_signals_dc_offset();
  void former_page();
  void shift_page_left();
  void shift_page_right();
  void playback_file();
  void stop_playback();
  void next_page();
  void shift_page_up();
  void shift_page_down();
  void zoomback();
  void forward();
  void show_splashscreen();
  void export_to_ascii();
  void check_edf_compatibility();
  void add_new_filter();
  void add_plif_ecg_filter();
  void add_spike_filter();
  void remove_all_filters();
  void remove_all_plif_ecg_filters();
  void remove_all_spike_filters();
//  void add_new_math_func();
//  void remove_all_math_funcs();
  void jump_to_dialog();
  void jump_to_start();
  void jump_to_end();
  void jump_to_time_millisec(long long);
  void show_annotations();
  void show_options_dialog();
  long long get_long_time(char *);
  void nk2edf_converter();
  void set_timesync(QAction *);
  void set_timesync_reference(QAction *);
  void recent_file_action_func(QAction *);
  void sync_by_crosshairs();
  void save_montage();
  void load_montage();
  void view_montage();
  void show_this_montage();
  void show_help();
  void show_kb_shortcuts();
  void print_to_edf();
  void set_user_defined_display_time();
  void print_to_bdf();
  void print_to_img_640x480();
  void print_to_img_800x600();
  void print_to_img_1024x768();
  void print_to_img_1280x1024();
  void print_to_img_1600x1200();
  void convert_ascii_to_edf();
  void convert_fino_to_edf();
  void convert_wave_to_edf();
  void convert_fm_audio_to_edf();
  void convert_mortara_to_edf();
  void convert_nexfin_to_edf();
  void edfd_converter();
  void slider_moved(int);
  void convert_emsa_to_edf();
  void bdf2edf_converter();
  void set_dc_offset_to_zero();
  void annotation_editor();
  void save_file();
  void unisens2edf_converter();
  void BI98002edf_converter();
  void export_annotations();
  void load_predefined_mtg(QAction *);
  void edit_predefined_montages();
  void show_spectrum_dock();
  void page_3cmsec();
  void page_25mmsec();
  void page_50mmsec();
  void reduce_signals();
  void edit_header();
  void biosemi2bdfplus_converter();
  void import_annotations();
  void open_stream();
  void start_stop_video();
  void stop_video_generic(int);
  void live_stream_timer_func();
  void video_poll_timer_func();
  void playback_realtime_timer_func();
  void organize_signals();
  void Escape_fun();
  void export_ecg_rr_interval_to_ascii();
  void convert_binary_to_edf();
  void convert_manscan_to_edf();
  void convert_scpecg_to_edf();
  void convert_mit_to_edf();
  void convert_biox_to_edf();
  void video_process_error(QProcess::ProcessError);
  void vlc_sock_error(QAbstractSocket::SocketError);
  void export_filtered_signals();
  void video_player_faster();
  void video_player_slower();
//  void search_pattern();

protected:
  void closeEvent(QCloseEvent *);

};


#endif



