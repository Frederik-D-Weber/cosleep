/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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




#include "scp_ecg2edf.h"


#define SPCECGBUFSIZE (1024 * 1024)

// #define SPCECG_DEBUG



UI_SCPECG2EDFwindow::UI_SCPECG2EDFwindow(char *recent_dir, char *save_dir)
{
  char txt_string[2048];

  recent_opendir = recent_dir;
  recent_savedir = save_dir;

  myobjectDialog = new QDialog;

  myobjectDialog->setMinimumSize(600, 480);
  myobjectDialog->setMaximumSize(600, 480);
  myobjectDialog->setWindowTitle("SCP-ECG to EDF+ converter");
  myobjectDialog->setModal(true);
  myobjectDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  pushButton1 = new QPushButton(myobjectDialog);
  pushButton1->setGeometry(20, 430, 100, 25);
  pushButton1->setText("Select File");

  pushButton2 = new QPushButton(myobjectDialog);
  pushButton2->setGeometry(480, 430, 100, 25);
  pushButton2->setText("Close");

  textEdit1 = new QTextEdit(myobjectDialog);
  textEdit1->setGeometry(20, 20, 560, 380);
  textEdit1->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  textEdit1->setReadOnly(true);
  textEdit1->setLineWrapMode(QTextEdit::NoWrap);
  sprintf(txt_string, "SCP-ECG to EDF+ converter.\n");
  textEdit1->append(txt_string);

  crc_ccitt_init();

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(SelectFileButton()));
  QObject::connect(pushButton2, SIGNAL(clicked()), myobjectDialog, SLOT(close()));

  myobjectDialog->exec();
}


void UI_SCPECG2EDFwindow::SelectFileButton()
{
  FILE *inputfile=NULL;

  int i, j, n,
      hdl=-1,
      blocks,
      *buf[SCP_ECG_MAX_CHNS],
      *buf2[SCP_ECG_MAX_CHNS],
      offset,
      *ptr,
      total_samples;

  unsigned short sh_tmp;

  long long filesize, ll_tmp;

  char input_filename[MAX_PATH_LENGTH],
       txt_string[2048],
       edf_filename[MAX_PATH_LENGTH],
       scratchpad[MAX_PATH_LENGTH],
       *block=NULL,
       *ch_ptr;

  pushButton1->setEnabled(false);

  for(i=0; i<SCP_ECG_MAX_CHNS; i++)
  {
    qrs_data.ref_beat[i] = NULL;

    buf[i] = NULL;

    buf2[i] = NULL;
  }

  memset(&scp_ecg, 0, sizeof(struct scp_ecg_prop_struct));

  scp_ecg.sf_prot = 1;
  scp_ecg.sf_ratio = 1;
  scp_ecg.ref_beat_subtract = 0;
  scp_ecg.avm_ratio = 1;

  strcpy(input_filename, QFileDialog::getOpenFileName(0, "Select inputfile", QString::fromLocal8Bit(recent_opendir), "SCP files (*.scp *.SCP)").toLocal8Bit().data());

  if(!strcmp(input_filename, ""))
  {
    goto EXIT_1;
  }

  get_directory_from_path(recent_opendir, input_filename, MAX_PATH_LENGTH);

  inputfile = fopeno(input_filename, "rb");
  if(inputfile==NULL)
  {
    snprintf(txt_string, 2048, "Can not open file %s for reading.\n", input_filename);
    textEdit1->append(QString::fromLocal8Bit(txt_string));
    goto EXIT_1;
  }

  get_filename_from_path(scratchpad, input_filename, MAX_PATH_LENGTH);

  snprintf(txt_string, 2048, "Read file: %s", scratchpad);
  textEdit1->append(QString::fromLocal8Bit(txt_string));

  fseeko(inputfile, 0LL, SEEK_END);
  filesize = ftello(inputfile);
  if(filesize<126)
  {
    textEdit1->append("Error, filesize is too small.\n");
    goto EXIT_2;
  }

  ll_tmp = 0LL;

  fseeko(inputfile, 2LL, SEEK_SET);

  if(fread(&ll_tmp, 4, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (1)\n");
    goto EXIT_2;
  }

  if(ll_tmp != filesize)
  {
    textEdit1->append("Error, filesize does not match with header.\n");
    goto EXIT_2;
  }

  block = (char *)malloc(SPCECGBUFSIZE);
  if(block == NULL)
  {
    textEdit1->append("Malloc error (block 1)\n");
    goto EXIT_2;
  }

  rewind(inputfile);

  if(fread(&sh_tmp, 2, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (2)\n");
    goto EXIT_3;
  }

  if(check_crc(inputfile, 2LL, filesize - 2LL, sh_tmp, block))
  {
    goto EXIT_3;
  }

  memset(&sp, 0, sizeof(struct section_prop_struct[12]));

  if(read_section_header(0, inputfile, 6LL, block))
  {
    goto EXIT_3;
  }

  if(strncmp(sp[0].reserved, "SCPECG", 6))
  {
    textEdit1->append("Error, reserved field of section header 0 does not contain string \"SCPECG\".\n");
    goto EXIT_3;
  }

  sp[0].file_offset = 6LL;

#ifdef SPCECG_DEBUG
  printf("\nsection ID is %i\n"
        "section file offset is %lli\n"
        "section CRC is 0x%04X\n"
        "section length is %i\n"
        "section version is %i\n"
        "section protocol version is %i\n",
        sp[0].section_id,
        sp[0].file_offset,
        (int)sp[0].crc,
        sp[0].section_length,
        sp[0].section_version,
        sp[0].section_protocol_version);
#endif

  if(read_data_section_zero(inputfile, block, filesize))
  {
    goto EXIT_3;
  }

/**************** SECTION 6 ECG rhythm data *********************************/

  fseeko(inputfile, sp[6].file_offset + 16LL, SEEK_SET);

  if(fread(block, 6, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred\n");
    goto EXIT_3;
  }

  scp_ecg.avm = *((unsigned short *)block);

  scp_ecg.sf = 1000000 / *((unsigned short *)(block + 2));

  scp_ecg.encoding = *((unsigned char *)(block + 4));

  scp_ecg.bimodal = *((unsigned char *)(block + 5));

  if(scp_ecg.bimodal)
  {
    if(sp[4].present != 1)
    {
      textEdit1->append("File contains bimodal compressed data but section 4 is not present.\n ");
      goto EXIT_3;
    }

    if(sp[5].present != 1)
    {
      textEdit1->append("File contains bimodal compressed data but section 5 is not present.\n ");
      goto EXIT_3;
    }
  }

/******************* SECTION 3 ECG lead definition **************************/

  fseeko(inputfile, sp[3].file_offset + 16LL, SEEK_SET);

  if(fread(scratchpad, 2, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (40)\n");
    goto EXIT_3;
  }

  scp_ecg.chns = *((unsigned char *)scratchpad);

  if(scp_ecg.chns < 1)
  {
    textEdit1->append("Error, number of signals is less than one.\n ");
    goto EXIT_3;
  }

  if(scp_ecg.chns > SCP_ECG_MAX_CHNS)
  {
    textEdit1->append("Error, number of signals is more than SCP_ECG_MAX_CHNS.\n ");
    goto EXIT_3;
  }

  if(scratchpad[1] & 1)
  {
    scp_ecg.ref_beat_subtract = 1;
  }
  else
  {
    scp_ecg.ref_beat_subtract = 0;
  }

  if(!(scratchpad[1] & 4))
  {
    textEdit1->append("Leads are not simultaneously recorded which is not supported by this converter. (1)\n ");
    goto EXIT_3;
  }

#ifdef SPCECG_DEBUG
  printf("chns is %u   avm is %u   sf is %u   encoding is %u   bimodal-compression is %u  ref_beat_subtract is %u\n",
        scp_ecg.chns, scp_ecg.avm, scp_ecg.sf, scp_ecg.encoding, scp_ecg.bimodal, scp_ecg.ref_beat_subtract);
#endif

  memset(&lp, 0, sizeof(struct lead_prop_struct[256]));

  fseeko(inputfile, sp[3].file_offset + 18LL, SEEK_SET);

  for(i=0; i<scp_ecg.chns; i++)
  {
    if(fread(&(lp[i].start), 4, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (30)\n");
      goto EXIT_3;
    }

    if(fread(&(lp[i].end), 4, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (31)\n");
      goto EXIT_3;
    }

    if(fread(&(lp[i].label), 1, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (32)\n");
      goto EXIT_3;
    }
  }

#ifdef SPCECG_DEBUG
  for(i=0; i<scp_ecg.chns; i++)
  {
    printf("lp[i].start is %i   lp[i].end is %i\n", lp[i].start, lp[i].end);
  }
#endif

  if(scp_ecg.chns > 1)
  {
    for(i=1; i<scp_ecg.chns; i++)
    {
      if(lp[i].start != lp[0].start)
      {
        textEdit1->append("Error, leads are not simultaneously recorded. (2)\n");
        goto EXIT_3;
      }

      if(lp[i].end != lp[0].end)
      {
        textEdit1->append("Error, leads are not simultaneously recorded. (3)\n");
        goto EXIT_3;
      }
    }
  }

  for(i=0; i<scp_ecg.chns; i++)
  {
    if(lp[i].start < 1)
    {
      textEdit1->append("Error, start sample number in section 3 is less than 1.\n");
      goto EXIT_3;
    }

    if(lp[i].end <= lp[i].start)
    {
      textEdit1->append("Error (56) (lp[i].end <= lp[i].start)\n");
      goto EXIT_3;
    }

    if(lp[i].start != 1)
    {
      textEdit1->append("Error (57) (lp[i].start != 1)\n");
      goto EXIT_3;
    }

    lp[i].start--;

    if((lp[i].end - lp[i].start) < scp_ecg.sf)
    {
      textEdit1->append("Error, recording length is less than one second.\n");
      goto EXIT_3;
    }

    lp[i].samples = lp[i].end - lp[i].start;
  }

/**************** SECTION 6 rhythm data *****************************/

  fseeko(inputfile, sp[6].file_offset + 16LL + 6LL, SEEK_SET);

  n = 0;

  for(i=0; i<scp_ecg.chns; i++)
  {
    if(fread(block, 2, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (41)\n");
      goto EXIT_3;
    }

    lp[i].bytes = *((unsigned short *)block);

    n += lp[i].bytes;

#ifdef SPCECG_DEBUG
    printf("lead samples is %i   bytes is %i\n", lp[i].samples, lp[i].bytes);
#endif

    if(sp[2].present != 1)  // huffmantable
    {
      if(lp[i].bytes < (lp[i].samples * 2))
      {
        textEdit1->append("Error, lead samples is less than lead bytes.\n");
        goto EXIT_3;
      }
    }
  }

  if(n > (sp[6].section_length - 22 - (scp_ecg.chns * 2)))
  {
    textEdit1->append("Error, total databytes is more than section size.\n");
    goto EXIT_3;
  }

/******************************** Section 2 Huffman tables *************************/

  if(sp[2].present == 1)
  {
    scp_ecg.huffman_enc = 1;

    fseeko(inputfile, sp[2].file_offset + 16LL, SEEK_SET);

    if(fread(block, 13, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (50)\n");
      goto EXIT_3;
    }

    ht.h_tables_cnt = *((unsigned short *)block);
    ht.code_structs_cnt = *((unsigned short *)(block + 2));
    ht.prefix_bits = *((unsigned char *)(block + 4));
    ht.total_bits = *((unsigned char *)(block + 5));
    ht.table_mode_switch = *((unsigned char *)(block + 6));
    ht.base_value = *((unsigned short *)(block + 7));
    ht.base_code = *((unsigned int *)(block + 9));

#ifdef SPCECG_DEBUG
    printf("ht.h_tables_cnt is %i   ht.code_structs_cnt is %i   ht.prefix_bits is %i   ht.total_bits is %i\n",
          ht.h_tables_cnt, ht.code_structs_cnt, ht.prefix_bits, ht.total_bits);
#endif

    if(ht.h_tables_cnt != 19999)
    {
      textEdit1->append("Aborted, this converter does not support customized Huffman decoding tables.\n");
      goto EXIT_3;
    }
  }

/********************************* SECTION 4 protected areas ****************************/

  if(scp_ecg.bimodal || scp_ecg.ref_beat_subtract)
  {
    memset(&qrs_data, 0, sizeof(struct qrs_loc_data_struct));

    fseeko(inputfile, sp[4].file_offset + 16LL, SEEK_SET);

    if(fread(block, 6, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred\n");
      goto EXIT_3;
    }

    qrs_data.ref_beat_length = *((unsigned short *)block);

    qrs_data.fiducial_tp = *((unsigned short *)(block + 2));

    qrs_data.n_qrs = *((unsigned short *)(block + 4));
    if(qrs_data.n_qrs > 1024)
    {
      textEdit1->append("Error, there are more than 1024 QRS complexes in this record.\n");
      goto EXIT_3;
    }

#ifdef SPCECG_DEBUG
    printf("number of QRS complexes: %i\n", qrs_data.n_qrs);
#endif

    fseeko(inputfile, sp[4].file_offset + 16LL + 6LL, SEEK_SET);

    if(fread(block, qrs_data.n_qrs * 14, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred\n");
      goto EXIT_3;
    }

    ch_ptr = block;

    for(i=0; i<qrs_data.n_qrs; i++)
    {
      qrs_data.qrs_subtr_type[i] = *((unsigned short *)ch_ptr);

      ch_ptr += 2;

      qrs_data.qrs_subtr_start[i] = *((unsigned int *)ch_ptr);

      ch_ptr += 4;

      qrs_data.qrs_subtr_fiducial[i] = *((unsigned int *)ch_ptr);

      ch_ptr += 4;

      qrs_data.qrs_subtr_end[i] = *((unsigned int *)ch_ptr);

      ch_ptr += 4;

#ifdef SPCECG_DEBUG
      printf("QRS subtraction type: %i   start: %4i   fiducial: %4i   end: %4i\n",
             qrs_data.qrs_subtr_type[i], qrs_data.qrs_subtr_start[i], qrs_data.qrs_subtr_fiducial[i], qrs_data.qrs_subtr_end[i]);
#endif
    }

    for(i=0; i<qrs_data.n_qrs; i++)
    {
      if((qrs_data.qrs_subtr_start[i]    <  1)                           ||
         (qrs_data.qrs_subtr_start[i]    >= qrs_data.qrs_subtr_end[i])   ||
         (qrs_data.qrs_subtr_fiducial[i] <  qrs_data.qrs_subtr_start[i]) ||
         (qrs_data.qrs_subtr_fiducial[i] >  qrs_data.qrs_subtr_end[i]))
      {
        if(qrs_data.qrs_subtr_type[i] == 0)
        {
          textEdit1->append("QRS subtraction: illegal value for start-, end- or fiducial-pointer.\n");
#ifdef SPCECG_DEBUG
          printf("illegal value for start-, end- or fiducial-pointer: channel %i\n", i);
#endif
          goto EXIT_3;
        }
      }

      if(i)
      {
        if(qrs_data.qrs_subtr_start[i] < qrs_data.qrs_subtr_end[i-1])
        {
          if(qrs_data.qrs_subtr_type[i] == 0)
          {
            textEdit1->append("QRS subtraction: illegal value for start- or end-pointer.\n");
#ifdef SPCECG_DEBUG
            printf("illegal value for start- or end-pointer: channel %i\n", i);
#endif
            goto EXIT_3;
          }
        }
      }
    }

    fseeko(inputfile, sp[4].file_offset + 16LL + 6LL + (qrs_data.n_qrs * 14LL), SEEK_SET);

    if(fread(block, qrs_data.n_qrs * 8, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred\n");
      goto EXIT_3;
    }

    ptr = (int *)block;

    for(i=0; i<qrs_data.n_qrs; i++)
    {
      qrs_data.qrs_prot_start[i] = *(ptr++);

      qrs_data.qrs_prot_end[i] = *(ptr++);

#ifdef SPCECG_DEBUG
      printf("QRS protected area start: %4i   QRS protected area end: %4i\n",
             qrs_data.qrs_prot_start[i], qrs_data.qrs_prot_end[i]);
#endif
    }
  }

/********************************* SECTION 5 referenced beat data ****************************/

  if(scp_ecg.ref_beat_subtract)
  {
    fseeko(inputfile, sp[5].file_offset + 16LL, SEEK_SET);

    if(fread(block, 6 + (scp_ecg.chns * 2), 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred\n");
      goto EXIT_3;
    }

    scp_ecg.avm_prot = *((unsigned short *)block);

    scp_ecg.sf_prot = 1000000 / *((unsigned short *)(block + 2));

    qrs_data.ref_beat_length *= 1000;

    qrs_data.ref_beat_length /= *((unsigned short *)(block + 2));

    scp_ecg.encoding_prot = *((unsigned char *)(block + 4));

    scp_ecg.sf_ratio = scp_ecg.sf_prot / scp_ecg.sf;

    scp_ecg.avm_ratio = (double)scp_ecg.avm / (double)scp_ecg.avm_prot;

#ifdef SPCECG_DEBUG
    printf("avm (prot) is %u   sf (prot) is %u   encoding (prot) is %u   sf ratio is %u   avm ratio is %f\n",
          scp_ecg.avm_prot, scp_ecg.sf_prot, scp_ecg.encoding_prot, scp_ecg.sf_ratio, scp_ecg.avm_ratio);

    printf("reference beat:   length is: %i   fiducial is: %i\n",
           qrs_data.ref_beat_length, qrs_data.fiducial_tp);
#endif

    if(qrs_data.fiducial_tp > qrs_data.ref_beat_length)
    {
      textEdit1->append("Error, fiducial position > reference beat length.\n");
      goto EXIT_3;
    }

    if(scp_ecg.bimodal)
    {
      if(scp_ecg.sf >= scp_ecg.sf_prot)
      {
        textEdit1->append("Error, SF >= SF in protected area.\n");
        goto EXIT_3;
      }

      if(scp_ecg.sf_prot % scp_ecg.sf)
      {
        textEdit1->append("Error, ratio SF:SF in protected area is not an integer.\n");
        goto EXIT_3;
      }

      for(i=0; i<qrs_data.n_qrs; i++)
      {
#ifdef SPCECG_DEBUG
        if(!i)
        {
          if((qrs_data.qrs_prot_start[i] - 1) % scp_ecg.sf_ratio)
          {
            printf("Warning, number of samples in the non-protected area is not a multple of SF ratio. (5-10)\n");
          }
        }
        else
        {
          if(((qrs_data.qrs_prot_start[i] - 1) - qrs_data.qrs_prot_end[i-1]) % scp_ecg.sf_ratio)
          {
            printf("Warning, number of samples in the non-protected area is not a multple of SF ratio. (5-12)\n");
          }
        }
#endif

        for(j=0; j<scp_ecg.chns; j++)
        {
          if(!i)
          {
            lp[j].bimod_samples = (qrs_data.qrs_prot_start[i] - 1) / scp_ecg.sf_ratio;
          }
          else
          {
            lp[j].bimod_samples += ((qrs_data.qrs_prot_start[i] - 1) - qrs_data.qrs_prot_end[i-1]) / scp_ecg.sf_ratio;
          }

          lp[j].bimod_samples += ((qrs_data.qrs_prot_end[i] - qrs_data.qrs_prot_start[i]) + 1);
        }
      }
#ifdef SPCECG_DEBUG
      for(j=0; j<scp_ecg.chns; j++)
      {
        printf("bimodal samples is %i\n", lp[j].bimod_samples);
      }
#endif
    }

    for(j=0; j<scp_ecg.chns; j++)
    {
      qrs_data.ref_beat_bytes[j] = *((unsigned short *)(block + 6 + (j * 2)));

#ifdef SPCECG_DEBUG
      printf("ref beat 0 lead length is %i\n", qrs_data.ref_beat_bytes[j]);
#endif
    }

    fseeko(inputfile, sp[5].file_offset + 16LL + 6LL + (scp_ecg.chns * 2LL), SEEK_SET);

    if(scp_ecg.ref_beat_subtract)
    {
      for(j=0; j<scp_ecg.chns; j++)
      {
        /*  16 is an arbitrary value because we don't know in advance how much the data
         *  will expand by the huffman decoder, 16 seems to be a safe value
         */
        qrs_data.ref_beat[j] = (int *)malloc(qrs_data.ref_beat_bytes[j] * 16 * sizeof(int));
        if(qrs_data.ref_beat[j] == NULL)
        {
          textEdit1->append("Malloc error (reference beat data).\n");
          goto EXIT_3;
        }

        if(fread(block, qrs_data.ref_beat_bytes[j], 1, inputfile) != 1)
        {
          textEdit1->append("A read-error occurred (5-1)\n");
          goto EXIT_3;
        }

        if(scp_ecg.huffman_enc)
        {
          qrs_data.huffman_decoder_produced_samples[j] = default_huffman_decoding(block, qrs_data.ref_beat[j], qrs_data.ref_beat_bytes[j], qrs_data.ref_beat_bytes[j] * 16);

          if(scp_ecg.encoding_prot == 1)  // reconstitution of the data from the first differences
          {
            reconstitute_data_first_diff(qrs_data.ref_beat[j], qrs_data.huffman_decoder_produced_samples[j]);
          }

          if(scp_ecg.encoding_prot == 2)  // reconstitution of the data from the second differences
          {
            reconstitute_data_second_diff(qrs_data.ref_beat[j], qrs_data.huffman_decoder_produced_samples[j]);
          }
        }
        else
        {
          textEdit1->append("Encountered an unexpected situation: reference beat subtraction without huffman encoding.\n");
          goto EXIT_3;
        }
      }
    }
  }

//////////////////////////////// patient data ////////////////////

  memset(&pat_dat, 0, sizeof(struct patient_data_struct));

  if(get_patient_data(inputfile))
  {
    goto EXIT_3;
  }

#ifdef SPCECG_DEBUG
  printf("patient ID:          %s\n"
        "patient lastname:    %s\n"
        "patient firstname:   %s\n"
        "patient sex:         %i\n"
        "startdate year:      %i\n"
        "startdate month:     %i\n"
        "startdate day:       %i\n"
        "starttime hour:      %i\n"
        "starttime minute:    %i\n"
        "starttime second:    %i\n"
        "birthdate year:      %i\n"
        "birthdate month:     %i\n"
        "birthdate day:       %i\n"
        "device model:        %s\n"
        "language code:       %u\n"
        "manufacturer:        %s\n",
        pat_dat.pat_id,
        pat_dat.last_name,
        pat_dat.first_name,
        pat_dat.sex,
        pat_dat.startdate_year,
        pat_dat.startdate_month,
        pat_dat.startdate_day,
        pat_dat.starttime_hour,
        pat_dat.starttime_minute,
        pat_dat.starttime_second,
        pat_dat.birthdate_year,
        pat_dat.birthdate_month,
        pat_dat.birthdate_day,
        pat_dat.device_model,
        pat_dat.lang_code,
        pat_dat.manufacturer);
#endif

////////////////////////// start conversion ///////////////////////////////

  free(block);
  block = NULL;

  if(scp_ecg.bimodal)
  {
    scp_ecg.sf = scp_ecg.sf_prot;

    scp_ecg.avm = scp_ecg.avm_prot;
  }

  n = 0;

  for(i=0; i<scp_ecg.chns; i++)
  {
    buf[i] = NULL;
  }

  for(i=0; i<scp_ecg.chns; i++)
  {
    n += lp[i].bytes;

    buf[i] = (int *)malloc(((lp[0].samples / scp_ecg.sf) + 2) * scp_ecg.sf * sizeof(int));
    if(buf[i] == NULL)
    {
      textEdit1->append("Malloc error (buf)\n");
      goto EXIT_4;
    }
  }

  block = (char *)calloc(1, n + 4096);
  if(block == NULL)
  {
    textEdit1->append("Malloc error (block 3)\n");
    goto EXIT_4;
  }

  fseeko(inputfile, sp[6].file_offset + 16LL + 6LL + (scp_ecg.chns * 2LL), SEEK_SET);  // rhythm data

  if(fread(block, n, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred during conversion (70)\n");
    goto EXIT_4;
  }

  offset = 0;

  for(j=0; j<scp_ecg.chns; j++)
  {
    if(j > 0)
    {
      offset += lp[j - 1].bytes;
    }

    if(scp_ecg.bimodal)
    {
      total_samples = lp[j].bimod_samples;
    }
    else
    {
      total_samples = lp[j].samples;
    }

    if(scp_ecg.huffman_enc)  // huffman table present
    {
      lp[j].huffman_decoder_produced_samples = default_huffman_decoding(block + offset, buf[j], lp[j].bytes, total_samples);
      if((lp[j].huffman_decoder_produced_samples < lp[j].bytes) || (lp[j].huffman_decoder_produced_samples > lp[j].samples))
      {
        textEdit1->append("Error during Huffman decoding.\n ");
        goto EXIT_4;
      }

      if(scp_ecg.encoding == 1)  // reconstitution of the data from the first differences
      {
        reconstitute_data_first_diff(buf[j], lp[j].huffman_decoder_produced_samples);
      }

      if(scp_ecg.encoding == 2)  // reconstitution of the data from the second differences
      {
        reconstitute_data_second_diff(buf[j], lp[j].huffman_decoder_produced_samples);
      }
    }
    else  // huffman table not present
    {
      for(i=0; i<total_samples; i++)
      {
        buf[j][i] = ((signed short *)(block + offset))[i];
      }

      if(scp_ecg.encoding == 1)  // reconstitution of the data from the first differences
      {
        reconstitute_data_first_diff(buf[j], total_samples);
      }

      if(scp_ecg.encoding == 2)  // reconstitution of the data from the second differences
      {
        reconstitute_data_second_diff(buf[j], total_samples);
      }
    }
  }

  if(scp_ecg.bimodal || scp_ecg.ref_beat_subtract)
  {

    for(i=0; i<scp_ecg.chns; i++)
    {
      buf2[i] = (int *)malloc(((lp[0].samples / scp_ecg.sf) + 2) * scp_ecg.sf * sizeof(int));
      if(buf2[i] == NULL)
      {
        textEdit1->append("Malloc error (buf2)\n");
        goto EXIT_4;
      }
    }

    if(scp_ecg.bimodal)
    {
      for(i=0; i<scp_ecg.chns; i++)
      {
        if(reconstitute_decimated_samples(buf[i], buf2[i], i))
        {
          textEdit1->append("Error during bimodal decompression.\n ");
          goto EXIT_4;
        }
      }
    }
    else
    {
      for(i=0; i<scp_ecg.chns; i++)
      {
        for(j=0; j<lp[i].samples; j++)
        {
          buf2[i][j] = buf[i][j];
        }
      }
    }

    int k, p, add_len;

    if(scp_ecg.ref_beat_subtract)
    {
      for(j=0; j<qrs_data.n_qrs; j++)
      {
        if(qrs_data.qrs_subtr_type[j] == 0)
        {
          add_len = qrs_data.qrs_subtr_end[j] - qrs_data.qrs_subtr_start[j] + 1;

          p = qrs_data.fiducial_tp - 1;

          p -= (qrs_data.qrs_subtr_fiducial[j] - qrs_data.qrs_subtr_start[j]);

          if(p < 0)
          {
            textEdit1->append("Error: illegal reference beat pointer value.\n ");
            goto EXIT_4;
          }

          for(i=0; i<scp_ecg.chns; i++)
          {
            if(add_len > qrs_data.huffman_decoder_produced_samples[i])
            {
              textEdit1->append("Error: subtraction length > reference beat length.\n ");
              goto EXIT_4;
            }

            for(k=0; k<add_len; k++)
            {
              buf2[i][k + qrs_data.qrs_subtr_start[j] - 1] += qrs_data.ref_beat[i][k + p];
            }
          }
        }
      }

      for(i=0; i<scp_ecg.chns; i++)
      {
        smooth_subtract_area_borders(buf2[i], i);
      }
    }
  }

/********************************* SECTION 7 global measurements ****************************/

  if(sp[7].present)
  {
    fseeko(inputfile, sp[7].file_offset + 16LL, SEEK_SET);

    if(fread(block, 2, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred\n");
      goto EXIT_3;
    }

    glob_msr_data.n_ref_beat_type = *((unsigned char *)block);

    glob_msr_data.n_qrs = glob_msr_data.n_ref_beat_type;

    glob_msr_data.n_pace_spike = *((unsigned char *)(block + 1));

    if(glob_msr_data.n_pace_spike)
    {
      fseeko(inputfile, sp[7].file_offset + 16LL + 6LL + (glob_msr_data.n_qrs * 16LL), SEEK_SET);

      if(fread(block, glob_msr_data.n_pace_spike * 4, 1, inputfile) != 1)
      {
        textEdit1->append("A read-error occurred\n");
        goto EXIT_3;
      }

      for(i=0; i<glob_msr_data.n_pace_spike; i++)
      {
        glob_msr_data.pace_spike_offset[i] = ((unsigned short *)block)[i];
      }
    }

#ifdef SPCECG_DEBUG
    printf("number of reference beat types or QRS complexes is %i\n"
           "number of pace spikes is: %i\n",
           glob_msr_data.n_qrs, glob_msr_data.n_pace_spike);
#endif
  }

//////////////////// create EDF file ////////////////////////////////

  strcpy(edf_filename, input_filename);

  remove_extension_from_filename(edf_filename);

  strcat(edf_filename, ".edf");

  hdl = edfopen_file_writeonly(edf_filename, EDFLIB_FILETYPE_EDFPLUS, scp_ecg.chns);

  if(hdl<0)
  {
    snprintf(txt_string, 2048, "Can not open file %s for writing.\n", edf_filename);
    textEdit1->append(txt_string);
    goto EXIT_4;
  }

  for(i=0; i<scp_ecg.chns; i++)
  {
    if(edf_set_samplefrequency(hdl, i, scp_ecg.sf))
    {
      textEdit1->append("Error: edf_set_samplefrequency()\n");
      goto EXIT_5;
    }

    if(edf_set_digital_minimum(hdl, i, -32768))
    {
      textEdit1->append("Error: edf_set_digital_minimum()\n");
      goto EXIT_5;
    }

    if(edf_set_digital_maximum(hdl, i, 32767))
    {
      textEdit1->append("Error: edf_set_digital_maximum()\n");
      goto EXIT_5;
    }

    lead_label_lookup(lp[i].label, scratchpad);

    if(edf_set_label(hdl, i, scratchpad))
    {
      textEdit1->append("Error: edf_set_label()\n");
      goto EXIT_5;
    }

    if(edf_set_physical_dimension(hdl, i, "uV"))
    {
      textEdit1->append("Error: edf_set_physical_dimension()\n");
      goto EXIT_5;
    }

    if(edf_set_physical_maximum(hdl, i, 32.767 * (double)scp_ecg.avm))
    {
      textEdit1->append("Error: edf_set_physical_maximum()\n");
      goto EXIT_5;
    }

    if(edf_set_physical_minimum(hdl, i, -32.768 * (double)scp_ecg.avm))
    {
      textEdit1->append("Error: edf_set_physical_minimum()\n");
      goto EXIT_5;
    }
  }

  strcpy(scratchpad, pat_dat.first_name);
  strcat(scratchpad, " ");
  strcat(scratchpad, pat_dat.last_name);
  remove_trailing_spaces(scratchpad);

  if(edf_set_patientname(hdl, scratchpad))
  {
    textEdit1->append("Error: edf_set_patientname()\n");
    goto EXIT_5;
  }

  edf_set_startdatetime(hdl,
                        pat_dat.startdate_year,
                        pat_dat.startdate_month,
                        pat_dat.startdate_day,
                        pat_dat.starttime_hour,
                        pat_dat.starttime_minute,
                        pat_dat.starttime_second);

  edf_set_birthdate(hdl,
                    pat_dat.birthdate_year,
                    pat_dat.birthdate_month,
                    pat_dat.birthdate_day);

  if(pat_dat.sex == 1)
  {
    edf_set_gender(hdl, 1);
  }

  if(pat_dat.sex == 2)
  {
    edf_set_gender(hdl, 0);
  }

  edf_set_patientcode(hdl, pat_dat.pat_id);

  strcpy(scratchpad, pat_dat.device_model);
  strcat(scratchpad, " ");
  strcat(scratchpad, pat_dat.device_ident);

  edf_set_equipment(hdl, scratchpad);

  strcpy(scratchpad, pat_dat.manufacturer);
  strcat(scratchpad, " ");
  strcat(scratchpad, pat_dat.device_serial);

  edf_set_recording_additional(hdl, scratchpad);

  if(sp[7].present)
  {
    if(glob_msr_data.n_pace_spike > 0)
    {
      edf_set_number_of_annotation_signals(hdl, 2);
    }
  }

#ifdef SPCECG_DEBUG
  edf_set_number_of_annotation_signals(hdl, 6);
#endif

  blocks = lp[0].samples / scp_ecg.sf;

  for(i=0; i<blocks; i++)
  {
    for(j=0; j<scp_ecg.chns; j++)
    {
      if(scp_ecg.bimodal || scp_ecg.ref_beat_subtract)
      {
        if(edfwrite_digital_samples(hdl, buf2[j] + (scp_ecg.sf * i)))
        {
          textEdit1->append("A write error occurred during conversion (EDFlib).\n");
          goto EXIT_5;
        }
      }
      else
      {
        if(edfwrite_digital_samples(hdl, buf[j] + (scp_ecg.sf * i)))
        {
          textEdit1->append("A write error occurred during conversion (EDFlib).\n");
          goto EXIT_5;
        }
      }
    }
  }

  if(sp[7].present)
  {
    for(i=0; i<glob_msr_data.n_pace_spike; i++)
    {
      edfwrite_annotation_latin1(hdl, glob_msr_data.pace_spike_offset[i] * 10, -1LL, "Pace");
    }
  }

#ifdef SPCECG_DEBUG
  if(scp_ecg.ref_beat_subtract)
  {
    for(i=0; i<qrs_data.n_qrs; i++)
    {
      if(qrs_data.qrs_subtr_type[i] == 0)
      {
        edfwrite_annotation_latin1(hdl, ((qrs_data.qrs_prot_start[i] - 1) * 10000) / scp_ecg.sf, -1LL, "QB");

        edfwrite_annotation_latin1(hdl, ((qrs_data.qrs_subtr_fiducial[i] - 1) * 10000) / scp_ecg.sf, -1LL, "FC");

        edfwrite_annotation_latin1(hdl, (qrs_data.qrs_prot_end[i] * 10000) / scp_ecg.sf, -1LL, "QE");

        edfwrite_annotation_latin1(hdl, ((qrs_data.qrs_subtr_start[i] - 1) * 10000) / scp_ecg.sf, -1LL, "SB");

        edfwrite_annotation_latin1(hdl, (qrs_data.qrs_subtr_end[i] * 10000) / scp_ecg.sf, -1LL, "SE");
      }
    }
  }
#endif

  textEdit1->append("Done.\n");

EXIT_5:

  if(hdl >= 0)
  {
    edfclose_file(hdl);
    hdl = -1;
  }

EXIT_4:

  for(i=0;i<scp_ecg.chns; i++)
  {
    if(buf[i] != NULL)
    {
      free(buf[i]);
      buf[i] = NULL;
    }

    if(buf2[i] != NULL)
    {
      free(buf2[i]);
      buf2[i] = NULL;
    }
  }

EXIT_3:

  if(block != NULL)
  {
    free(block);
    block = NULL;
  }

EXIT_2:

  if(inputfile != NULL)
  {
    fclose(inputfile);
    inputfile = NULL;
  }

EXIT_1:

  for(i=0; i<SCP_ECG_MAX_CHNS; i++)
  {
    if(qrs_data.ref_beat[i] != NULL)
    {
      free(qrs_data.ref_beat[i]);
      qrs_data.ref_beat[i] = NULL;
    }
  }

  pushButton1->setEnabled(true);
}


int UI_SCPECG2EDFwindow::read_data_section_zero(FILE *inputfile, char *block, long long filesize)
{
  int i;

  for(i=1; i<12; i++)
  {
    fseeko(inputfile, sp[0].file_offset + 16LL + (i * 10LL), SEEK_SET);

    if(fread(&(sp[i].section_id), 2, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (20)\n");
      return -1;
    }

    if(sp[i].section_id != i)
    {
      textEdit1->append("Error, section ID nr does not match (21)\n");
      return -1;
    }

    if(fread(&(sp[i].section_length), 4, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (22)\n");
      return -1;
    }

    if(fread(&(sp[i].file_offset), 4, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (23)\n");
      return -1;
    }

    if((sp[i].section_length > 0) && (sp[i].file_offset > 0LL))
    {
      sp[i].present = 1;

      sp[i].file_offset--;
    }
    else
    {
      if((sp[i].section_id == 1) || (sp[i].section_id == 3) || (sp[i].section_id == 6))
      {
        textEdit1->append("Error, a required section is missing (24)\n");
        return -1;
      }
    }

    if((sp[i].file_offset + sp[i].section_length) > filesize)
    {
      textEdit1->append("Error, index + length of section is > filesize (25)\n");
      return -1;
    }

    if(sp[i].present == 1)
    {
      if(read_section_header(i, inputfile, sp[i].file_offset, block))
      {
        return -1;
      }
    }
  }

  return 0;
}


int UI_SCPECG2EDFwindow::check_crc(FILE *inputfile, long long offset, long long len, unsigned short crc2, char *block)
{
  int i,
      blocks;

  unsigned short crc = 0xFFFF;

  long long ll_tmp;


  blocks = len / SPCECGBUFSIZE;

  fseeko(inputfile, offset, SEEK_SET);

  for(i=0; i<blocks; i++)
  {
    if(fread(block, SPCECGBUFSIZE, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (3)\n");
      return -1;
    }

    crc = crc_ccitt((unsigned char *)block, SPCECGBUFSIZE, crc);
  }

  ll_tmp = len % SPCECGBUFSIZE;

  if(ll_tmp > 0LL)
  {
    if(fread(block, ll_tmp, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (4)\n");
      return -1;
    }

    crc = crc_ccitt((unsigned char *)block, ll_tmp, crc);
  }

  if(crc != crc2)
  {
    textEdit1->append("CRC check failed!\n");
    return -1;
  }

  return 0;
}


int UI_SCPECG2EDFwindow::read_section_header(int n, FILE *inputfile, long long offset, char *block)
{
  char str[256];

  fseeko(inputfile, offset, SEEK_SET);

  if(fread(&(sp[n].crc), 2, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (5)\n");
    return -1;
  }

  if(fread(&(sp[n].section_id), 2, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (6)\n");
    return -1;
  }

  if(sp[n].section_id != n)
  {
    textEdit1->append("Error, section ID does not match (188)");
    return -1;
  }

  if(fread(&(sp[n].section_length), 4, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (7)\n");
    return -1;
  }

  if(fread(&(sp[n].section_version), 1, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (8)\n");
    return -1;
  }

  if(fread(&(sp[n].section_protocol_version), 1, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (9)\n");
    return -1;
  }

  if(fread(&(sp[n].reserved), 6, 1, inputfile) != 1)
  {
    textEdit1->append("A read-error occurred (10)\n");
    return -1;
  }

  if(check_crc(inputfile, offset + 2, sp[n].section_length - 2LL, sp[n].crc, block))
  {
    sprintf(str, "CRC-error in section %i\n", n);
    textEdit1->append(str);
    return -1;
  }

  sp[n].present = 1;

  return 0;
}


unsigned short UI_SCPECG2EDFwindow::crc_ccitt(const unsigned char *message, int nbytes, unsigned short remainder)
{
  int byte;

  unsigned char data;

  for(byte=0; byte<nbytes; byte++)  /* Divide the message by the polynomial, a byte at a time. */
  {
    data = message[byte] ^ (remainder >> 8);

    remainder = crc_ccitt_table[data] ^ (remainder << 8);
  }

  return remainder;  /* The final remainder is the CRC. */
}


void UI_SCPECG2EDFwindow::crc_ccitt_init(void)
{
  int dividend;

  unsigned short remainder;

  unsigned char bit;

  for(dividend=0; dividend<256; dividend++)  /* Compute the remainder of each possible dividend. */
  {
    remainder = dividend << 8;  /* Start with the dividend followed by zeros. */

    for(bit=8; bit>0; bit--)  /* Perform modulo-2 division, a bit at a time. */
    {
      if(remainder & 32768)  /* Try to divide the current data bit. */
      {
        remainder = (remainder << 1) ^ 0x1021;  /* polynomial */
      }
      else
      {
        remainder = (remainder << 1);
      }
    }

    crc_ccitt_table[dividend] = remainder;  /* Store the result into the table. */
  }
}


inline unsigned char UI_SCPECG2EDFwindow::reverse_bitorder(unsigned char byte)
{
  byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
  byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
  byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;

  return byte;
}


void UI_SCPECG2EDFwindow::lead_label_lookup(unsigned char idx, char *label)
{
  char scp_labels[256][9]={"unspec.", "I", "II", "V1", "V2", "V3", "V4", "V5", "V6", "V7",
                           "V2R", "V3R", "V4R", "V5R", "V6R", "V7R", "X", "Y", "Z", "CC5",
                           "CM5", "LA", "RA", "LL", "fl", "fE", "fC", "fA", "fM", "fF",
                           "fH", "dl", "dll", "dV1", "dV2", "dV3", "dV4", "dV5", "dV6", "dV7",
                           "dV2R", "dV3R", "dV4R", "dV5R", "dV6R", "dV7R", "dX", "dY", "dZ", "dCC5",
                           "dCM5", "dLA", "dRA", "dLL", "dfl", "dfE", "dfC", "dfA", "dfM", "dfF",
                           "dfH", "III", "aVR", "aVL", "aVF", "aVRneg", "V8", "V9", "V8R", "V9R",
                           "D", "A", "J", "Defib", "Extern", "A1", "A2", "A3", "A4", "dV8",
                           "dV9", "dV8R", "dV9R", "dD", "dA", "dj", "Chest", "V", "VR", "VL",
                           "VF", "MCL", "MCL1", "MCL2", "MCL3", "MCL4", "MCL5", "MCL6", "CC", "CC1",
                           "CC2", "CC3", "CC4", "CC6", "CC7", "CM", "CM1", "CM2", "CM3", "CM4",
                           "CM6", "dIII", "daVR", "daVL", "daVF", "daVRneg", "dChest", "dV", "dVR", "dVL",
                           "dVF", "CM7", "CH5", "CS5", "CB5", "CR5", "ML", "AB1", "AB2", "AB3",
                           "AB4", "ES", "AS", "AI", "S", "dDefib", "dExtern", "da1", "da2", "da3",
                           "da4", "dMCL1", "dMCL2", "dMCL3", "dMCL4", "dMCL5", "dMCL6", "RL", "CV5RL", "CV6LL",
                           "CV6LU", "V10", "dMCL", "dCC", "dCC1", "dCC2", "dCC3", "dCC4", "dCC5", "dCC6",
                           "dCM", "dCM1", "dCM2", "dCM3", "dCM4", "dCM6", "dCM7", "dCH5", "dCS5", "dCB5",
                           "dCR5", "dML", "dAB1", "dAB2", "dAB3", "dAB4", "dES", "dAS", "dAI", "dS",
                           "dRL", "dCV5RL", "dCV6LL", "dCV6LU", "dV10", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", "", "", "", "", "",
                           "", "", "", "", "", ""};

  if(idx > 184)
  {
    strcpy(label, "unknown");
  }
  else
  {
    strcpy(label, scp_labels[idx]);
  }
}


int UI_SCPECG2EDFwindow::get_patient_data(FILE *inputfile)
{
  char str[2048];

  int i, j, n, len, tag;

  long long offset;

  void *pntr=NULL;


  if(inputfile == NULL)
  {
    return -1;
  }

  offset = sp[1].file_offset + 16LL;

  for(int k=0; ; k++)
  {
    fseeko(inputfile, offset, SEEK_SET);

    if(fread(str, 3, 1, inputfile) != 1)
    {
      textEdit1->append("A read-error occurred (80)\n");
      return -1;
    }

    len = *((unsigned short *)(str + 1));

    if((offset + len + 3 - sp[1].file_offset) > sp[1].section_length)
    {
      textEdit1->append("Found an error in section 1 (81)\n"
                        "Conversion aborted\n");
      return -1;
    }

    offset += (len + 3);

    tag = *((unsigned char *)str);

    if(tag == 255)
    {
      return 0;
    }

    if(len)
    {
      n = len;

      if(n > 2046)
      {
        n = 2046;
      }

      if(fread(str, n, 1, inputfile) != 1)
      {
        textEdit1->append("A read-error occurred (82)\n");
        return -1;
      }

      str[n] = 0;

      if(tag == 2)  // Patient ID
      {
        strncpy(pat_dat.pat_id, str, 20);

        pat_dat.pat_id[20] = 0;
        remove_trailing_spaces(pat_dat.pat_id);
        remove_leading_spaces(pat_dat.pat_id);
      }

      if(tag == 0)  // Patient last name
      {
        strncpy(pat_dat.last_name, str, 20);

        pat_dat.last_name[20] = 0;
        remove_trailing_spaces(pat_dat.last_name);
        remove_leading_spaces(pat_dat.last_name);
      }

      if(tag == 1)  // Patient first name
      {
        strncpy(pat_dat.first_name, str, 20);

        pat_dat.first_name[20] = 0;
        remove_trailing_spaces(pat_dat.first_name);
        remove_leading_spaces(pat_dat.first_name);
      }

      if(tag == 8)  // Patient sex
      {
        pat_dat.sex = str[0];
      }

      if(tag == 25)  // startdate
      {
        pntr = str;
        pat_dat.startdate_year = *((unsigned short *)pntr);
        pat_dat.startdate_month = str[2];
        pat_dat.startdate_day = str[3];
      }

      if(tag == 26)  // starttime
      {
        pat_dat.starttime_hour = str[0];
        pat_dat.starttime_minute = str[1];
        pat_dat.starttime_second = str[2];
      }

      if(tag == 5)  // birthdate
      {
        pntr = str;
        pat_dat.startdate_year = *((unsigned short *)pntr);
        pat_dat.startdate_month = str[2];
        pat_dat.startdate_day = str[3];
      }

      if(tag == 14)  // machine ID acquiring device
      {
        pat_dat.lang_code = str[16];
        strncpy(pat_dat.device_model, str + 8, 5);
        pat_dat.device_model[5] = 0;

        i = str[35];
        i += 36;

        if(i < 1500)
        {
          strncpy(pat_dat.device_serial, str + i + 1, 48);
          latin1_to_ascii(pat_dat.device_serial, 48);
          pat_dat.device_serial[48] = 0;
          remove_trailing_spaces(pat_dat.device_serial);
          remove_leading_spaces(pat_dat.device_serial);
        }

        j = 0;

        for( ; i<1500; i++)
        {
          if(str[i] == 0)
          {
            j++;

            if(j == 1)
            {
              strncpy(pat_dat.device_ident, str + i + 1, 48);
              latin1_to_ascii(pat_dat.device_ident, 48);
              pat_dat.device_ident[48] = 0;
              remove_trailing_spaces(pat_dat.device_ident);
              remove_leading_spaces(pat_dat.device_ident);
            }

            if(j == 3)
            {
              strncpy(pat_dat.manufacturer, str + i + 1, 48);
              latin1_to_ascii(pat_dat.manufacturer, 48);
              pat_dat.manufacturer[48] = 0;
              remove_trailing_spaces(pat_dat.manufacturer);
              remove_leading_spaces(pat_dat.manufacturer);

              break;
            }
          }
        }
      }
    }
  }

  return 0;
}


int UI_SCPECG2EDFwindow::is_in_protected_area(int smpl)
{
  int i;

  for(i=0; i<qrs_data.n_qrs; i++)
  {
    if(smpl < qrs_data.qrs_prot_start[i])
    {
      return 0;
    }
    else if(smpl <= qrs_data.qrs_prot_end[i])
      {
        return 1;
      }
  }

  return 0;
}


int UI_SCPECG2EDFwindow::is_nearby_subtracted_area(int smpl)
{
  int i;

  for(i=0; i<qrs_data.n_qrs; i++)
  {
    if(qrs_data.qrs_subtr_type[i] == 0)
    {
      if(smpl < (qrs_data.qrs_subtr_start[i] - 1))
      {
        return 0;
      }
      else if(smpl <= (qrs_data.qrs_subtr_end[i] + 1))
        {
          return 1;
        }
    }
  }

  return 0;
}


int UI_SCPECG2EDFwindow::default_huffman_decoding(char *buf_in, int *buf_out, int sz_in, int sz_out)
{
  int i, j, bits=0, last_byte=0;

  char ch_tmp;

  union{
         unsigned long long ll_int;
         unsigned int one[2];
         unsigned short two[4];
         unsigned char four[8];
       } var;

  for(i=0; ;)
  {
    if(!scp_ecg.bimodal)
    {
      if(i >= sz_out)
      {
        break;
      }
    }

    if((bits / 8) >= sz_in)
    {
      break;
    }

    if(((sz_in * 8) - bits) < 8)
    {
      last_byte = 1;
    }

    memcpy(&var, buf_in + (bits / 8), 5);

    for(j=0; j<5; j++)
    {
      var.four[j] = reverse_bitorder(var.four[j]);
    }

    var.ll_int >>= (int)(bits % 8LL);

    if((var.four[0] & 1) == 0)  // b00000001
    {
      buf_out[i++] = 0;
      bits++;
    } else
    if((var.four[0] & 7) == 1)  // b00000111
    {
      buf_out[i++] = 1;
      bits += 3;
    } else
    if((var.four[0] & 7) == 5)  // b00000111
    {
      buf_out[i++] = -1;
      bits += 3;
    } else
    if((var.four[0] & 15) == 3)  // b00001111
    {
      buf_out[i++] = 2;
      bits += 4;
    } else
    if((var.four[0] & 15) == 11)  // b00001111
    {
      buf_out[i++] = -2;
      bits += 4;
    } else
    if((var.four[0] & 31) == 7)  // b00011111
    {
      buf_out[i++] = 3;
      bits += 5;
    } else
    if((var.four[0] & 31) == 23)  // b00011111
    {
      buf_out[i++] = -3;
      bits += 5;
    } else
    if((var.four[0] & 63) == 15)  // b00111111
    {
      buf_out[i++] = 4;
      bits += 6;
    } else
    if((var.four[0] & 63) == 47)  // b00111111
    {
      buf_out[i++] = -4;
      bits += 6;
    } else
    if((var.four[0] & 127) == 31)  // b01111111
    {
      buf_out[i++] = 5;
      bits += 7;
    } else
    if((var.four[0] & 127) == 95)  // b01111111
    {
      buf_out[i++] = -5;
      bits += 7;
    } else
    if(var.four[0] == 63)  // b11111111
    {
      buf_out[i++] = 6;
      bits += 8;
    } else
    if(last_byte)
    {
      break;
    } else
    if(var.four[0] == 191)  // b11111111
    {
      buf_out[i++] = -6;
      bits += 8;
    } else
    if(var.four[0] == 127)  // b11111111
    {
      if((var.four[1] & 1) == 0)  // b00000001
      {
        buf_out[i++] = 7;
        bits += 9;
      }
      else
      {
        buf_out[i++] = -7;
        bits += 9;
      }
    } else
    if(var.four[0] == 255)  // b11111111
    {
      if((var.four[1] & 3) == 0)  // b00000011
      {
        buf_out[i++] = 8;
        bits += 10;
      } else
      if((var.four[1] & 3) == 2)  // b00000011
      {
        buf_out[i++] = -8;
        bits += 10;
      } else
      if((var.four[1] & 3) == 1)  // b00000011
      {
        var.ll_int >>= 2;
        var.four[1] = reverse_bitorder(var.four[1]);
        buf_out[i++] = *((signed char *)&(var.four[1]));  // 8-bit original
        bits += 18;
      } else
      if((var.four[1] & 3) == 3)  // b00000011
      {
        var.ll_int >>= 10;
        ch_tmp = reverse_bitorder(var.four[0]);
        var.four[0] = reverse_bitorder(var.four[1]);
        var.four[1] = ch_tmp;
        buf_out[i++] = *((signed short *)&(var.two[0]));  // 16-bit original
        bits += 26;
      }
    }
  }

#ifdef SPCECG_DEBUG
  printf("huffman decoding: bytes processed is %i  size in: %i   size out: %i  samples produced is %i\n",
         bits / 8, sz_in, sz_out, i);
#endif

  return i;
}


void UI_SCPECG2EDFwindow::reconstitute_data_first_diff(int *buf, int sz)
{
  int i, val=0;

  for(i=0; i<sz; i++)
  {
    if(!i)
    {
      val = buf[i];
    }
    else
    {
      val += buf[i];

      buf[i] = val;
    }
  }
}


void UI_SCPECG2EDFwindow::reconstitute_data_second_diff(int *buf, int sz)
{
  int i, val_a=0, val_b=0;

  for(i=0; i<sz; i++)
  {
    if(i < 2)
    {
      if(!i)
      {
        val_a = buf[i];
      }
      else  // i == 1
      {
        val_b = buf[i];
      }
    }
    else  // Decoding of the 2nd difference data is performed using the following formula:
    {     // X(n) = D2(n) + 2*X(n-1) - X(n-2)
      buf[i] += (val_b * 2) - val_a;

      val_a = val_b;

      val_b = buf[i];
    }
  }
}


void UI_SCPECG2EDFwindow::smooth_subtract_area_borders(int *buf, int chn)
{
  int i, val;

  for(i=0; i<qrs_data.n_qrs; i++)
  {
    if(qrs_data.qrs_subtr_type[i] == 0)
    {
      if(qrs_data.qrs_subtr_start[i] > 1)
      {
        val = (buf[qrs_data.qrs_subtr_start[i] - 2] + buf[qrs_data.qrs_subtr_start[i] - 1]) / 2;

        buf[qrs_data.qrs_subtr_start[i] - 2] = val;

        buf[qrs_data.qrs_subtr_start[i] - 1] = val;
      }

      if(qrs_data.qrs_subtr_end[i] < lp[chn].samples)
      {
        val = (buf[qrs_data.qrs_subtr_end[i] - 1] + buf[qrs_data.qrs_subtr_end[i]]) / 2;

        buf[qrs_data.qrs_subtr_end[i] - 1] = val;

        buf[qrs_data.qrs_subtr_end[i]] = val;
      }
    }
  }
}


int UI_SCPECG2EDFwindow::reconstitute_decimated_samples(int *buf_in, int *buf_out, int chn)
{
  int i, j=0, r, step;

  for(i=0; i<lp[chn].samples;)
  {
    if((is_in_protected_area(i + 1)) || (is_in_protected_area(i + scp_ecg.sf_ratio)))
    {
      buf_out[i++] = buf_in[j++] * scp_ecg.avm_ratio;
    }
    else
    {
      if((j + 1) < lp[chn].huffman_decoder_produced_samples)
      {
        step = ((buf_in[j+1] - buf_in[j]) * scp_ecg.avm_ratio) / scp_ecg.sf_ratio;
      }
      else
      {
        step = 0;
      }

      for(r=0; r<scp_ecg.sf_ratio; r++)
      {
        if(is_nearby_subtracted_area(i + 1))
        {
          buf_out[i++] = (buf_in[j] * scp_ecg.avm_ratio);
        }
        else
        {
          buf_out[i++] = (buf_in[j] * scp_ecg.avm_ratio) + (step * r);
        }

        if(is_in_protected_area(i + 1))
        {
          break;
        }
      }

      j++;
    }

    if(j > lp[chn].huffman_decoder_produced_samples)
    {
      return -1;
    }
  }

#ifdef SPCECG_DEBUG
  printf("reconstitute decimated samples: samples in processed is %i\n", j);
#endif

  return 0;
}
















