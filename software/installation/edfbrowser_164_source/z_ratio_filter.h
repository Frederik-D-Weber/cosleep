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







#ifndef zratio_filter_INCLUDED
#define zratio_filter_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "utils.h"

#include "third_party/kiss_fft/kiss_fftr.h"





struct zratio_filter_settings{
  double crossoverfreq;
  int dftblocksize;
  double samplefreq;
  int fft_outputbufsize;
  double freqstep;
  int f0;
  int f1;
  int f2;
  int f3;
  int f4;
  double *fft_inputbuf;
  double *fft_outputbuf;
  kiss_fftr_cfg cfg;
  kiss_fft_cpx *kiss_fftbuf;
  int smpls_in_inputbuf;
  double zratio_value;
  double *fft_inputbuf_bu;
  int smpls_in_inputbuf_bu;
  double zratio_value_bu;
  double bitvalue;
};



struct zratio_filter_settings * create_zratio_filter(int, long long, double, double);
double run_zratio_filter(double, struct zratio_filter_settings *);
void free_zratio_filter(struct zratio_filter_settings *);
void reset_zratio_filter(struct zratio_filter_settings *);
void zratio_filter_save_buf(struct zratio_filter_settings *);
void zratio_filter_restore_buf(struct zratio_filter_settings *);
#endif





