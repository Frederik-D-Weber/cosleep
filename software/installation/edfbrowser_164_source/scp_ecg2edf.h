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


#ifndef UI_SCPECG2EDFFORM_H
#define UI_SCPECG2EDFFORM_H


#include <QtGlobal>
#include <QApplication>
#include <QDialog>
#include <QPushButton>
#include <QObject>
#include <QTextEdit>
#include <QFileDialog>
#include <QCursor>
#include <QStyle>
#if QT_VERSION < 0x050000
#include <QPlastiqueStyle>
#include <QWindowsStyle>
#endif
#include <QProgressDialog>
#include <QString>
#include <QByteArray>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "utils.h"
#include "edflib.h"
#include "utc_date_time.h"

#define SCP_ECG_MAX_CHNS 256



class UI_SCPECG2EDFwindow : public QObject
{
  Q_OBJECT

public:
  UI_SCPECG2EDFwindow(char *recent_dir=NULL, char *save_dir=NULL);

private:


QPushButton  *pushButton1,
             *pushButton2;

QTextEdit    *textEdit1;

QDialog      *myobjectDialog;

char  *recent_opendir,
      *recent_savedir;

unsigned short crc_ccitt_table[256];

struct scp_ecg_prop_struct{
        int avm;
        int avm_prot;
        double avm_ratio;
        int sf;
        int sf_prot;
        int sf_ratio;
        int chns;
        int encoding;
        int encoding_prot;
        int bimodal;
        int ref_beat_subtract;
        int huffman_enc;
        } scp_ecg;

struct section_prop_struct{
        int present;
        long long file_offset;
        unsigned short crc;
        int section_id;
        int section_length;
        int section_version;
        int section_protocol_version;
        char reserved[6];
        } sp[12];

struct lead_prop_struct{
        int start;
        int end;
        int samples;
        int bimod_samples;
        int bytes;
        unsigned char label;
        int huffman_decoder_produced_samples;
        } lp[SCP_ECG_MAX_CHNS];

struct huffmantable_struct{
        int h_tables_cnt;
        int code_structs_cnt;
        int prefix_bits;
        int total_bits;
        int table_mode_switch;
        int base_value;
        int base_code;
        } ht;

struct patient_data_struct{
        char pat_id[21];
        int startdate_year;
        int startdate_month;
        int startdate_day;
        int starttime_hour;
        int starttime_minute;
        int starttime_second;
        char last_name[21];
        char first_name[21];
        int birthdate_year;
        int birthdate_month;
        int birthdate_day;
        int sex;
        char device_model[6];
        char device_serial[49];
        char device_ident[49];
        char manufacturer[49];
        unsigned char lang_code;
        } pat_dat;

struct qrs_loc_data_struct{
        int ref_beat_length;
        int fiducial_tp;
        int n_qrs;
        int qrs_subtr_type[1024];
        int qrs_subtr_start[1024];
        int qrs_subtr_fiducial[1024];
        int qrs_subtr_end[1024];
        int qrs_prot_start[1024];
        int qrs_prot_end[1024];
        int ref_beat_bytes[SCP_ECG_MAX_CHNS];
        int huffman_decoder_produced_samples[SCP_ECG_MAX_CHNS];
        int *ref_beat[SCP_ECG_MAX_CHNS];
        } qrs_data;

struct global_measure_data_struct{
        int n_ref_beat_type;
        int n_qrs;
        int n_pace_spike;
        int pace_spike_offset[1024];
        int pace_spike_amp[1024];
        } glob_msr_data;


int read_data_section_zero(FILE *, char *, long long);

int read_section_header(int, FILE *, long long, char *);

int check_crc(FILE *, long long, long long, unsigned short, char *);

void crc_ccitt_init(void);

unsigned short crc_ccitt(const unsigned char *, int, unsigned short);

inline unsigned char reverse_bitorder(unsigned char);

void lead_label_lookup(unsigned char, char *);

int get_patient_data(FILE *);

int is_in_protected_area(int);

int is_nearby_subtracted_area(int);

int default_huffman_decoding(char *, int *, int, int);

void reconstitute_data_first_diff(int *, int);

void reconstitute_data_second_diff(int *, int);

int reconstitute_decimated_samples(int *, int *, int);

void smooth_subtract_area_borders(int *, int);

private slots:

void SelectFileButton();

};




#endif


