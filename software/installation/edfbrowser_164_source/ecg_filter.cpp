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




#include "ecg_filter.h"



#define ECG_FILTER_STAT_BUFSIZE 262144

#define SECONDS_RUNIN 5


// #define ECG_FILTER_DEBUG_ACTIVE



struct ecg_filter_settings * create_ecg_filter(double sf, double bitval, int powerlinefreq)
{
  int i;

  double d_tmp;

  struct ecg_filter_settings *settings;

  if(sf < 99.9)  return NULL;

  if((powerlinefreq != 50) && (powerlinefreq != 60)) return NULL;

  settings = (struct ecg_filter_settings *) calloc(1, sizeof(struct ecg_filter_settings));
  if(settings==NULL)  return NULL;

  d_tmp = sf / (double)powerlinefreq;
  settings->avgfilter_50_size = d_tmp;
  if((d_tmp - settings->avgfilter_50_size) > 0.5)  settings->avgfilter_50_size++;

  d_tmp = sf / 35.0;
  settings->avgfilter_35_size = d_tmp;
  if((d_tmp - settings->avgfilter_35_size) > 0.5)  settings->avgfilter_35_size++;

  d_tmp = sf / 25.0;
  settings->avgfilter_25_size = d_tmp;
  if((d_tmp - settings->avgfilter_25_size) > 0.5)  settings->avgfilter_25_size++;

  settings->avgfilter_50_idx = 0;
  settings->avgfilter_35_idx = 0;
  settings->avgfilter_25_idx = 0;

  settings->avgfilter_50_buf = (double *)calloc(1, settings->avgfilter_50_size * sizeof(double));
  if(settings->avgfilter_50_buf == NULL)
  {
    free(settings);

    return NULL;
  }
  settings->avgfilter_50_buf_bu = (double *)calloc(1, settings->avgfilter_50_size * sizeof(double));
  if(settings->avgfilter_50_buf_bu == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings);

    return NULL;
  }
  settings->avgfilter_35_buf = (double *)calloc(1, settings->avgfilter_35_size * sizeof(double));
  if(settings->avgfilter_35_buf == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings);

    return NULL;
  }
  settings->avgfilter_35_buf_bu = (double *)calloc(1, settings->avgfilter_35_size * sizeof(double));
  if(settings->avgfilter_35_buf_bu == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings);

    return NULL;
  }
  settings->avgfilter_25_buf = (double *)calloc(1, settings->avgfilter_25_size * sizeof(double));
  if(settings->avgfilter_25_buf == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings);

    return NULL;
  }
  settings->avgfilter_25_buf_bu = (double *)calloc(1, settings->avgfilter_25_size * sizeof(double));
  if(settings->avgfilter_25_buf_bu == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings);

    return NULL;
  }

  settings->SV_idx = 0;
  settings->SV_sz = ((((int)sf) + 25) / 100) + 1;
  if(settings->SV_sz < 3)
  {
    settings->SV_sz = 3;
  }
  settings->SV = (double *)calloc(1, settings->SV_sz * sizeof(double));
  if(settings->SV == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings->avgfilter_25_buf_bu);
    free(settings);

    return NULL;
  }

  settings->M_startslope = sf * 0.2;
  settings->M_endslope = sf * 1.2;
  settings->smpl_n = 0;
  for(i=0; i<5; i++)
  {
    settings->M[i] = 0.0;
  }
  settings->M_avg = 0.0;
  settings->M_idx = 0;
  settings->drain = 0.0;
  settings->top = 0.0;
  settings->runin = SECONDS_RUNIN * sf;
  settings->sf = sf;

  settings->F_size = 0.35 * sf;
  settings->F_wd = 0.05 * sf;
  settings->F_sf_factor = 150.0 * (sf / 200.0);
  if(settings->F_sf_factor < 150.0)
  {
    settings->F_sf_factor = 150.0;
  }
  settings->F_value = 0.0;
  settings->F_idx = 0;
  settings->F = (double *)calloc(1, settings->F_size * sizeof(double));
  if(settings->F == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings->SV);
    free(settings);

    return NULL;
  }
  settings->F_bu = (double *)calloc(1, settings->F_size * sizeof(double));
  if(settings->F_bu == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings->SV);
    free(settings->F);
    free(settings);

    return NULL;
  }

  for(i=0; i<5; i++)
  {
    settings->R[i] = sf;
  }
  settings->R_idx = 0;
  settings->R_avg = sf;
  settings->R_value = 0.0;
  settings->R_startslope = sf * 0.66;
  settings->R_endslope = sf;

  settings->bu_filled = 0;

  settings->bpm = 60.0;

  settings->bitvalue = bitval;

  settings->stat_buf_idx = 0;

  settings->sample_cntr = 0;

  settings->T_peak_avg = 0.0;

  settings->stat_buf = (double *)malloc(sizeof(double) * ECG_FILTER_STAT_BUFSIZE);
  if(settings->stat_buf == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings->SV);
    free(settings->F);
    free(settings->F_bu);
    free(settings);

    return NULL;
  }

  settings->stat_smpl_buf = (long long *)malloc(sizeof(long long) * ECG_FILTER_STAT_BUFSIZE);
  if(settings->stat_smpl_buf == NULL)
  {
    free(settings->avgfilter_50_buf);
    free(settings->avgfilter_50_buf_bu);
    free(settings->avgfilter_35_buf);
    free(settings->avgfilter_35_buf_bu);
    free(settings->avgfilter_25_buf);
    free(settings->SV);
    free(settings->F);
    free(settings->F_bu);
    free(settings->stat_buf);
    free(settings);

    return NULL;
  }

  return settings;
}


