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


#ifndef EDFBROWSER_GLOBAL_H
#define EDFBROWSER_GLOBAL_H

#include <stdio.h>

#if defined(__APPLE__) || defined(__MACH__) || defined(__APPLE_CC__)

#define fopeno fopen

#else

#define fseeko fseeko64
#define ftello ftello64
#define fopeno fopen64

#endif

#define PROGRAM_NAME "EDFbrowser"
#define PROGRAM_VERSION "1.64"
#define MINIMUM_QT4_VERSION 0x040701
#define MINIMUM_QT5_VERSION 0x050901
#define MAXFILES 32
#define MAXSIGNALS 512
#define MAXFILTERS 16
#define TIME_DIMENSION (10000000LL)
#define MAX_ANNOTATION_LEN 512
#define VIEWTIME_SYNCED_OFFSET 0
#define VIEWTIME_SYNCED_ABSOLUT 1
#define VIEWTIME_UNSYNCED 2
#define VIEWTIME_USER_DEF_SYNCED 3
#define MAX_PATH_LENGTH 1024
#define MAX_RECENTFILES 32
#define MAX_ACTIVE_ANNOT_MARKERS 64
#define MAXSPECTRUMDIALOGS 32
#define MAXSPECTRUMDOCKS 8
#define MAXPREDEFINEDMONTAGES 12
#define MAXAVERAGECURVEDIALOGS 32
#define MAXZSCOREDIALOGS 32
#define MAXZOOMHISTORY 16

#define ANNOT_ID_NK_TRIGGER   0
#define ANNOT_ID_BS_TRIGGER   1

#define VIDEO_STATUS_STOPPED      0
#define VIDEO_STATUS_STARTUP_1    1
#define VIDEO_STATUS_STARTUP_2    2
#define VIDEO_STATUS_STARTUP_3    3
#define VIDEO_STATUS_STARTUP_4    4
#define VIDEO_STATUS_STARTUP_5    5
#define VIDEO_STATUS_STARTUP_6    6
#define VIDEO_STATUS_PLAYING     16
#define VIDEO_STATUS_PAUSED      17
#define VIDEO_STATUS_ENDED       18

#include "filter.h"
#include "third_party/fidlib/fidlib.h"
#include "ravg_filter.h"
#include "spike_filter.h"
#include "ecg_filter.h"
#include "z_ratio_filter.h"
#include "edf_annot_list.h"
#include "plif_ecg_subtract_filter.h"


struct edfparamblock{
        char   label[17];
        char   transducer[81];
        char   physdimension[9];
        double phys_min;
        double phys_max;
        int    dig_min;
        int    dig_max;
        char   prefilter[81];
        int    smp_per_record;
        char   reserved[33];
        double offset;
        int    buf_offset;
        double bitvalue;
        int    annotation;
      };

struct edfhdrblock{
        FILE      *file_hdl;
        int       file_num;
        char      version[32];
        char      filename[MAX_PATH_LENGTH];
        char      patient[81];
        char      recording[81];
        char      plus_patientcode[81];
        char      plus_gender[16];
        char      plus_birthdate[16];
        char      plus_patient_name[81];
        char      plus_patient_additional[81];
        char      plus_startdate[16];
        char      plus_admincode[81];
        char      plus_technician[81];
        char      plus_equipment[81];
        char      plus_recording_additional[81];
        long long l_starttime;
        long long utc_starttime;
        char      reserved[45];
        int       hdrsize;
        int       edfsignals;
        long long datarecords;
        int       recordsize;
        int       annot_ch[256];
        int       nr_annot_chns;
        int       edf;
        int       edfplus;
        int       bdf;
        int       bdfplus;
        int       discontinuous;
        int       genuine_nk;
        int       nk_triggers_read;
        int       genuine_biosemi;
        double    data_record_duration;
        long long long_data_record_duration;
        long long viewtime;
        long long starttime_offset;
        long long prefiltertime;
        int       annots_not_read;
        int       recording_len_sec;
        struct edfparamblock *edfparam;
        struct annotation_list annot_list;
      };

