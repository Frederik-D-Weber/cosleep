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




#include "z_ratio_filter.h"


#define ZRATIO_EPOCH_LEN 2
#define ZRATIO_F0 0.5
#define ZRATIO_F1 3.0
#define ZRATIO_F3 12.0
#define ZRATIO_F4 25.0




struct zratio_filter_settings * create_zratio_filter(int smp_per_record, long long long_data_record_duration, double crossoverf, double bitval)
{
  struct zratio_filter_settings * settings;

  settings = (struct zratio_filter_settings *)calloc(1, sizeof(struct zratio_filter_settings));
  if(settings == NULL)
  {
    return NULL;
  }

  settings->crossoverfreq = crossoverf;

  settings->bitvalue = bitval;

  settings->dftblocksize = ((long long)smp_per_record * (ZRATIO_EPOCH_LEN * TIME_DIMENSION)) / long_data_record_duration;

  if(settings->dftblocksize < (ZRATIO_EPOCH_LEN * 100))
  {
    free(settings);
    return NULL;
  }

  settings->samplefreq = (double)smp_per_record / ((double)long_data_record_duration / TIME_DIMENSION);
  if(dblcmp(settings->samplefreq, 100.0) < 0)
  {
    free(settings);
    return NULL;
  }

  settings->fft_outputbufsize = settings->dftblocksize / 2;

  settings->freqstep = settings->samplefreq / (double)settings->dftblocksize;
  if(dblcmp(settings->freqstep, 1.0) > 0)
  {
    free(settings);
    return NULL;
  }

  settings->f2 = settings->crossoverfreq / settings->freqstep;

  settings->f0 = ZRATIO_F0 / settings->freqstep;
  if(settings->f0 < 1)
  {
    settings->f0 = 1;
  }
  settings->f1 = ZRATIO_F1 / settings->freqstep;
  settings->f3 = ZRATIO_F3 / settings->freqstep;
  settings->f4 = ZRATIO_F4 / settings->freqstep;

  settings->fft_inputbuf = (double *)malloc(sizeof(double) * settings->dftblocksize);
  if(settings->fft_inputbuf == NULL)
  {
    free(settings);
    return NULL;
  }

  settings->fft_inputbuf_bu = (double *)malloc(sizeof(double) * settings->dftblocksize);
  if(settings->fft_inputbuf_bu == NULL)
  {
    free(settings->fft_inputbuf);
    free(settings);
    return NULL;
  }

  settings->fft_outputbuf = (double *)calloc(1, sizeof(double) * settings->fft_outputbufsize);
  if(settings->fft_outputbuf == NULL)
  {
    free(settings->fft_inputbuf);
    free(settings->fft_inputbuf_bu);
    free(settings);
    return NULL;
  }

  settings->kiss_fftbuf = (kiss_fft_cpx *)malloc((settings->fft_outputbufsize + 1) * sizeof(kiss_fft_cpx));
  if(settings->kiss_fftbuf == NULL)
  {
    free(settings->fft_inputbuf);
    free(settings->fft_inputbuf_bu);
    free(settings->fft_outputbuf);
    free(settings);
    return NULL;
  }

  settings->cfg = kiss_fftr_alloc(settings->dftblocksize, 0, NULL, NULL);

  settings->smpls_in_inputbuf = 0;

  settings->zratio_value = 0.0;

  return settings;
}


double run_zratio_filter(double new_sample, struct zratio_filter_settings *settings)
{
  int i;

  double power_delta,
         power_theta,
         power_alpha,
         power_beta,
         power_total;

  settings->fft_inputbuf[settings->smpls_in_inputbuf++] = new_sample;

  if(settings->smpls_in_inputbuf >= settings->dftblocksize)
  {
    settings->smpls_in_inputbuf = 0;

    kiss_fftr(settings->cfg, settings->fft_inputbuf, settings->kiss_fftbuf);

    power_delta = 0.0;
    power_theta = 0.0;
    power_alpha = 0.0;
    power_beta = 0.0;
    power_total = 0.0;

    for(i=0; i<settings->fft_outputbufsize; i++)
    {
      settings->fft_outputbuf[i] = (((settings->kiss_fftbuf[i].r * settings->kiss_fftbuf[i].r) + (settings->kiss_fftbuf[i].i * settings->kiss_fftbuf[i].i)) / settings->fft_outputbufsize);

      if((i >= settings->f0) && (i < settings->f1))
      {
        power_delta += settings->fft_outputbuf[i];
      }

      if((i >= settings->f1) && (i < settings->f2))
      {
        power_theta += settings->fft_outputbuf[i];
      }

      if((i >= settings->f2) && (i < settings->f3))
      {
        power_alpha += settings->fft_outputbuf[i];
      }

      if((i >= settings->f3) && (i <= settings->f4))
      {
        power_beta += settings->fft_outputbuf[i];
      }
    }

    power_total = power_delta + power_theta + power_alpha + power_beta;

    if(dblcmp(power_total, 0.0) > 0)
    {
      settings->zratio_value = ((power_delta + power_theta) - (power_alpha + power_beta)) / power_total;
    }
    else
    {
      settings->zratio_value = 0.0;
    }
  }

  return settings->zratio_value / settings->bitvalue;
}


void zratio_filter_save_buf(struct zratio_filter_settings *settings)
{
  settings->smpls_in_inputbuf_bu = settings->smpls_in_inputbuf;
  settings->zratio_value_bu = settings->zratio_value;
  memcpy(settings->fft_inputbuf_bu, settings->fft_inputbuf, settings->dftblocksize);
}


void zratio_filter_restore_buf(struct zratio_filter_settings *settings)
{
  settings->smpls_in_inputbuf = settings->smpls_in_inputbuf_bu;
  settings->zratio_value = settings->zratio_value_bu;
  memcpy(settings->fft_inputbuf, settings->fft_inputbuf_bu, settings->dftblocksize);
}


void free_zratio_filter(struct zratio_filter_settings *settings)
{
  free(settings->fft_inputbuf);
  free(settings->fft_inputbuf_bu);
  free(settings->fft_outputbuf);
  free(settings->kiss_fftbuf);
  free(settings->cfg);
  free(settings);
}


void reset_zratio_filter(struct zratio_filter_settings *settings)
{
  settings->smpls_in_inputbuf = 0;
  settings->zratio_value = 0.0;
}














