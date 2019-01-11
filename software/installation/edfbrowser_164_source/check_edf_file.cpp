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



#include "check_edf_file.h"



struct edfhdrblock * EDFfileCheck::check_edf_file(FILE *inputfile, char *txt_string, int live_stream)
{
  int i, j, p, r=0, n,
      dotposition,
      error;

  char *edf_hdr,
       scratchpad[128],
       scratchpad2[64];

  long long l_tmp,
            l_tmp2;

  struct date_time_struct date_time;

/***************** check header ******************************/

  edf_hdr = (char *)calloc(1, 256);
  if(edf_hdr==NULL)
  {
    sprintf(txt_string, "Memory allocation error. (edf_hdr)");
    return NULL;
  }

  edfhdr = (struct edfhdrblock *)calloc(1, sizeof(struct edfhdrblock));
  if(edfhdr==NULL)
  {
    sprintf(txt_string, "Memory allocation error. (edfhdr)");
    return NULL;
  }

  rewind(inputfile);
  if(fread(edf_hdr, 256, 1, inputfile)!=1)
  {
    sprintf(txt_string, "Error, reading file");
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

/**************************** VERSION ***************************************/

  strncpy(scratchpad, edf_hdr, 8);
  scratchpad[8] = 0;

  if(((signed char *)scratchpad)[0]==-1)   // BDF-file
  {
    for(i=1; i<8; i++)
    {
      if((scratchpad[i]<32)||(scratchpad[i]>126))
      {
        sprintf(txt_string, "Error, %ith character of version field is not a valid 7-bit ASCII character.",
        i + 1);
        free(edf_hdr);
        free(edfhdr);
        return NULL;
      }
    }

    if(strcmp(scratchpad + 1, "BIOSEMI"))
    {
      sprintf(txt_string, "Error, header has unknown version: \".%s\",\n"
            "expected \".BIOSEMI\"",
            scratchpad + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }

    edfhdr->bdf = 1;
  }
  else    // EDF-file
  {
    for(i=0; i<8; i++)
    {
      if((scratchpad[i]<32)||(scratchpad[i]>126))
      {
        sprintf(txt_string, "Error, %ith character of version field is not a valid 7-bit ASCII character.",
        i + 1);
        free(edf_hdr);
        free(edfhdr);
        return NULL;
      }
    }

    if(strcmp(scratchpad, "0       "))
    {
      sprintf(txt_string, "Error, header has unknown version: \"%s\",\n"
            "expected \"0       \"",
            scratchpad);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }

    edfhdr->edf = 1;
  }

  strncpy(edfhdr->version, edf_hdr, 8);
  edfhdr->version[8] = 0;
  if(edfhdr->bdf)  edfhdr->version[0] = '.';

/********************* PATIENTNAME *********************************************/

  strncpy(scratchpad, edf_hdr + 8, 80);
  scratchpad[80] = 0;
  for(i=0; i<80; i++)
  {
    if((((unsigned char *)scratchpad)[i]<32)||(((unsigned char *)scratchpad)[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of local patient identification field is not a valid 7-bit ASCII character.\n"
                          "Use the header editor to fix your file. Look at the manual for the details.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  strncpy(edfhdr->patient, edf_hdr + 8, 80);
  edfhdr->patient[80] = 0;

/********************* RECORDING *********************************************/

  strncpy(scratchpad, edf_hdr + 88, 80);
  scratchpad[80] = 0;
  for(i=0; i<80; i++)
  {
    if((((unsigned char *)scratchpad)[i]<32)||(((unsigned char *)scratchpad)[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of local recording identification field is not a valid 7-bit ASCII character.\n"
                          "Use the header editor to fix your file. Look at the manual for the details.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  strncpy(edfhdr->recording, edf_hdr + 88, 80);
  edfhdr->recording[80] = 0;

/********************* STARTDATE *********************************************/

  strncpy(scratchpad, edf_hdr + 168, 8);
  scratchpad[8] = 0;
  for(i=0; i<8; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of startdate field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  error = 0;

  if((edf_hdr[170]!='.')||(edf_hdr[173]!='.'))
  {
    sprintf(txt_string, "Error, separator character of startdate is not a dot.\n"
                        "You can fix this problem with the header editor, check the manual for details.");
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  if((edf_hdr[168]<48)||(edf_hdr[168]>57))      error = 1;
  if((edf_hdr[169]<48)||(edf_hdr[169]>57))      error = 1;
  if((edf_hdr[171]<48)||(edf_hdr[171]>57))      error = 1;
  if((edf_hdr[172]<48)||(edf_hdr[172]>57))      error = 1;
  if((edf_hdr[174]<48)||(edf_hdr[174]>57))      error = 1;
  if((edf_hdr[175]<48)||(edf_hdr[175]>57))      error = 1;
  strncpy(scratchpad, edf_hdr + 168, 8);

  if(error)
  {
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, startdate of recording is invalid: \"%s\",\n"
           "expected \"dd.mm.yy\"",
           scratchpad);
    free(edf_hdr);
    return NULL;
  }

  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;

  if((atoi(scratchpad)<1)||(atoi(scratchpad)>31))
  {
    strncpy(scratchpad, edf_hdr + 168, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, startdate of recording is invalid: \"%s\",\n"
           "expected \"dd.mm.yy\" where \"dd\" should be more than 00 and less than 32",
           scratchpad);
    free(edf_hdr);
    return NULL;
  }

  if((atoi(scratchpad+3)<1)||(atoi(scratchpad+3)>12))
  {
    strncpy(scratchpad, edf_hdr + 168, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, startdate of recording is invalid: \"%s\",\n"
           "expected \"dd.mm.yy\" where \"mm\" should be more than 00 and less than 13",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  date_time.day = atoi(scratchpad);
  date_time.month = atoi(scratchpad + 3);
  date_time.year = atoi(scratchpad + 6);
  if(date_time.year>84)
  {
    date_time.year += 1900;
  }
  else
  {
    date_time.year += 2000;
  }

/********************* STARTTIME *********************************************/

  strncpy(scratchpad, edf_hdr + 176, 8);
  scratchpad[8] = 0;
  for(i=0; i<8; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of starttime field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  error = 0;

  if((edf_hdr[178]!='.')||(edf_hdr[181]!='.'))
  {
    sprintf(txt_string, "Error, separator character of starttime is not a dot.\n"
                        "You can fix this problem with the header editor, check the manual for details.");
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  if((edf_hdr[176]<48)||(edf_hdr[176]>57))      error = 1;
  if((edf_hdr[177]<48)||(edf_hdr[177]>57))      error = 1;
  if((edf_hdr[179]<48)||(edf_hdr[179]>57))      error = 1;
  if((edf_hdr[180]<48)||(edf_hdr[180]>57))      error = 1;
  if((edf_hdr[182]<48)||(edf_hdr[182]>57))      error = 1;
  if((edf_hdr[183]<48)||(edf_hdr[183]>57))      error = 1;

  strncpy(scratchpad, edf_hdr + 176, 8);

  if(error)
  {
    strncpy(scratchpad, edf_hdr + 176, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, starttime of recording is invalid: \"%s\",\n"
           "expected \"hh.mm.ss\"",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  scratchpad[2] = 0;
  scratchpad[5] = 0;
  scratchpad[8] = 0;

  if(atoi(scratchpad)>23)
  {
    strncpy(scratchpad, edf_hdr + 176, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, starttime of recording is invalid: \"%s\",\n"
           "expected \"hh.mm.ss\" where \"hh\" should be less than 24",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  if(atoi(scratchpad+3)>59)
  {
    strncpy(scratchpad, edf_hdr + 176, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, starttime of recording is invalid: \"%s\",\n"
           "expected \"hh.mm.ss\" where \"mm\" should be less than 60",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  if(atoi(scratchpad+6)>59)
  {
    strncpy(scratchpad, edf_hdr + 176, 8);
    scratchpad[8] = 0;
    sprintf(txt_string, "Error, starttime of recording is invalid: \"%s\",\n"
           "expected \"hh.mm.ss\" where \"ss\" should be less than 60",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  date_time.hour = atoi(scratchpad);
  date_time.minute = atoi(scratchpad + 3);
  date_time.second = atoi(scratchpad + 6);

  date_time_to_utc(&edfhdr->utc_starttime, date_time);

  edfhdr->l_starttime = 3600 * atoi(scratchpad);
  edfhdr->l_starttime += 60 * atoi(scratchpad + 3);
  edfhdr->l_starttime += atoi(scratchpad + 6);

  edfhdr->l_starttime *= TIME_DIMENSION;

/***************** NUMBER OF SIGNALS IN HEADER *******************************/

  strncpy(scratchpad, edf_hdr + 252, 4);
  scratchpad[4] = 0;
  for(i=0; i<4; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of number of signals field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  if(is_integer_number(scratchpad))
  {
    sprintf(txt_string, "Error, number of signals field is invalid: \"%s\".",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }
  edfhdr->edfsignals = atoi(scratchpad);
  if(edfhdr->edfsignals<1)
  {
    sprintf(txt_string, "Error, number of signals is %i, expected >0.",
           edfhdr->edfsignals);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  if(edfhdr->edfsignals>MAXSIGNALS)
  {
    sprintf(txt_string, "Error, number of signals in file is %i, can not support more than %i signals.",
           edfhdr->edfsignals,
           MAXSIGNALS);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

/***************** NUMBER OF BYTES IN HEADER *******************************/

  strncpy(scratchpad, edf_hdr + 184, 8);
  scratchpad[8] = 0;

  for(i=0; i<8; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of number of bytes in header field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  if(is_integer_number(scratchpad))
  {
    sprintf(txt_string, "Error, number of bytes in header field is invalid: \"%s\".",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  n  = atoi(scratchpad);
  if((edfhdr->edfsignals * 256 + 256)!=n)
  {
    sprintf(txt_string, "Error, number of bytes in header does not match: %i,\n"
           "expected %i signals * 256 + 256 = %i bytes.",
           n, edfhdr->edfsignals, edfhdr->edfsignals * 256 + 256);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

/********************* RESERVED FIELD *************************************/

  edfhdr->edfplus = 0;
  edfhdr->discontinuous = 0;
  strncpy(scratchpad, edf_hdr + 192, 44);
  scratchpad[44] = 0;

  for(i=0; i<44; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of reserved field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  if(edfhdr->edf)
  {
    if(!strncmp(scratchpad, "EDF+C", 5))
    {
      edfhdr->edfplus = 1;
    }

    if(!strncmp(scratchpad, "EDF+D", 5))
    {
      edfhdr->edfplus = 1;
      edfhdr->discontinuous = 1;
    }
  }

  if(edfhdr->bdf)
  {
    if(!strncmp(scratchpad, "BDF+C", 5))
    {
      edfhdr->bdfplus = 1;
    }

    if(!strncmp(scratchpad, "BDF+D", 5))
    {
      edfhdr->bdfplus = 1;
      edfhdr->discontinuous = 1;
    }
  }

  strncpy(edfhdr->reserved, edf_hdr + 192, 44);
  edfhdr->reserved[44] = 0;

/********************* NUMBER OF DATARECORDS *************************************/

  strncpy(scratchpad, edf_hdr + 236, 8);
  scratchpad[8] = 0;

  for(i=0; i<8; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of number of datarecords field is not a valid ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  if(is_integer_number(scratchpad))
  {
    sprintf(txt_string, "Error, number of datarecords field is invalid: \"%s\".\n"
                        "You can fix this problem with the header editor, check the manual for details.",
           scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  edfhdr->datarecords = atoi(scratchpad);
  if(live_stream == 0)
  {
    if(edfhdr->datarecords<1)
    {
#ifdef Q_OS_WIN32
      __mingw_sprintf(txt_string, "Error, number of datarecords is %lli, expected >0.\n"
                          "You can fix this problem with the header editor, check the manual for details.",
            edfhdr->datarecords);
#else
      sprintf(txt_string, "Error, number of datarecords is %lli, expected >0.\n"
                          "You can fix this problem with the header editor, check the manual for details.",
            edfhdr->datarecords);
#endif
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

/********************* DATARECORD DURATION *************************************/

  strncpy(scratchpad, edf_hdr + 244, 8);
  scratchpad[8] = 0;

  for(i=0; i<8; i++)
  {
    if((scratchpad[i]<32)||(scratchpad[i]>126))
    {
      sprintf(txt_string, "Error, %ith character of recordduration field is not a valid 7-bit ASCII character.",
      i + 1);
      free(edf_hdr);
      free(edfhdr);
      return NULL;
    }
  }

  if(is_number(scratchpad))
  {
    sprintf(txt_string, "Error, record duration field is invalid: \"%s\".",
            scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  edfhdr->data_record_duration = atof(scratchpad);
  if(edfhdr->data_record_duration<-0.000001)
  {
    sprintf(txt_string, "Error, record duration is invalid: \"%s\", should be >=0",
            scratchpad);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  edfhdr->long_data_record_duration = get_long_duration(scratchpad);

  free(edf_hdr);

/********************* START WITH THE SIGNALS IN THE HEADER *********************/

  edf_hdr = (char *)calloc(1, (edfhdr->edfsignals + 1) * 256);
  if(edf_hdr==NULL)
  {
    sprintf(txt_string, "Memory allocation error. (edf_hdr takes %i bytes)", (edfhdr->edfsignals + 1) * 256);
    free(edfhdr);
    return NULL;
  }

  rewind(inputfile);
  if(fread(edf_hdr, (edfhdr->edfsignals + 1) * 256, 1, inputfile)!=1)
  {
    sprintf(txt_string, "Error, reading %i bytes from file", (edfhdr->edfsignals + 1) * 256);
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

  edfhdr->edfparam = (struct edfparamblock *)calloc(1, sizeof(struct edfparamblock[edfhdr->edfsignals]));
  if(edfhdr->edfparam==NULL)
  {
    sprintf(txt_string, "Memory allocation error! (edfparam)");
    free(edf_hdr);
    free(edfhdr);
    return NULL;
  }

/**************************** LABELS *************************************/

  edfhdr->nr_annot_chns = 0;
  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (i * 16), 16);
    for(j=0; j<16; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of label field of signal %i is not a valid 7-bit ASCII character.\n"
                            "Use the header editor to fix your file. Look at the manual for the details.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    if(edfhdr->edfplus)
    {
      if(!strncmp(scratchpad, "EDF Annotations ", 16))
      {
        edfhdr->annot_ch[edfhdr->nr_annot_chns] = i;
        edfhdr->nr_annot_chns++;
        edfhdr->edfparam[i].annotation = 1;
      }
    }
    if(edfhdr->bdfplus)
    {
      if(!strncmp(scratchpad, "BDF Annotations ", 16))
      {
        edfhdr->annot_ch[edfhdr->nr_annot_chns] = i;
        edfhdr->nr_annot_chns++;
        edfhdr->edfparam[i].annotation = 1;
      }
    }
    strncpy(edfhdr->edfparam[i].label, edf_hdr + 256 + (i * 16), 16);
    edfhdr->edfparam[i].label[16] = 0;
  }
  if(edfhdr->edfplus&&(!edfhdr->nr_annot_chns))
  {
    sprintf(txt_string, "Error, file is marked as EDF+ but it has no annotations-signal.");
    free(edf_hdr);
    free(edfhdr->edfparam);
    free(edfhdr);
    return NULL;
  }
  if(edfhdr->bdfplus&&(!edfhdr->nr_annot_chns))
  {
    sprintf(txt_string, "Error, file is marked as BDF+ but it has no annotations-signal.");
    free(edf_hdr);
    free(edfhdr->edfparam);
    free(edfhdr);
    return NULL;
  }
  if((edfhdr->edfsignals!=edfhdr->nr_annot_chns)||((!edfhdr->edfplus)&&(!edfhdr->bdfplus)))
  {
    if(edfhdr->data_record_duration<0.000001)
    {
      sprintf(txt_string, "Error, record duration is invalid, should be >0");
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }

/**************************** TRANSDUCER TYPES *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 16) + (i * 80), 80);
    for(j=0; j<80; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of transducer type field of signal %i is not a valid 7-bit ASCII character.\n"
                            "Use the header editor to fix your file. Look at the manual for the details.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    strncpy(edfhdr->edfparam[i].transducer, edf_hdr + 256 + (edfhdr->edfsignals * 16) + (i * 80), 80);
    edfhdr->edfparam[i].transducer[80] = 0;

    if(edfhdr->edfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        for(j=0; j<80; j++)
        {
          if(edfhdr->edfparam[i].transducer[j]!=' ')
          {
            sprintf(txt_string, "Error, transducer type field of EDF annotationchannel must be empty.\n"
                                "Use the header editor to fix your file. Look at the manual for the details.");
            free(edf_hdr);
            free(edfhdr->edfparam);
            free(edfhdr);
            return NULL;
          }
        }
      }
    }
    if(edfhdr->bdfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        for(j=0; j<80; j++)
        {
          if(edfhdr->edfparam[i].transducer[j]!=' ')
          {
            sprintf(txt_string, "Error, transducer type field of BDF annotationchannel must be empty."
                                "Use the header editor to fix your file. Look at the manual for the details.");
            free(edf_hdr);
            free(edfhdr->edfparam);
            free(edfhdr);
            return NULL;
          }
        }
      }
    }
  }

/**************************** PHYSICAL DIMENSIONS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 96) + (i * 8), 8);
    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of physical dimension field of signal %i is not a valid 7-bit ASCII character.\n"
                            "Use the header editor to fix your file. Look at the manual for the details.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    strncpy(edfhdr->edfparam[i].physdimension, edf_hdr + 256 + (edfhdr->edfsignals * 96) + (i * 8), 8);
    edfhdr->edfparam[i].physdimension[8] = 0;
  }

/**************************** PHYSICAL MINIMUMS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 104) + (i * 8), 8);
    scratchpad[8] = 0;

    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of physical minimum field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }

    if(is_number(scratchpad))
    {
      sprintf(txt_string, "Error, physical minimum field of signal %i is invalid: \"%s\".\n"
                          "You can try to fix it with the header editor, check the manual for the procedure.",
             i + 1,
             scratchpad);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    edfhdr->edfparam[i].phys_min = atof(scratchpad);
  }

/**************************** PHYSICAL MAXIMUMS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 112) + (i * 8), 8);
    scratchpad[8] = 0;

    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of physical maximum field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }

    if(is_number(scratchpad))
    {
      sprintf(txt_string, "Error, physical maximum field of signal %i is invalid: \"%s\".\n"
                          "You can try to fix it with the header editor, check the manual for the procedure.",
             i + 1,
             scratchpad);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    edfhdr->edfparam[i].phys_max = atof(scratchpad);
    if(edfhdr->edfparam[i].phys_max==edfhdr->edfparam[i].phys_min)
    {
      sprintf(txt_string, "Error, physical maximum of signal %i is equal to physical minimum",
             i + 1);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }

/**************************** DIGITAL MINIMUMS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 120) + (i * 8), 8);
    scratchpad[8] = 0;

    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of digital minimum field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }

    if(is_integer_number(scratchpad))
    {
      sprintf(txt_string, "Error, digital minimum field of signal %i is invalid: \"%s\".\n"
                          "Use the header editor to fix your file. Look at the manual for the details.",
             i + 1,
             scratchpad);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    n = atoi(scratchpad);
    if(edfhdr->edfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        if(n!=-32768)
        {
          sprintf(txt_string, "Error, digital minimum of signal %i is wrong: %i,"
                              "\ndigital minimum of an EDF Annotations signal should be -32768\n"
                              "Use the header editor to fix your file. Look at the manual for the details.",
                i + 1,
                n);
          free(edf_hdr);
          free(edfhdr->edfparam);
          free(edfhdr);
          return NULL;
        }
      }
    }
    if(edfhdr->bdfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        if(n!=-8388608)
        {
          sprintf(txt_string, "Error, digital minimum of signal %i is wrong: %i,"
                              "\ndigital minimum of a BDF Annotations signal should be -8388608\n"
                              "Use the header editor to fix your file. Look at the manual for the details.",
                i + 1,
                n);
          free(edf_hdr);
          free(edfhdr->edfparam);
          free(edfhdr);
          return NULL;
        }
      }
    }
    if(edfhdr->edf)
    {
      if((n>32767)||(n<-32768))
      {
        sprintf(txt_string, "Error, digital minimum of signal %i is out of range: %i,\nshould be in the range -32768 to 32767",
              i + 1,
              n);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    if(edfhdr->bdf)
    {
      if((n>8388607)||(n<-8388608))
      {
        sprintf(txt_string, "Error, digital minimum of signal %i is out of range: %i,\nshould be in the range -8388608 to 8388607",
              i + 1,
              n);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    edfhdr->edfparam[i].dig_min = n;
  }

/**************************** DIGITAL MAXIMUMS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 128) + (i * 8), 8);
    scratchpad[8] = 0;

    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of digital maximum field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }

    if(is_integer_number(scratchpad))
    {
      sprintf(txt_string, "Error, digital maximum field of signal %i is invalid: \"%s\".\n"
                          "Use the header editor to fix your file. Look at the manual for the details.",
             i + 1,
             scratchpad);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    n = atoi(scratchpad);
    if(edfhdr->edfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        if(n!=32767)
        {
          sprintf(txt_string, "Error, digital maximum of signal %i is wrong: %i,"
                              "\ndigital maximum of an EDF Annotations signal should be 32767\n"
                              "Use the header editor to fix your file. Look at the manual for the details.",
                i + 1,
                n);
          free(edf_hdr);
          free(edfhdr->edfparam);
          free(edfhdr);
          return NULL;
        }
      }
    }
    if(edfhdr->bdfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        if(n!=8388607)
        {
          sprintf(txt_string, "Error, digital maximum of signal %i is wrong: %i,"
                              "\ndigital maximum of a BDF Annotations signal should be 8388607\n"
                              "Use the header editor to fix your file. Look at the manual for the details.",
                i + 1,
                n);
          free(edf_hdr);
          free(edfhdr->edfparam);
          free(edfhdr);
          return NULL;
        }
      }
    }
    if(edfhdr->edf)
    {
      if((n>32767)||(n<-32768))
      {
        sprintf(txt_string, "Error, digital maximum of signal %i is out of range: %i,\nshould be in the range -32768 to 32767",
              i + 1,
              n);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    else
    {
      if((n>8388607)||(n<-8388608))
      {
        sprintf(txt_string, "Error, digital maximum of signal %i is out of range: %i,\nshould be in the range -8388608 to 8388607",
              i + 1,
              n);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    edfhdr->edfparam[i].dig_max = n;
    if(edfhdr->edfparam[i].dig_max<(edfhdr->edfparam[i].dig_min + 1))
    {
      sprintf(txt_string, "Error, digital maximum of signal %i is less than or equal to digital minimum.\n"
                          "Use the header editor to fix your file. Look at the manual for the details.",
             i + 1);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }

/**************************** PREFILTER FIELDS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 136) + (i * 80), 80);
    for(j=0; j<80; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of prefilter field of signal %i is not a valid 7-bit ASCII character.\n"
                            "Use the header editor to fix your file. Look at the manual for the details.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    strncpy(edfhdr->edfparam[i].prefilter, edf_hdr + 256 + (edfhdr->edfsignals * 136) + (i * 80), 80);
    edfhdr->edfparam[i].prefilter[80] = 0;

    if(edfhdr->edfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        for(j=0; j<80; j++)
        {
          if(edfhdr->edfparam[i].prefilter[j]!=' ')
          {
            sprintf(txt_string, "Error, prefilter field of EDF annotationchannel must be empty.\n"
                                "Use the header editor to fix your file. Look at the manual for the details.");
            free(edf_hdr);
            free(edfhdr->edfparam);
            free(edfhdr);
            return NULL;
          }
        }
      }
    }
    if(edfhdr->bdfplus)
    {
      if(edfhdr->edfparam[i].annotation)
      {
        for(j=0; j<80; j++)
        {
          if(edfhdr->edfparam[i].prefilter[j]!=' ')
          {
            sprintf(txt_string, "Error, prefilter field of BDF annotationchannel must be empty.\n"
                                "Use the header editor to fix your file. Look at the manual for the details.");
            free(edf_hdr);
            free(edfhdr->edfparam);
            free(edfhdr);
            return NULL;
          }
        }
      }
    }
  }

/*********************** NR OF SAMPLES IN EACH DATARECORD ********************/

  edfhdr->recordsize = 0;

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 216) + (i * 8), 8);
    scratchpad[8] = 0;

    for(j=0; j<8; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of number of samples field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }

    if(is_integer_number(scratchpad))
    {
      sprintf(txt_string, "Error, number of samples in datarecord field of signal %i is invalid: \"%s\".",
             i + 1,
             scratchpad);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    n = atoi(scratchpad);
    if(n<1)
    {
      sprintf(txt_string, "Error, number of samples in datarecord of signal %i is out of range: %i,\nshould be >0",
             i + 1,
             n);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
    edfhdr->edfparam[i].smp_per_record = n;
    edfhdr->recordsize += n;
  }

  if(edfhdr->bdf)
  {
    edfhdr->recordsize *= 3;

    if(edfhdr->recordsize > 15728640)
    {
      sprintf(txt_string, "Error, the datarecordsize is %i bytes. The datarecordsize should not exceed 15 MB.",
                          edfhdr->recordsize);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }
  else
  {
    edfhdr->recordsize *= 2;

    if(edfhdr->recordsize > 10485760)
    {
      sprintf(txt_string, "Error, the datarecordsize is %i bytes. The datarecordsize should not exceed 10 MB.",
                          edfhdr->recordsize);
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }

/**************************** RESERVED FIELDS *************************************/

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    strncpy(scratchpad, edf_hdr + 256 + (edfhdr->edfsignals * 224) + (i * 32), 32);
    for(j=0; j<32; j++)
    {
      if((scratchpad[j]<32)||(scratchpad[j]>126))
      {
        sprintf(txt_string, "Error, %ith character of reserved field of signal %i is not a valid 7-bit ASCII character.",
        j + 1,
        i + 1);
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }
    }
    strncpy(edfhdr->edfparam[i].reserved, edf_hdr + 256 + (edfhdr->edfsignals * 224) + (i * 32), 32);
    edfhdr->edfparam[i].reserved[32] = 0;
  }

/********************* EDF+ PATIENTNAME *********************************************/

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    error = 0;
    dotposition = 0;
    strncpy(scratchpad, edf_hdr + 8, 80);
    scratchpad[80] = 0;
    for(i=0; i<80; i++)
    {
      if(scratchpad[i]==' ')
      {
        dotposition = i;
        break;
      }
    }
    dotposition++;
    if((dotposition>73)||(dotposition<2))  error = 1;
    if(scratchpad[dotposition + 2]!='X')
    {
      if(dotposition>65)  error = 1;
    }
    if((scratchpad[dotposition]!='M')&&(scratchpad[dotposition]!='F')&&(scratchpad[dotposition]!='X'))  error = 1;
    dotposition++;
    if(scratchpad[dotposition]!=' ')  error = 1;
    if(scratchpad[dotposition + 1]=='X')
    {
      if(scratchpad[dotposition + 2]!=' ')  error = 1;
      if(scratchpad[dotposition + 3]==' ')  error = 1;
    }
    else
    {
      if(scratchpad[dotposition + 12]!=' ')  error = 1;
      if(scratchpad[dotposition + 13]==' ')  error = 1;
      dotposition++;
      strncpy(scratchpad2, scratchpad + dotposition, 11);
      scratchpad2[11] = 0;
      if((scratchpad2[2]!='-')||(scratchpad2[6]!='-'))  error = 1;
      scratchpad2[2] = 0;
      scratchpad2[6] = 0;
      if((scratchpad2[0]<48)||(scratchpad2[0]>57))  error = 1;
      if((scratchpad2[1]<48)||(scratchpad2[1]>57))  error = 1;
      if((scratchpad2[7]<48)||(scratchpad2[7]>57))  error = 1;
      if((scratchpad2[8]<48)||(scratchpad2[8]>57))  error = 1;
      if((scratchpad2[9]<48)||(scratchpad2[9]>57))  error = 1;
      if((scratchpad2[10]<48)||(scratchpad2[10]>57))  error = 1;
      if((atoi(scratchpad2)<1)||(atoi(scratchpad2)>31))  error = 1;
      if(strcmp(scratchpad2 + 3, "JAN"))
        if(strcmp(scratchpad2 + 3, "FEB"))
          if(strcmp(scratchpad2 + 3, "MAR"))
            if(strcmp(scratchpad2 + 3, "APR"))
              if(strcmp(scratchpad2 + 3, "MAY"))
                if(strcmp(scratchpad2 + 3, "JUN"))
                  if(strcmp(scratchpad2 + 3, "JUL"))
                    if(strcmp(scratchpad2 + 3, "AUG"))
                      if(strcmp(scratchpad2 + 3, "SEP"))
                        if(strcmp(scratchpad2 + 3, "OCT"))
                          if(strcmp(scratchpad2 + 3, "NOV"))
                            if(strcmp(scratchpad2 + 3, "DEC"))
                              error = 1;
    }

    if(error)
    {
      if(edfhdr->edfplus)
      {
        sprintf(txt_string, "Error, file is marked as EDF+ but local patient identification field does not\ncomply to the EDF+ standard:\n"
              "\"%.80s\"",
              edf_hdr+8);
      }
      if(edfhdr->bdfplus)
      {
        sprintf(txt_string, "Error, file is marked as BDF+ but local patient identification field does not\ncomply to the BDF+ standard:\n"
              "\"%.80s\"",
              edf_hdr+8);
      }
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }

    p = 0;
    if(edfhdr->patient[p]=='X')
    {
      edfhdr->plus_patientcode[0] = 0;
      p += 2;
    }
    else
    {
      for(i=0; i<(80-p); i++)
      {
        if(edfhdr->patient[i+p]==' ')
        {
          break;
        }
        edfhdr->plus_patientcode[i] = edfhdr->patient[i+p];
        if(edfhdr->plus_patientcode[i]=='_')  edfhdr->plus_patientcode[i] = ' ';
      }
      edfhdr->plus_patientcode[i] = 0;
      p += i + 1;
    }

    if(edfhdr->patient[p]=='M')
    {
      strcpy(edfhdr->plus_gender, "Male");
    }
    if(edfhdr->patient[p]=='F')
    {
      strcpy(edfhdr->plus_gender, "Female");
    }
    if(edfhdr->patient[p]=='X')
    {
      edfhdr->plus_gender[0] = 0;
    }
    for(i=0; i<(80-p);i++)
    {
      if(edfhdr->patient[i+p]==' ')
      {
        break;
      }
    }
    p += i + 1;

    if(edfhdr->patient[p]=='X')
    {
      edfhdr->plus_birthdate[0] = 0;
      p += 2;
    }
    else
    {
      for(i=0; i<(80-p); i++)
      {
        if(edfhdr->patient[i+p]==' ')
        {
          break;
        }
        edfhdr->plus_birthdate[i] = edfhdr->patient[i+p];
      }
      edfhdr->plus_birthdate[2] = ' ';
      edfhdr->plus_birthdate[3] += 32;
      edfhdr->plus_birthdate[4] += 32;
      edfhdr->plus_birthdate[5] += 32;
      edfhdr->plus_birthdate[6] = ' ';
      edfhdr->plus_birthdate[11] = 0;
      p += i + 1;
    }

    for(i=0; i<(80-p);i++)
    {
      if(edfhdr->patient[i+p]==' ')
      {
        break;
      }
      edfhdr->plus_patient_name[i] = edfhdr->patient[i+p];
      if(edfhdr->plus_patient_name[i]=='_')  edfhdr->plus_patient_name[i] = ' ';
    }
    edfhdr->plus_patient_name[i] = 0;
    p += i + 1;

    for(i=0; i<(80-p);i++)
    {
      edfhdr->plus_patient_additional[i] = edfhdr->patient[i+p];
    }
    edfhdr->plus_patient_additional[i] = 0;
    p += i + 1;
  }

/********************* EDF+ RECORDINGFIELD *********************************************/

  if(edfhdr->edfplus || edfhdr->bdfplus)
  {
    error = 0;
    strncpy(scratchpad, edf_hdr + 88, 80);
    scratchpad[80] = 0;
    if(strncmp(scratchpad, "Startdate ", 10))  error = 1;
    if(scratchpad[10]=='X')
    {
      if(scratchpad[11]!=' ')  error = 1;
      if(scratchpad[12]==' ')  error = 1;
      p = 12;
    }
    else
    {
      if(scratchpad[21]!=' ')  error = 1;
      if(scratchpad[22]==' ')  error = 1;
      p = 22;
      strncpy(scratchpad2, scratchpad + 10, 11);
      scratchpad2[11] = 0;
      if((scratchpad2[2]!='-')||(scratchpad2[6]!='-'))  error = 1;
      scratchpad2[2] = 0;
      scratchpad2[6] = 0;
      if((scratchpad2[0]<48)||(scratchpad2[0]>57))  error = 1;
      if((scratchpad2[1]<48)||(scratchpad2[1]>57))  error = 1;
      if((scratchpad2[7]<48)||(scratchpad2[7]>57))  error = 1;
      if((scratchpad2[8]<48)||(scratchpad2[8]>57))  error = 1;
      if((scratchpad2[9]<48)||(scratchpad2[9]>57))  error = 1;
      if((scratchpad2[10]<48)||(scratchpad2[10]>57))  error = 1;
      if((atoi(scratchpad2)<1)||(atoi(scratchpad2)>31))  error = 1;
      r = 0;
      if(!strcmp(scratchpad2 + 3, "JAN"))  r = 1;
        else if(!strcmp(scratchpad2 + 3, "FEB"))  r = 2;
          else if(!strcmp(scratchpad2 + 3, "MAR"))  r = 3;
            else if(!strcmp(scratchpad2 + 3, "APR"))  r = 4;
              else if(!strcmp(scratchpad2 + 3, "MAY"))  r = 5;
                else if(!strcmp(scratchpad2 + 3, "JUN"))  r = 6;
                  else if(!strcmp(scratchpad2 + 3, "JUL"))  r = 7;
                    else if(!strcmp(scratchpad2 + 3, "AUG"))  r = 8;
                      else if(!strcmp(scratchpad2 + 3, "SEP"))  r = 9;
                        else if(!strcmp(scratchpad2 + 3, "OCT"))  r = 10;
                          else if(!strcmp(scratchpad2 + 3, "NOV"))  r = 11;
                            else if(!strcmp(scratchpad2 + 3, "DEC"))  r = 12;
                              else error = 1;
    }

    n = 0;
    for(i=p; i<80; i++)
    {
      if(i>78)
      {
        error = 1;
        break;
      }
      if(scratchpad[i]==' ')
      {
        n++;
        if(scratchpad[i + 1]==' ')
        {
          error = 1;
          break;
        }
      }
      if(n>1)  break;
    }

    if(error)
    {
      if(edfhdr->edfplus)
      {
        sprintf(txt_string, "Error, file is marked as EDF+ but recording field does not comply to the\nEDF+ standard:\n"
              "\"%.80s\"",
              edf_hdr+88);
      }
      if(edfhdr->bdfplus)
      {
        sprintf(txt_string, "Error, file is marked as BDF+ but recording field does not comply to the\nBDF+ standard:\n"
              "\"%.80s\"",
              edf_hdr+88);
      }
      free(edf_hdr);
      free(edfhdr->edfparam);
      return NULL;
    }

    if(edf_hdr[98]!='X')
    {
      error = 0;

      strncpy(scratchpad, edf_hdr + 168, 8);
      scratchpad[2] = 0;
      scratchpad[5] = 0;
      scratchpad[8] = 0;

      if(atoi(scratchpad)!=atoi(scratchpad2))  error = 1;
      if(atoi(scratchpad+3)!=r)  error = 1;
      if(atoi(scratchpad+6)!=atoi(scratchpad2+9))  error = 1;
      if(error)
      {
        if(edfhdr->edfplus)
        {
          sprintf(txt_string, "Error, file is marked as EDF+ but startdate field does not match with startdate in\nrecordfield:\n"
                "\"%.8s\" <-> \"%.11s\".",
                edf_hdr+168,
                edf_hdr+98);
        }
        if(edfhdr->bdfplus)
        {
          sprintf(txt_string, "Error, file is marked as BDF+ but startdate field does not match with startdate in\nrecordfield:\n"
                "\"%.8s\" <-> \"%.11s\".",
                edf_hdr+168,
                edf_hdr+98);
        }
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }

      date_time.year = atoi(scratchpad2 + 7);

      if(date_time.year<1970)
      {
        sprintf(txt_string, "Error, recording startdate is older than 1970.");
        free(edf_hdr);
        free(edfhdr->edfparam);
        free(edfhdr);
        return NULL;
      }

      date_time_to_utc(&edfhdr->utc_starttime, date_time);
    }

    p = 10;
    for(i=0; i<(80-p); i++)
    {
      if(edfhdr->recording[i+p]==' ')
      {
        break;
      }
      edfhdr->plus_startdate[i] = edfhdr->recording[i+p];
    }
    edfhdr->plus_startdate[2] = ' ';
    edfhdr->plus_startdate[3] += 32;
    edfhdr->plus_startdate[4] += 32;
    edfhdr->plus_startdate[5] += 32;
    edfhdr->plus_startdate[6] = ' ';
    edfhdr->plus_startdate[11] = 0;
    p += i + 1;

    if(edfhdr->recording[p]=='X')
    {
      edfhdr->plus_admincode[0] = 0;
      p += 2;
    }
    else
    {
      for(i=0; i<(80-p); i++)
      {
        if(edfhdr->recording[i+p]==' ')
        {
          break;
        }
        edfhdr->plus_admincode[i] = edfhdr->recording[i+p];
        if(edfhdr->plus_admincode[i]=='_')  edfhdr->plus_admincode[i] = ' ';
      }
      edfhdr->plus_admincode[i] = 0;
      p += i + 1;
    }

    if(edfhdr->recording[p]=='X')
    {
      edfhdr->plus_technician[0] = 0;
      p += 2;
    }
    else
    {
      for(i=0; i<(80-p); i++)
      {
        if(edfhdr->recording[i+p]==' ')
        {
          break;
        }
        edfhdr->plus_technician[i] = edfhdr->recording[i+p];
        if(edfhdr->plus_technician[i]=='_')  edfhdr->plus_technician[i] = ' ';
      }
      edfhdr->plus_technician[i] = 0;
      p += i + 1;
    }

    if(edfhdr->recording[p]=='X')
    {
      edfhdr->plus_equipment[0] = 0;
      p += 2;
    }
    else
    {
      for(i=0; i<(80-p); i++)
      {
        if(edfhdr->recording[i+p]==' ')
        {
          break;
        }
        edfhdr->plus_equipment[i] = edfhdr->recording[i+p];
        if(edfhdr->plus_equipment[i]=='_')  edfhdr->plus_equipment[i] = ' ';
      }
      edfhdr->plus_equipment[i] = 0;
      p += i + 1;
    }

    for(i=0; i<(80-p);i++)
    {
      edfhdr->plus_recording_additional[i] = edfhdr->recording[i+p];
    }
    edfhdr->plus_recording_additional[i] = 0;
    p += i + 1;
  }

/********************* FILESIZE *********************************************/

  edfhdr->hdrsize = edfhdr->edfsignals * 256 + 256;

  if(live_stream)
  {
    fseeko(inputfile, 0LL, SEEK_END);

    edfhdr->datarecords = ftello(inputfile);

    edfhdr->datarecords -= (long long)edfhdr->hdrsize;

    edfhdr->datarecords /= (long long)edfhdr->recordsize;
  }
  else
  {
    l_tmp = edfhdr->recordsize;
    l_tmp *= edfhdr->datarecords;
    l_tmp += (edfhdr->edfsignals * 256 + 256);

    fseeko(inputfile, 0LL, SEEK_END);
    l_tmp2 = ftello(inputfile);

    if(l_tmp != l_tmp2)
    {
#ifdef Q_OS_WIN32
      __mingw_sprintf(txt_string, "Error, filesize does not match with the calculated filesize based on the parameters\n"
                          "in the header. Filesize is %lli and filesize according to header is %lli.\n"
                          "You can fix this problem with the header editor, check the manual for details.",
                          l_tmp2, l_tmp);
#else
      sprintf(txt_string, "Error, filesize does not match with the calculated filesize based on the parameters\n"
                          "in the header. Filesize is %lli and filesize according to header is %lli.\n"
                          "You can fix this problem with the header editor, check the manual for details.",
                          l_tmp2, l_tmp);
#endif
      free(edf_hdr);
      free(edfhdr->edfparam);
      free(edfhdr);
      return NULL;
    }
  }

  n = 0;

  for(i=0; i<edfhdr->edfsignals; i++)
  {
    edfhdr->edfparam[i].buf_offset = n;
    if(edfhdr->bdf)  n += edfhdr->edfparam[i].smp_per_record * 3;
    else  n += edfhdr->edfparam[i].smp_per_record * 2;

    edfhdr->edfparam[i].bitvalue = (edfhdr->edfparam[i].phys_max - edfhdr->edfparam[i].phys_min) / (edfhdr->edfparam[i].dig_max - edfhdr->edfparam[i].dig_min);
    edfhdr->edfparam[i].offset = edfhdr->edfparam[i].phys_max / edfhdr->edfparam[i].bitvalue - edfhdr->edfparam[i].dig_max;
  }

  edfhdr->recording_len_sec = (int)((edfhdr->datarecords * edfhdr->long_data_record_duration) / TIME_DIMENSION);

  txt_string[0] = 0;

  free(edf_hdr);

  return edfhdr;
}




int EDFfileCheck::is_integer_number(char *str)
{
  int i=0, l, hasspace = 0, hassign=0, digit=0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0]=='+')||(str[0]=='-'))
  {
    hassign++;
    i++;
  }

  for(; i<l; i++)
  {
    if(str[i]==' ')
    {
      if(!digit)
      {
        return 1;
      }
      hasspace++;
    }
    else
    {
      if((str[i]<48)||(str[i]>57))
      {
        return 1;
      }
      else
      {
        if(hasspace)
        {
          return 1;
        }
        digit++;
      }
    }
  }

  if(digit)  return 0;
  else  return 1;
}



int EDFfileCheck::is_number(char *str)
{
  int i=0, l, hasspace = 0, hassign=0, digit=0, hasdot=0, hasexp=0;

  l = strlen(str);

  if(!l)  return 1;

  if((str[0]=='+')||(str[0]=='-'))
  {
    hassign++;
    i++;
  }

  for(; i<l; i++)
  {
    if((str[i]=='e')||(str[i]=='E'))
    {
      if((!digit)||hasexp)
      {
        return 1;
      }
      hasexp++;
      hassign = 0;
      digit = 0;

      break;
    }

    if(str[i]==' ')
    {
      if(!digit)
      {
        return 1;
      }
      hasspace++;
    }
    else
    {
      if(((str[i]<48)||(str[i]>57))&&str[i]!='.')
      {
        return 1;
      }
      else
      {
        if(hasspace)
        {
          return 1;
        }
        if(str[i]=='.')
        {
          if(hasdot)  return 1;
          hasdot++;
        }
        else
        {
          digit++;
        }
      }
    }
  }

  if(hasexp)
  {
    if(++i==l)
    {
      return 1;
    }

    if((str[i]=='+')||(str[i]=='-'))
    {
      hassign++;
      i++;
    }

    for(; i<l; i++)
    {
      if(str[i]==' ')
      {
        if(!digit)
        {
          return 1;
        }
        hasspace++;
      }
      else
      {
        if((str[i]<48)||(str[i]>57))
        {
          return 1;
        }
        else
        {
          if(hasspace)
          {
            return 1;
          }

          digit++;
        }
      }
    }
  }

  if(digit)  return 0;
  else  return 1;
}


long long EDFfileCheck::get_long_duration(char *str)
{
  int i, len=8, hasdot=0, dotposition=0;

  long long value=0, radix;

  if((str[0] == '+') || (str[0] == '-'))
  {
    for(i=0; i<7; i++)
    {
      str[i] = str[i+1];
    }

    str[7] = ' ';
  }

  for(i=0; i<8; i++)
  {
    if(str[i]==' ')
    {
      len = i;
      break;
    }
  }

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      hasdot = 1;
      dotposition = i;
      break;
    }
  }

  if(hasdot)
  {
    radix = TIME_DIMENSION;

    for(i=dotposition-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }

    radix = TIME_DIMENSION / 10;

    for(i=dotposition+1; i<len; i++)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix /= 10;
    }
  }
  else
  {
    radix = TIME_DIMENSION;

    for(i=len-1; i>=0; i--)
    {
        value += ((long long)(str[i] - 48)) * radix;
        radix *= 10;
    }
  }

  return value;
}