struct signalcompblock{
        int filenum;
        struct edfhdrblock *edfhdr;
        int num_of_signals;
        int viewbufsize;
        int viewbufoffset;
        long long records_in_viewbuf;
        long long samples_in_viewbuf;
        long long samples_on_screen;
        long long sample_start;
        long long sample_stop;
        int timeoffset;
        int sample_timeoffset;
        int pixels_shift;
        double sample_timeoffset_part;
        double sample_pixel_ratio;
        int edfsignal[MAXSIGNALS];
        int factor[MAXSIGNALS];
        int polarity;
        double sensitivity[MAXSIGNALS];
        int oldvalue;
        long long oldsmplnr;
        long long file_duration;
        char signallabel[512];
        char alias[17];
        int signallabellen;
        char signallabel_bu[512];
        int signallabellen_bu;
        int hascursor1;
        int hascursor2;
        int hasoffsettracking;
        int hasgaintracking;
        int hasruler;
        double screen_offset;
        double voltpercm;
        char physdimension[9];
        char physdimension_bu[9];
        int color;
        int filter_cnt;
        int samples_in_prefilterbuf;
        long long prefilter_starttime;
        int prefilter_reset_sample;
        double filterpreset_a[MAXFILTERS];
        double filterpreset_b[MAXFILTERS];
        struct filter_settings *filter[MAXFILTERS];
        int max_dig_value;
        int min_dig_value;
        int fidfilter_cnt;
        int fidfilter_type[MAXFILTERS];
        double fidfilter_freq[MAXFILTERS];
        double fidfilter_freq2[MAXFILTERS];
        double fidfilter_ripple[MAXFILTERS];
        int fidfilter_order[MAXFILTERS];
        int fidfilter_model[MAXFILTERS];
        FidFilter *fidfilter[MAXFILTERS];
        FidRun *fid_run[MAXFILTERS];
        FidFunc *fidfuncp[MAXFILTERS];
        void *fidbuf[MAXFILTERS];
        void *fidbuf2[MAXFILTERS];
        int fidfilter_setup[MAXFILTERS];
        int stat_cnt;
        int stat_zero_crossing_cnt;
        double stat_sum;
        double stat_sum_sqr;
        double stat_sum_rectified;
        int ravg_filter_cnt;
        int ravg_filter_type[MAXFILTERS];
        int ravg_filter_size[MAXFILTERS];
        int ravg_filter_setup[MAXFILTERS];
        struct ravg_filter_settings *ravg_filter[MAXFILTERS];
        struct ecg_filter_settings *ecg_filter;
        double spike_filter_velocity;
        int spike_filter_holdoff;
        struct spike_filter_settings *spike_filter;
        int plif_ecg_subtract_filter_plf;
        struct plif_subtract_filter_settings *plif_ecg_filter;
        struct plif_subtract_filter_settings *plif_ecg_filter_sav;
        struct zratio_filter_settings *zratio_filter;
        double zratio_crossoverfreq;
        int spectr_dialog[MAXSPECTRUMDIALOGS];
        int avg_dialog[MAXAVERAGECURVEDIALOGS];
        int zscoredialog[MAXZSCOREDIALOGS];
      };

struct zoomhistoryblock{
        int pntr;
        int history_size_tail;
        int history_size_front;
        long long viewtime[MAXZOOMHISTORY][MAXFILES];
        long long pagetime[MAXZOOMHISTORY];
        double voltpercm[MAXZOOMHISTORY][MAXSIGNALS];
        double sensitivity[MAXZOOMHISTORY][MAXSIGNALS][MAXSIGNALS];
        double screen_offset[MAXZOOMHISTORY][MAXSIGNALS];
       };

struct active_markersblock{
        int file_num;
        struct annotationblock *list[MAX_ACTIVE_ANNOT_MARKERS];
        int count;
        int selected;
       };

struct graphicLineStruct{
        int x1;
        int y1;
        int x2;
        int y2;
        };

struct graphicBufStruct{
        struct graphicLineStruct graphicLine[MAXSIGNALS];
        };

struct crossHairStruct{
        int color;
        int file_num;
        int active;
        int moving;
        int position;
        int x_position;
        int y_position;
        int y_value;
        double value;
        long long time;
        long long time_relative;
       };

struct video_player_struct{
        int status;
        int cntdwn_timer;
        int poll_timer;
        long long utc_starttime;
        int starttime_diff;
        int stop_det_counter;
        int fpos;
        int speed;
       };

struct annot_filter_struct{
       int tmin;
       int tmax;
       int invert;
       int hide_other;
       int hide_in_list_only;
       };

#endif



