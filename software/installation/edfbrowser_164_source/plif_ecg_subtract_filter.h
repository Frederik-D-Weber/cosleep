/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017, 2018 Teunis van Beelen
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


#ifndef PLIF_ECG_subtract_filter_INCLUDED
#define PLIF_ECG_subtract_filter_INCLUDED


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define PLIF_NBUFS   40


#ifdef __cplusplus
extern "C" {
#endif


struct plif_subtract_filter_settings{
                       int sf;
                       int tpl;
                       double *ravg_buf;
                       double *ravg_noise_buf[PLIF_NBUFS];
                       double *input_buf[PLIF_NBUFS];
                       double *ref_buf;
                       double linear_threshold;
                       double linear_diff[PLIF_NBUFS];
                       int ravg_idx;
                       int buf_idx;
                      };



struct plif_subtract_filter_settings * plif_create_subtract_filter(int, int, double);
double plif_run_subtract_filter(double, struct plif_subtract_filter_settings *);
void plif_reset_subtract_filter(struct plif_subtract_filter_settings *, double);
void plif_free_subtract_filter(struct plif_subtract_filter_settings *);
void plif_subtract_filter_state_copy(struct plif_subtract_filter_settings *, struct plif_subtract_filter_settings *);
struct plif_subtract_filter_settings * plif_subtract_filter_create_copy(struct plif_subtract_filter_settings *);


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif















