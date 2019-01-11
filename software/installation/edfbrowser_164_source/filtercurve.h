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



#ifndef FILTERCURVE_H
#define FILTERCURVE_H


#include <QtGlobal>
#include <QWidget>
#include <QPainter>
#include <QPen>




class FilterCurve: public QWidget
{
  Q_OBJECT

public:
  FilterCurve(QWidget *parent=0);
  ~FilterCurve();

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(30,10); }

  void setSignalColor1(QColor);
  void setSignalColor2(QColor);
  void setTraceWidth(int);
  void setBackgroundColor(QColor);
  void setRasterColor(QColor);
  void setMarkerColor(QColor);
  void setH_RasterSize(int);
  void setV_RasterSize(int);
  void drawCurve(double *, int, double, double);
  void drawCurve(double *, int, double, double, double *, int, double, double, int mrkr1 = 0, int mrkr2 = 0);

public slots:

protected:
  void paintEvent(QPaintEvent *);

private:

  QColor SignalColor1,
         SignalColor2,
         BackgroundColor,
         RasterColor,
         MarkerColor;

  QFont *filtcurve_font;

  double max_value1,
         min_value1,
         *buf1,
         max_value2,
         min_value2,
         *buf2;

  int bufsize1,
      bufsize2,
      h_rastersize,
      v_rastersize,
      drawcurve_before_raster,
      tracewidth,
      marker_1,
      marker_2;
};


#endif