double run_ecg_filter(double new_sample, struct ecg_filter_settings *settings)
// double run_ecg_filter(double new_sample, struct ecg_filter_settings *settings, int signalcompnr)  // debug
{
  int i, j;

  double new_input,
         max_value,
         result;


/* running average filter 1 */

  settings->avgfilter_50_buf[settings->avgfilter_50_idx] = new_sample;

  settings->avgfilter_50_idx++;

  if(settings->avgfilter_50_idx >= settings->avgfilter_50_size)
  {
    settings->avgfilter_50_idx = 0;
  }

  settings->avgfilter_50_mean = 0.0;

  for(i=0; i<settings->avgfilter_50_size; i++)
  {
    settings->avgfilter_50_mean += settings->avgfilter_50_buf[i];
  }

  settings->avgfilter_50_mean /= settings->avgfilter_50_size;

/* running average filter 2 */

  settings->avgfilter_35_buf[settings->avgfilter_35_idx] = settings->avgfilter_50_mean;

  settings->avgfilter_35_idx++;

  if(settings->avgfilter_35_idx >= settings->avgfilter_35_size)
  {
    settings->avgfilter_35_idx = 0;
  }

  settings->avgfilter_35_mean = 0.0;

  for(i=0; i<settings->avgfilter_35_size; i++)
  {
    settings->avgfilter_35_mean += settings->avgfilter_35_buf[i];
  }

  settings->avgfilter_35_mean /= settings->avgfilter_35_size;

/* running average filter 3 */

  settings->avgfilter_25_buf[settings->avgfilter_25_idx] = settings->avgfilter_35_mean;

  settings->avgfilter_25_idx++;

  if(settings->avgfilter_25_idx >= settings->avgfilter_25_size)
  {
    settings->avgfilter_25_idx = 0;
  }

  settings->avgfilter_25_mean = 0.0;

  for(i=0; i<settings->avgfilter_25_size; i++)
  {
    settings->avgfilter_25_mean += settings->avgfilter_25_buf[i];
  }

  settings->avgfilter_25_mean /= settings->avgfilter_25_size;

// return settings->avgfilter_25_mean;  // debug

// if(signalcompnr == 1)  return settings->avgfilter_25_mean;  // debug

/* spatial velocity filter */

  settings->SV[settings->SV_idx] = settings->avgfilter_25_mean;

  new_input = settings->SV[settings->SV_idx] - settings->SV[(settings->SV_idx + 1) % settings->SV_sz];

  if(++settings->SV_idx >= settings->SV_sz)  settings->SV_idx = 0;

  if(new_input < 0.0)  new_input *= -1.0;

// return new_input;  // debug

// if(signalcompnr == 2)  return new_input;  // debug

  settings->F[settings->F_idx++] = new_input;
  settings->F_idx %= settings->F_size;

  max_value = 0.0;
  for(i=0; i<settings->F_wd; i++)
  {
    j = (settings->F_idx + settings->F_size - i - 1) % settings->F_size;

    if(settings->F[j] > max_value)
    {
      max_value = settings->F[j];
    }
  }
  result = max_value;

  max_value = 0.0;
  for(i=0; i<settings->F_wd; i++)
  {
    j = (settings->F_idx + i) % settings->F_size;

    if(settings->F[j] > max_value)
    {
      max_value = settings->F[j];
    }
  }
  result -= max_value;

  settings->F_value += (result / settings->F_sf_factor);

// return settings->F_value;  // debug

////////////////////////////////////////////////

  if(settings->runin)
  {
    if(settings->sample_cntr > settings->sf)
    {
      if(new_input > settings->top)
      {
        settings->top = new_input;
      }

      if(new_input > settings->T_peak_avg)
      {
        settings->T_peak_avg = new_input;
      }
    }

    if(settings->sample_cntr == settings->sf)
    {
      settings->F_value = 0.0;

      settings->F_idx = 0;

      memset(settings->F, 0, settings->F_size * sizeof(double));
    }

    if(settings->runin == 1)
    {
      for(i=0; i<5; i++)
      {
        settings->M[i] = settings->top * 0.5;
      }

      settings->M_avg = settings->top * 0.5;

      settings->drain = settings->top * 0.2 / settings->sf;

      settings->smpl_n = settings->M_startslope;

#ifdef ECG_FILTER_DEBUG_ACTIVE
      printf("->   settings->drain is %f\n", settings->drain);

      printf("->   settings->M_avg is %f\n", settings->M_avg);
#endif
    }

    settings->runin--;

    settings->sample_cntr++;

// if(signalcompnr == 6)  return settings->M_avg + settings->R_value;  // debug: check ecg detection treshold
// if(signalcompnr == 3)  return settings->M_avg;  // debug: check ecg detection treshold
// if(signalcompnr == 4)  return settings->F_value;  // debug: check ecg detection treshold
// if(signalcompnr == 5)  return settings->R_value;  // debug: check ecg detection treshold
// if(signalcompnr == 7)  return settings->T_peak_avg / 2.0;  // debug: check ecg detection treshold

    return settings->bpm / settings->bitvalue;
  }

  if(new_input > settings->T_peak_avg)
  {
    settings->T_peak_avg += ((new_input - settings->T_peak_avg) / (settings->sf / 10.0));
  }
  else
  {
    settings->T_peak_avg -= (settings->T_peak_avg / (settings->sf * 10.0));
  }

  if((settings->smpl_n > settings->R_startslope) && (settings->smpl_n < settings->R_endslope))
  {
    settings->R_value -= (settings->drain * 0.7);
  }

  if(settings->smpl_n < settings->M_startslope)
  {
    if(new_input > settings->top)
    {
      settings->top = new_input;

#ifdef ECG_FILTER_DEBUG_ACTIVE
      printf("new_input is %f\n", new_input);
#endif
    }

    settings->sample_cntr++;

    settings->smpl_n++;
  }
  else
  {
    if(settings->smpl_n == settings->M_startslope)
    {
      settings->top *= 0.6;

#ifdef ECG_FILTER_DEBUG_ACTIVE
      printf("settings->top is %f\n", settings->top);
#endif
      if(settings->top > (1.5 * settings->M[settings->M_idx]))
      {
        settings->top = 1.1 * settings->M[settings->M_idx];
      }

      settings->M[settings->M_idx++] = settings->top;

      settings->M_idx %= 5;

      settings->M_avg = (settings->M[0] + settings->M[1] + settings->M[2] + settings->M[3] + settings->M[4]) / 5.0;

      settings->drain = settings->M_avg * 0.4 / settings->sf;

#ifdef ECG_FILTER_DEBUG_ACTIVE
      printf("settings->top is %f\n", settings->top);

      printf("settings->M_avg is %f\n", settings->M_avg);

      printf("settings->drain is %f\n", settings->drain);
#endif
    }

    if((new_input > (settings->M_avg + settings->F_value + settings->R_value))  // beat detected?
      && (new_input > (settings->T_peak_avg / 2.0)))
//    if(new_input > (settings->M_avg + settings->F_value + settings->R_value))  // beat detected?
//    if(new_input > (settings->M_avg + settings->R_value))  // beat detected?  // debug
//    if(new_input > (settings->M_avg))  // debug
    {
      settings->top = new_input;

      settings->R[settings->R_idx++] = settings->smpl_n;

#ifdef ECG_FILTER_DEBUG_ACTIVE
      printf("beat detected: settings->top is %f\n", settings->top);

      printf("beat detected: settings->smpl_n is %i\n", settings->smpl_n);
#endif
      settings->R_idx %= 5;

      settings->R_avg = (settings->R[0] + settings->R[1] + settings->R[2] + settings->R[3] + settings->R[4]) / 5;

      settings->R_value = 0.0;

      settings->R_startslope = 0.66 * settings->R_avg;

      settings->R_endslope = settings->R_avg;

      settings->bpm = (settings->sf * 60.0) / settings->smpl_n;

      if(settings->stat_buf_idx < (ECG_FILTER_STAT_BUFSIZE - 1))
      {
        settings->stat_buf[settings->stat_buf_idx] = ((double)settings->smpl_n) / settings->sf;

        if(settings->stat_buf_idx > 0)
        {
          settings->stat_smpl_buf[settings->stat_buf_idx] = settings->stat_smpl_buf[settings->stat_buf_idx - 1] + settings->sample_cntr;
        }
        else
        {
          settings->stat_smpl_buf[settings->stat_buf_idx] = settings->sample_cntr;
        }

        settings->stat_buf_idx++;
      }

      settings->sample_cntr = 1;

      settings->smpl_n = 1;
    }
    else
    {
      if(settings->smpl_n < settings->M_endslope)
      {
        settings->M_avg -= settings->drain;
      }

      settings->sample_cntr++;

      settings->smpl_n++;
    }
  }

// if(signalcompnr == 6)  return settings->M_avg + settings->F_value + settings->R_value;  // debug: check ecg detection treshold
// if(signalcompnr == 6)  return settings->M_avg + settings->R_value;  // debug: check ecg detection treshold
// if(signalcompnr == 3)  return settings->M_avg;  // debug: check ecg detection treshold
// if(signalcompnr == 4)  return settings->F_value;  // debug: check ecg detection treshold
// if(signalcompnr == 5)  return settings->R_value;  // debug: check ecg detection treshold
// if(signalcompnr == 7)  return settings->T_peak_avg / 2.0;  // debug: check ecg detection treshold

  return settings->bpm / settings->bitvalue;
}


void ecg_filter_save_buf(struct ecg_filter_settings *settings)
{
  int i;


  settings->avgfilter_50_idx_bu = settings->avgfilter_50_idx;
  memcpy(settings->avgfilter_50_buf_bu, settings->avgfilter_50_buf, sizeof(double) * settings->avgfilter_50_size);
  settings->avgfilter_50_mean_bu = settings->avgfilter_50_mean;

  settings->avgfilter_35_idx_bu = settings->avgfilter_35_idx;
  memcpy(settings->avgfilter_35_buf_bu, settings->avgfilter_35_buf, sizeof(double) * settings->avgfilter_35_size);
  settings->avgfilter_35_mean_bu = settings->avgfilter_35_mean;

  settings->avgfilter_25_idx_bu = settings->avgfilter_25_idx;
  memcpy(settings->avgfilter_25_buf_bu, settings->avgfilter_25_buf, sizeof(double) * settings->avgfilter_25_size);
  settings->avgfilter_25_mean_bu = settings->avgfilter_25_mean;

  settings->runin_bu = settings->runin;

  for(i=0; i<3; i++)
  {
    settings->SV_bu[i] = settings->SV[i];
  }
  settings->SV_idx_bu = settings->SV_idx;

  settings->smpl_n_bu = settings->smpl_n;
  for(i=0; i<5; i++)
  {
    settings->M_bu[i] = settings->M[i];
    settings->R_bu[i] = settings->R[i];
  }
  settings->M_idx_bu = settings->M_idx;
  settings->M_avg_bu = settings->M_avg;
  settings->drain_bu = settings->drain;
  settings->top_bu = settings->top;
  for(i=0; i<settings->F_size; i++)
  {
    settings->F_bu[i] = settings->F[i];
  }
  settings->F_idx_bu = settings->F_idx;
  settings->F_value_bu = settings->F_value;
  settings->R_idx_bu = settings->R_idx;
  settings->R_avg_bu = settings->R_avg;
  settings->R_value_bu = settings->R_value;
  settings->R_startslope_bu = settings->R_startslope;
  settings->R_endslope_bu = settings->R_endslope;
  settings->bpm_bu = settings->bpm;
  settings->T_peak_avg_bu = settings->T_peak_avg;

  settings->bu_filled = 1;
}


void ecg_filter_restore_buf(struct ecg_filter_settings *settings)
{
  int i;


  settings->stat_buf_idx = 0;

  settings->sample_cntr = 0;

  if(settings->bu_filled == 0)  return;

  settings->avgfilter_50_idx = settings->avgfilter_50_idx_bu;
  memcpy(settings->avgfilter_50_buf, settings->avgfilter_50_buf_bu, sizeof(double) * settings->avgfilter_50_size);
  settings->avgfilter_50_mean = settings->avgfilter_50_mean_bu;

  settings->avgfilter_35_idx = settings->avgfilter_35_idx_bu;
  memcpy(settings->avgfilter_35_buf, settings->avgfilter_35_buf_bu, sizeof(double) * settings->avgfilter_35_size);
  settings->avgfilter_35_mean = settings->avgfilter_35_mean_bu;

  settings->avgfilter_25_idx = settings->avgfilter_25_idx_bu;
  memcpy(settings->avgfilter_25_buf, settings->avgfilter_25_buf_bu, sizeof(double) * settings->avgfilter_25_size);
  settings->avgfilter_25_mean = settings->avgfilter_25_mean_bu;

  settings->runin = settings->runin_bu;

  for(i=0; i<3; i++)
  {
    settings->SV[i] = settings->SV_bu[i];
  }
  settings->SV_idx = settings->SV_idx_bu;

  settings->smpl_n = settings->smpl_n_bu;
  for(i=0; i<5; i++)
  {
    settings->M[i] = settings->M_bu[i];
    settings->R[i] = settings->R_bu[i];
  }
  settings->M_idx = settings->M_idx_bu;
  settings->M_avg = settings->M_avg_bu;
  settings->drain = settings->drain_bu;
  settings->top = settings->top_bu;
  for(i=0; i<settings->F_size; i++)
  {
    settings->F[i] = settings->F_bu[i];
  }
  settings->F_idx = settings->F_idx_bu;
  settings->F_value = settings->F_value_bu;
  settings->R_idx = settings->R_idx_bu;
  settings->R_avg = settings->R_avg_bu;
  settings->R_value = settings->R_value_bu;
  settings->R_startslope = settings->R_startslope_bu;
  settings->R_endslope = settings->R_endslope_bu;
  settings->bpm = settings->bpm_bu;
  settings->T_peak_avg = settings->T_peak_avg_bu;
}


void reset_ecg_filter(struct ecg_filter_settings *settings)
{
  int i;


  settings->avgfilter_50_idx = 0;
  settings->avgfilter_35_idx = 0;
  settings->avgfilter_25_idx = 0;

  memset(settings->avgfilter_50_buf, 0, settings->avgfilter_50_size * sizeof(double));
  memset(settings->avgfilter_35_buf, 0, settings->avgfilter_35_size * sizeof(double));
  memset(settings->avgfilter_25_buf, 0, settings->avgfilter_25_size * sizeof(double));

  settings->SV_idx = 0;
  for(i=0; i<settings->SV_sz; i++)
  {
    settings->SV[i] = 0.0;
  }
  settings->smpl_n = 0;
  for(i=0; i<5; i++)
  {
    settings->M[i] = 0.0;
  }
  settings->M_avg = 0.0;
  settings->M_idx = 0;
  settings->drain = 0.0;
  settings->top = 0.0;
  settings->runin = SECONDS_RUNIN * settings->sf;

  settings->F_value = 0.0;
  settings->F_idx = 0;
  memset(settings->F, 0, settings->F_size * sizeof(double));

  for(i=0; i<5; i++)
  {
    settings->R[i] = settings->sf;
  }
  settings->R_idx = 0;
  settings->R_avg = settings->sf;
  settings->R_value = 0.0;
  settings->R_startslope = settings->sf * 0.66;
  settings->R_endslope = settings->sf;

  settings->bu_filled = 0;

  settings->bpm = 60.0;

  settings->stat_buf_idx = 0;

  settings->sample_cntr = 0;

  settings->T_peak_avg = 0.0;
}


void free_ecg_filter(struct ecg_filter_settings *settings)
{
  free(settings->avgfilter_50_buf);
  free(settings->avgfilter_50_buf_bu);
  free(settings->avgfilter_35_buf);
  free(settings->avgfilter_35_buf_bu);
  free(settings->avgfilter_25_buf);
  free(settings->avgfilter_25_buf_bu);
  free(settings->SV);
  free(settings->F);
  free(settings->F_bu);
  free(settings->stat_buf);
  free(settings->stat_smpl_buf);
  free(settings);
}


int ecg_filter_get_beat_cnt(struct ecg_filter_settings *settings)
{
  return settings->stat_buf_idx;
}


long long * ecg_filter_get_onset_beatlist(struct ecg_filter_settings *settings)
{
  return settings->stat_smpl_buf;
}


double * ecg_filter_get_interval_beatlist(struct ecg_filter_settings *settings)
{
  return settings->stat_buf;
}











