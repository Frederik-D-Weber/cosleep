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



#ifndef FLYWHEELBUTTON_H
#define FLYWHEELBUTTON_H

#include <QtGlobal>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QColor>
#include <QLinearGradient>
#include <QBrush>

#include <sys/time.h>
#include <math.h>



class UI_Flywheel: public QWidget
{
  Q_OBJECT

public:
  UI_Flywheel(QWidget *parent=0);
  ~UI_Flywheel();

  QSize sizeHint() const {return minimumSizeHint(); }
  QSize minimumSizeHint() const {return QSize(10,30); }

private slots:

  void wheel_rotate(void);
  void t2_time_out(void);

protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);

private:
  QTimer *t1,
         *t2;

  int mouse_y,
      mouse_old_y,
      mouse_delta_y,
      use_move_events,
      time_old,
      time_new,
      dial_speed,
      dial_speed_arr[4],
      dial_speed_ptr,
      spinning,
      holdoff,
      knobs,
      cursor_above_object;

  double wheel_position;


  struct timeval tv;

  QColor *bordercolor_up,
         *bordercolor_down;

  QLinearGradient *linearGrad;

signals:
     void dialMoved(int);
};


#endif


