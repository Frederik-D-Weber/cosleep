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


#include "fft_wrap.h"


static void hamming_window_func(const double *, double *, int);
static void blackman_window_func(const double *, double *, int);


struct fft_wrap_settings_struct * fft_wrap_create(double *buf, int buf_size, int dft_size, int window_type)
{
  struct fft_wrap_settings_struct *st;

  if(buf == NULL)  return NULL;
  if(buf_size < 2)  return NULL;
  if(dft_size < 2)  return NULL;
  if(dft_size & 1)  dft_size--;
  if((window_type < 0) || (window_type > 2))  return NULL;

  st = (struct fft_wrap_settings_struct *)calloc(1, sizeof(struct fft_wrap_settings_struct));
  if(st == NULL)  return NULL;
  st->sz_in = buf_size;
  st->dft_sz = dft_size;
  st->wndw_type = window_type;

  st->blocks = 1;

  if(st->dft_sz < st->sz_in)
  {
    st->blocks = st->sz_in / st->dft_sz;
  }
  else
  {
    st->dft_sz = st->sz_in;
  }

  if(st->dft_sz & 1)  st->dft_sz--;

  st->smpls_left = st->sz_in % st->dft_sz;
  if(st->smpls_left & 1)  st->smpls_left--;

  st->sz_out = st->dft_sz / 2;
  st->buf_in = buf;
  if(st->wndw_type)
  {
    st->buf_wndw = (double *)malloc(sizeof(double) * (st->dft_sz + 2));
    if(st->buf_wndw == NULL)
    {
      free(st);
      return NULL;
    }
  }
  st->buf_out = (double *)malloc(sizeof(double) * (st->sz_out + 2));
  if(st->buf_out == NULL)
  {
    free(st->buf_wndw);
    free(st);
    return NULL;
  }
  st->kiss_fftbuf = (kiss_fft_cpx *)malloc((st->sz_out + 1) * sizeof(kiss_fft_cpx));
  if(st->kiss_fftbuf == NULL)
  {
    free(st->buf_wndw);
    free(st->buf_out);
    free(st);
    return NULL;
  }
  st->cfg = kiss_fftr_alloc(st->dft_sz, 0, NULL, NULL);

  return st;
}


void fft_wrap_run(struct fft_wrap_settings_struct *st)
{
  int i, j;

  if(st == NULL)  return;
  if(st->sz_in < 2)  return;
  if(st->dft_sz < 2) return;
  if(st->sz_out < 1) return;
  if(st->buf_in == NULL)  return;
  if(st->buf_out == NULL)  return;
  if(st->kiss_fftbuf == NULL)  return;

  if(st->wndw_type)
  {
    if(st->buf_wndw == NULL)  return;

    if(st->wndw_type == 1)
    {
      hamming_window_func(st->buf_in, st->buf_wndw, st->dft_sz);
    }
    else if(st->wndw_type == 2)
      {
        blackman_window_func(st->buf_in, st->buf_wndw, st->dft_sz);
      }
      else
      {
        return;
      }

    kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
  }
  else
  {
    kiss_fftr(st->cfg, st->buf_in, st->kiss_fftbuf);
  }

  for(i=0; i<st->sz_out; i++)
  {
    st->buf_out[i] = ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
  }

  for(j=1; j<st->blocks; j++)
  {
    if(st->wndw_type == 1)
    {
      hamming_window_func(st->buf_in + (j * st->dft_sz), st->buf_wndw, st->dft_sz);
    }
    else if(st->wndw_type == 2)
      {
        blackman_window_func(st->buf_in + (j * st->dft_sz), st->buf_wndw, st->dft_sz);
      }

    if(st->wndw_type)
    {
      kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
    }
    else
    {
      kiss_fftr(st->cfg, st->buf_in + (j * st->dft_sz), st->kiss_fftbuf);
    }

    for(i=0; i<st->sz_out; i++)
    {
      st->buf_out[i] += ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;
    }
  }

  if(st->smpls_left)
  {
    if(st->wndw_type == 1)
    {
      hamming_window_func(st->buf_in + ((j-1) * st->dft_sz) + st->smpls_left, st->buf_wndw, st->dft_sz);
    }
    else if(st->wndw_type == 2)
      {
        blackman_window_func(st->buf_in + ((j-1) * st->dft_sz) + st->smpls_left, st->buf_wndw, st->dft_sz);
      }

    if(st->wndw_type)
    {
      kiss_fftr(st->cfg, st->buf_wndw, st->kiss_fftbuf);
    }
    else
    {
      kiss_fftr(st->cfg, st->buf_in + ((j-1) * st->dft_sz) + st->smpls_left, st->kiss_fftbuf);
    }

    for(i=0; i<st->sz_out; i++)
    {
      st->buf_out[i] += ((st->kiss_fftbuf[i].r * st->kiss_fftbuf[i].r) + (st->kiss_fftbuf[i].i * st->kiss_fftbuf[i].i)) / st->sz_out;

      st->buf_out[i] /= (st->blocks + 1);
    }
  }
  else
  {
    if(st->blocks > 1)
    {
      for(i=0; i<st->sz_out; i++)
      {
        st->buf_out[i] /= st->blocks;
      }
    }
  }
}


void free_fft_wrap(struct fft_wrap_settings_struct *st)
{
  if(st == NULL)  return;
  free(st->cfg);
  free(st->kiss_fftbuf);
  free(st->buf_out);
  free(st->buf_wndw);
  memset(st, 0, sizeof(struct fft_wrap_settings_struct));
  free(st);
}


static void hamming_window_func(const double *src, double *dest, int sz)
{
  int i;

  for(i=0; i<sz; i++)
  {
    dest[i] = (0.53836 - (0.46164 * cos((2.0 * M_PI * i) / (sz - 1)))) * src[i];
  }
}


static void blackman_window_func(const double *src, double *dest, int sz)
{
  int i;

  for(i=0; i<sz; i++)
  {
    dest[i] = (0.42 - (0.5 * cos((2.0 * M_PI * i) / (sz - 1))) + (0.08 * cos((4.0 * M_PI * i) / (sz - 1)))) * src[i];
  }
}


















