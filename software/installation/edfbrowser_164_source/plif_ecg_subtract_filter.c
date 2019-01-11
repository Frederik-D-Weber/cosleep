/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2017, 2018 Teunis van Beelen
*
* Email: teuniz@gmail.com
*
**************************************************************************
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
*
* Inspired by:
*
* - Subtraction Method For Powerline Interference Removing From ECG
*   Chavdar Levkov, Georgy Mihov, Ratcho Ivanov, Ivan K. Daskalov
*   Ivaylo Christov, Ivan Dotsinsky
*
* - Removal of power-line interference from the ECG: a review of the
*   subtraction procedure
*   Chavdar Levkov, Georgy Mihov, Ratcho Ivanov, Ivan Daskalov,
*   Ivaylo Christov and Ivan Dotsinsky
*
* - Accuracy of 50 Hz interference subtraction from an electrocardiogram
*   I. A, Dotsinsky I.K. Daskalov
*
* - Dynamic powerline interference subtraction from biosignals
*   Ivaylo I. Christov
*
*
* The subtraction method extracts the powerline interference noise during a
* a linear region between two consecutive QRS complexes and stores it in a buffer.
* Buffersize equals one complete powerlinefrequency cycle (20 milli-Sec.).
* The reference noise from the buffer is used to subtract it from the signal outside
* the linear region i.e. during the QRS complex.
* This method only works correctly when the ratio of the samplefrequency and the
* powerline frequency is an integer multiple.
* In case they are synchronized, this method will remove also the harmonics of the
* powerline frequency. In that case extra notch-filters for the harmonics are
* not necessary. The advantage of this method is that it will not cause ringing
* in the waveform of the QRS complex (like notch-filters do).
*
***************************************************************************
*/




#include "plif_ecg_subtract_filter.h"


/*
 *
 * sf: samplefrequency (must be >= 500Hz and must be an integer multiple of the powerline frequency)
 *
 * pwlf: powerline frequency (must be set to 50Hz or 60Hz)
 *
 * lt: linear region threshold, MUST BE SET TO 5uV!
 *     used to detect linear region between two consecutive QRS complexes
 *
 */
struct plif_subtract_filter_settings * plif_create_subtract_filter(int sf, int pwlf, double lt)
{
  int i;

  struct plif_subtract_filter_settings *st;

/* perform some sanity checks */
  if(sf < 500)  return NULL;  /* we need at least the samplefrequency considered the "gold standard" */

  if((pwlf != 50) && (pwlf != 60))  return NULL;  /* powerline frequency must be either 50 or 60Hz */

  if(sf % pwlf)  return NULL;  /* ratio between the samplefrequency and the powerline frequency must be an integer multiple */

  if((lt < 1) || (lt > 100000))  return NULL;  /* range for the linear detection threshold */

  st = (struct plif_subtract_filter_settings *) calloc(1, sizeof(struct plif_subtract_filter_settings));
  if(st==NULL)  return NULL;

  st->sf = sf;
  st->tpl = sf / pwlf;  /* the number of samples in one cycle of the powerline frequency */
  st->ravg_idx = 0;
  st->buf_idx = 0;
  st->linear_threshold = lt;  /* the threshold to detect the linear region */
  st->ravg_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ravg_buf == NULL) /* buffer for the running average filter */
  {
    free(st);
    return NULL;
  }
  st->ref_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf == NULL)  /* buffer for the reference noise, used to be subtracted from the ECG signal */
  {
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->ravg_noise_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->ravg_noise_buf[i] == NULL)  /* buffers used for the noise extraction */
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st);
      return NULL;
    }
  }
  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->input_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->input_buf[i] == NULL)  /* inputbuffers used for detecting the linear region */
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st);
      return NULL;
    }
  }

  return st;
}


double plif_run_subtract_filter(double new_input, struct plif_subtract_filter_settings *st)
{
  int i, j, pre, linear_buf_idx, linear_bufs, linear;

  double ravg_val, fd_max, fd_min, dtmp, thr, ret_val;

  if(st == NULL)
  {
    return 0;
  }

  /* running average filter */
  st->ravg_buf[st->ravg_idx] = new_input;

  ravg_val = 0;

  for(i=0; i<st->tpl; i++)
  {
    ravg_val += st->ravg_buf[i];
  }

  ravg_val /= st->tpl;

  /* delay the input with half tpl samples */
  new_input = st->ravg_buf[(st->ravg_idx + (st->tpl / 2)) % st->tpl];

  ret_val = new_input - st->ref_buf[st->ravg_idx];

  st->input_buf[st->buf_idx][st->ravg_idx] = new_input;

  st->ravg_noise_buf[st->buf_idx][st->ravg_idx] = new_input - ravg_val;  /* store the noise extracted from the signal into the buffers */

  if(++st->ravg_idx >= st->tpl)  /* buffer full? if so, check for linearity */
  {
    st->ravg_idx = 0;

    fd_max = 1e-9;
    fd_min = 1e9;

    pre = (st->buf_idx - 1 + PLIF_NBUFS) % PLIF_NBUFS;  /* index to the buffer before */

    for(i=0; i<st->tpl; i++)  /* compare this buffer with buffer before for their max and min values */
    {                         /* distance between the 1th differences equals tpl in order to exclude the powerline noise from the detection */
      dtmp = st->input_buf[st->buf_idx][i] - st->input_buf[pre][i];

      if(dtmp > fd_max)  fd_max = dtmp;

      if(dtmp < fd_min)  fd_min = dtmp;
    }

    st->linear_diff[st->buf_idx] = fd_max - fd_min;  /* for every buffer we store the maximum difference (related to the buffer before) */

    for(j=0; j<39; j++)
    {
      thr = (j + 1) * st->linear_threshold;  /* first we try with the lowest threshold possible (5uV) */
                                             /* if we can't find a linear region of at least 60 milli-seconds long, */
                                             /* we increase the threshold and try again */
      for(i=0, linear=0, linear_bufs=0; i<(PLIF_NBUFS - 1); i++)
      {
        linear_buf_idx = st->buf_idx - i + PLIF_NBUFS;
        linear_buf_idx %= PLIF_NBUFS;

        if(st->linear_diff[linear_buf_idx] < thr) linear_bufs++;
        else linear_bufs = 0;

        if(linear_bufs == 5)  /* we need five consegutive buffers (100 milli-sec.) to pass the threshold limit */
        {
          linear = 1;

          break;
        }
      }

      if(linear)  break;
    }

    if(linear)  /* are we in a linear region? */
    {
      for(j=0; j<3; j++)  /* average three buffers from the five (don't use the first and the last buffer) containing the extracted noise */
      {
        linear_buf_idx += j + PLIF_NBUFS;
        linear_buf_idx %= PLIF_NBUFS;

        if(!j)
        {
          for(i=0; i<st->tpl; i++)
          {
            st->ref_buf[i] = st->ravg_noise_buf[linear_buf_idx][i];
          }
        }
        else
        {
          for(i=0; i<st->tpl; i++)
          {
            st->ref_buf[i] += st->ravg_noise_buf[linear_buf_idx][i];
          }
        }
      }

      for(i=0; i<st->tpl; i++)
      {
        st->ref_buf[i] /= j;  /* calculate the average */
      }
    }

    st->buf_idx++;  /* increment the index */
    st->buf_idx %= PLIF_NBUFS; /* check boundary and roll-over if necessary */
  }

  return ret_val;
}


struct plif_subtract_filter_settings * plif_subtract_filter_create_copy(struct plif_subtract_filter_settings *st_ori)
{
  int i;

  struct plif_subtract_filter_settings *st;

  if(st_ori == NULL)
  {
    return NULL;
  }

  st = (struct plif_subtract_filter_settings *) calloc(1, sizeof(struct plif_subtract_filter_settings));
  if(st==NULL)  return NULL;

  *st = *st_ori;

  st->ravg_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ravg_buf == NULL)
  {
    free(st);
    return NULL;
  }
  memcpy(st->ravg_buf, st_ori->ravg_buf, sizeof(double) * st->tpl);

  st->ref_buf = (double *)calloc(1, sizeof(double) * st->tpl);
  if(st->ref_buf == NULL)
  {
    free(st->ravg_buf);
    free(st);
    return NULL;
  }
  memcpy(st->ref_buf, st_ori->ref_buf, sizeof(double) * st->tpl);

  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->ravg_noise_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->ravg_noise_buf[i] == NULL)
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st);
      return NULL;
    }
    memcpy(st->ravg_noise_buf[i], st_ori->ravg_noise_buf[i], sizeof(double) * st->tpl);
  }

  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->input_buf[i] = (double *)calloc(1, sizeof(double) * st->tpl);
    if(st->input_buf[i] == NULL)
    {
      free(st->ravg_buf);
      free(st->ref_buf);
      free(st);
      return NULL;
    }
    memcpy(st->input_buf[i], st_ori->input_buf[i], sizeof(double) * st->tpl);
  }

  for(i=0;i<PLIF_NBUFS; i++)
  {
    st->linear_diff[i] = st_ori->linear_diff[i];
  }

  return st;
}


void plif_free_subtract_filter(struct plif_subtract_filter_settings *st)
{
  int i;

  if(st == NULL)
  {
    return;
  }

  free(st->ravg_buf);
  for(i=0; i<PLIF_NBUFS; i++)
  {
    free(st->ravg_noise_buf[i]);
    free(st->input_buf[i]);
  }
  free(st->ref_buf);
  free(st);
}


void plif_reset_subtract_filter(struct plif_subtract_filter_settings *st, double reference)
{
  int i, j;

  if(st == NULL)
  {
    return;
  }

  st->ravg_idx = 0;
  st->buf_idx = 0;

  for(j=0; j<st->tpl; j++)
  {
    st->ravg_buf[j] = reference;
  }

  for(i=0; i<PLIF_NBUFS; i++)
  {
    memset(st->ravg_noise_buf[i], 0, sizeof(double) * st->tpl);

    for(j=0; j<st->tpl; j++)
    {
      st->input_buf[i][j] = reference;
    }

    st->linear_diff[i] = 1e9;
  }

  memset(st->ref_buf, 0, sizeof(double) * st->tpl);
}


void plif_subtract_filter_state_copy(struct plif_subtract_filter_settings *dest, struct plif_subtract_filter_settings *src)
{
  int i;

  if((dest == NULL) || (src == NULL))  return;

  if(dest->sf != src->sf)  return;

  if(dest->tpl != src->tpl)  return;

  dest->ravg_idx = src->ravg_idx;
  dest->buf_idx = src->buf_idx;
  dest->linear_threshold = src->linear_threshold;

  memcpy(dest->ravg_buf, src->ravg_buf, sizeof(double) * dest->tpl);
  memcpy(dest->ref_buf, src->ref_buf, sizeof(double) * dest->tpl);

  for(i=0; i<PLIF_NBUFS; i++)
  {
    memcpy(dest->ravg_noise_buf[i], src->ravg_noise_buf[i], sizeof(double) * dest->tpl);
    memcpy(dest->input_buf[i], src->input_buf[i], sizeof(double) * dest->tpl);

    dest->linear_diff[i] = src->linear_diff[i];
  }
}



















