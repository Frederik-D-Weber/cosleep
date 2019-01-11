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







#ifndef ecg_filter_INCLUDED
#define ecg_filter_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>





struct ecg_filter_settings{
                       int avgfilter_50_size;
                       double *avgfilter_50_buf;
                       int avgfilter_50_idx;
                       double avgfilter_50_mean;
                       double *avgfilter_50_buf_bu;
                       int avgfilter_50_idx_bu;
                       double avgfilter_50_mean_bu;

                       int avgfilter_35_size;
                       double *avgfilter_35_buf;
                       int avgfilter_35_idx;
                       double avgfilter_35_mean;
                       double *avgfilter_35_buf_bu;
                       int avgfilter_35_idx_bu;
                       double avgfilter_35_mean_bu;

                       int avgfilter_25_size;
                       double *avgfilter_25_buf;
                       int avgfilter_25_idx;
                       double avgfilter_25_mean;
                       double *avgfilter_25_buf_bu;
                       int avgfilter_25_idx_bu;
                       double avgfilter_25_mean_bu;

                       double *SV;
                       int SV_sz;
                       int SV_idx;
                       double SV_bu[3];
                       int SV_idx_bu;

                       int runin;
                       int runin_bu;
                       int M_startslope;
                       int M_endslope;
                       int smpl_n;
                       double M[5];
                       int M_idx;
                       double M_avg;
                       double drain;
                       double top;
                       double sf;
                       double *F;
                       int F_size;
                       int F_idx;
                       int F_wd;
                       double F_value;
                       double F_sf_factor;
                       int R[5];
                       int R_idx;
                       int R_avg;
                       double R_value;
                       int R_startslope;
                       int R_endslope;
                       double bpm;
                       double bitvalue;

                       int M_startslope_bu;
                       int M_endslope_bu;
                       int smpl_n_bu;
                       double M_bu[5];
                       int M_idx_bu;
                       double M_avg_bu;
                       double drain_bu;
                       double top_bu;
                       double *F_bu;
                       int F_idx_bu;
                       double F_value_bu;
                       int R_bu[5];
                       int R_idx_bu;
                       int R_avg_bu;
                       double R_value_bu;
                       int R_startslope_bu;
                       int R_endslope_bu;
                       double bpm_bu;
                       int bu_filled;

                       double *stat_buf;
                       long long *stat_smpl_buf;
                       int stat_buf_idx;
                       int sample_cntr;

                       double T_peak_avg;
                       double T_peak_avg_bu;
};



struct ecg_filter_settings * create_ecg_filter(double, double, int);
double run_ecg_filter(double, struct ecg_filter_settings *);
void free_ecg_filter(struct ecg_filter_settings *);
void ecg_filter_save_buf(struct ecg_filter_settings *);
void ecg_filter_restore_buf(struct ecg_filter_settings *);
void reset_ecg_filter(struct ecg_filter_settings *);
int ecg_filter_get_beat_cnt(struct ecg_filter_settings *);
long long * ecg_filter_get_onset_beatlist(struct ecg_filter_settings *);
double * ecg_filter_get_interval_beatlist(struct ecg_filter_settings *);
#endif





