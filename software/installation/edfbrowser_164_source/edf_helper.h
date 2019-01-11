/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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


#ifndef EDFPLUS_HELPER_H
#define EDFPLUS_HELPER_H


#include <stdlib.h>
#include <string.h>

#include "global.h"


#ifdef __cplusplus
extern "C" {
#endif

int edfplus_annotation_get_tal_timestamp_digit_cnt(struct edfhdrblock *);
int edfplus_annotation_get_tal_timestamp_decimal_cnt(struct edfhdrblock *);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif





