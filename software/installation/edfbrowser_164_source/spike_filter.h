/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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





#ifndef spike_filter_INCLUDED
#define spike_filter_INCLUDED



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPIKEFILTER_SPIKE_NO      0
#define SPIKEFILTER_SPIKE_ONSET   1




struct spike_filter_settings{
    int sf;
    int holdoff;
    int holdoff_sav;
    int holdoff_set;
    int cutoff;
    int cutoff_sav;
    int spikewidth;
    int spikewidth_sav;
    double array[4096];
    double array_sav[4096];
    int idx;
    int idx_sav;
    int n_max;
    int bufsz;
    double velocity;
    int polarity;
    int polarity_sav;
    int flank_det;
    int flank_det_sav;
    int flank_det_set;
    int run_in;
    int run_in_sav;
    double base_smpl;
    double base_smpl_sav;
    int spike_pos;
    int spike_pos_sav;
    int *pd_sig;
  };



struct spike_filter_settings * create_spike_filter(int, double, int, int *);
double run_spike_filter(double, struct spike_filter_settings *);
void free_spike_filter(struct spike_filter_settings *);
void reset_spike_filter(struct spike_filter_settings *);
struct spike_filter_settings * create_spike_filter_copy(struct spike_filter_settings *);
void spike_filter_save_buf(struct spike_filter_settings *);
void spike_filter_restore_buf(struct spike_filter_settings *);


#endif


