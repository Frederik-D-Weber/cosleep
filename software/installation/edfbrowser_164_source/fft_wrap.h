/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2018 Teunis van Beelen
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



#ifndef fft_wrap_INCLUDED
#define fft_wrap_INCLUDED



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "third_party/kiss_fft/kiss_fftr.h"

#ifdef __cplusplus
extern "C" {
#endif


struct fft_wrap_settings_struct{
  int sz_in;
  int dft_sz;
  int sz_out;
  int blocks;
  int smpls_left;
  int wndw_type;
  double *buf_in;
  double *buf_wndw;
  double *buf_out;
  kiss_fftr_cfg cfg;
  kiss_fft_cpx *kiss_fftbuf;
};


struct fft_wrap_settings_struct * fft_wrap_create(double *, int, int, int);
void fft_wrap_run(struct fft_wrap_settings_struct *);
void free_fft_wrap(struct fft_wrap_settings_struct *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif







