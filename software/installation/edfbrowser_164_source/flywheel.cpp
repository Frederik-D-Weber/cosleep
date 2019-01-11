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



#include "flywheel.h"



UI_Flywheel::UI_Flywheel(QWidget *w_parent) : QWidget(w_parent)
{
  int i;

  use_move_events = 0;
  wheel_position = 0.0;
  spinning = 0;
  knobs = 10;
  cursor_above_object = 0;
  dial_speed = 0;
  dial_speed_ptr = 0;
  for(i=0; i<4; i++)
  {
    dial_speed_arr[i] = 0;
  }

  bordercolor_up   = new QColor(224, 224, 224);
  bordercolor_down = new QColor(92, 92, 92);

  linearGrad = new QLinearGradient();
//   linearGrad->setColorAt(0, QColor(155, 155, 155));
//   linearGrad->setColorAt(1, QColor(95, 95, 95));
  linearGrad->setColorAt(0, QColor(175, 175, 175));
  linearGrad->setColorAt(1, QColor(75, 75, 75));

  t1 = new QTimer(this);
  t2 = new QTimer(this);
  t2->setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1->setTimerType(Qt::PreciseTimer);
  t2->setTimerType(Qt::PreciseTimer);
#endif

  connect(t1, SIGNAL(timeout()), this, SLOT(wheel_rotate()));
  connect(t2, SIGNAL(timeout()), this, SLOT(t2_time_out()));
}


UI_Flywheel::~UI_Flywheel()
{
  delete bordercolor_up;
  delete bordercolor_down;

  delete linearGrad;
}


void UI_Flywheel::t2_time_out(void)
{
  dial_speed = 0;
}


void UI_Flywheel::wheel_rotate(void)
{
  int h, cumulative_pixels=0;

  double circum;


  t2->start(100);

  h = height();

  circum = (M_PI * (double)h);

  if(use_move_events && (!spinning))
  {
    wheel_position += (((M_PI * 2.0) / (circum / (double)mouse_delta_y)) + (M_PI * 2.0));

    while(wheel_position >= (M_PI * 2.0))
    {
      wheel_position -= (M_PI * 2.0);
    }

    while(wheel_position < 0.0)
    {
      wheel_position += (M_PI * 2.0);
    }

    cumulative_pixels = mouse_delta_y;
  }

  if(spinning)
  {
    if(dial_speed)
    {
      wheel_position += ((M_PI * 2.0) / (circum / ((double)dial_speed / 25.0)));

      cumulative_pixels = (double)dial_speed / 25.0;

      while(wheel_position >= (M_PI * 2.0))
      {
        wheel_position -= (M_PI * 2.0);
      }

      while(wheel_position < 0.0)
      {
        wheel_position += (M_PI * 2.0);
      }

      if(holdoff)
      {
        holdoff--;
      }
      else
      {
        if(dial_speed > 0)
        {
          dial_speed -= 25;

          if(dial_speed < 0)
          {
            dial_speed = 0;
          }
        }
        else
        {
          dial_speed += 25;

          if(dial_speed > 0)
          {
            dial_speed = 0;
          }
        }

        if((dial_speed < 500) && (dial_speed > -500))
        {
          if((dial_speed < 125) && (dial_speed > -125))
          {
            holdoff = 6;
          }
          else
          {
            holdoff = 3;
          }
        }
      }
    }

    if(dial_speed == 0)
    {
      t1->stop();

      spinning = 0;
    }
  }

  update();

  emit dialMoved(cumulative_pixels);
}


void UI_Flywheel::paintEvent(QPaintEvent *)
{
  int w, h, h_2, i, j, b1, b2;

  double knobstep, rad, rad_offset;


  QPainter painter(this);
#if QT_VERSION >= 0x050000
  painter.setRenderHint(QPainter::Qt4CompatiblePainting, true);
#endif

  w = width();
  h = height();
  h_2 = h / 2;

  knobstep = (M_PI * 2.0) / (double)(knobs * 2);

  linearGrad->setStart(QPoint(w / 2, 0));
  linearGrad->setFinalStop(QPoint(w / 2, h));

  painter.fillRect(0, 0, w-1, h-1, QBrush(*linearGrad));

  rad_offset = wheel_position;
  while(rad_offset > (knobstep * 2.0))
  {
    rad_offset -= (knobstep * 2.0);
  }

  for(i=0; i<knobs; i+=2)
  {
    rad = (i * knobstep) + rad_offset;
    b1 = h_2 - (h_2 * cos(rad));

    rad = ((i + 1) * knobstep) + rad_offset;
    if(rad >= M_PI)
    {
      b2 = h;
    }
    else
    {
      b2 = h_2 - (h_2 * cos(rad));
    }

//     j = 222.0 - ((60.0 / (double)h) * (((double)b1 + (double)b2) / 2.0));
    j = 240.0 - ((100.0 / (double)h) * (((double)b1 + (double)b2) / 2.0));

    painter.fillRect(1, b1, w-2, b2 - b1, QColor(j, j, j));

    if(b1 > h_2)
    {
      if(b1 > (h * 0.75))
      {
        if(b1 > (h * 0.9))
        {
          painter.fillRect(1, b1, w-2, 3, *bordercolor_up);
        }
        else
        {
          painter.fillRect(1, b1, w-2, 2, *bordercolor_up);
        }
      }

      painter.setPen(*bordercolor_up);
      painter.drawLine(1, b1, w-2, b1);
    }

    if(b2 < h_2)
    {
      if(b2 < (h * 0.25))
      {
        if(b2 < (h * 0.1))
        {
          painter.fillRect(1, b2, w-2, 2, *bordercolor_down);
        }
        else
        {
          painter.fillRect(1, b2, w-2, 2, *bordercolor_down);
        }
      }

      painter.setPen(*bordercolor_down);
      painter.drawLine(1, b2, w-2, b2);
    }
  }

  painter.setPen(Qt::black);
  painter.drawRect(0, 0, w-1, h-1);
}


void UI_Flywheel::mousePressEvent(QMouseEvent *press_event)
{
  int i;

  if(press_event->button()==Qt::LeftButton)
  {
    for(i=0; i<4; i++)
    {
      dial_speed_arr[i] = 0;
    }

    spinning = 0;

    dial_speed = 0;

    t1->stop();

    mouse_old_y = press_event->y();

    gettimeofday(&tv, NULL);

    time_old = tv.tv_usec;
    time_new = tv.tv_usec;

    use_move_events = 1;

    cursor_above_object = 1;

    setMouseTracking(true);

    t2->start(100);
  }
}


void UI_Flywheel::mouseReleaseEvent(QMouseEvent *release_event)
{
  if(release_event->button()==Qt::LeftButton)
  {
    if(use_move_events && (!spinning))
    {
      if((dial_speed > 50) || (dial_speed < -50))
      {
        spinning = 1;

        holdoff = 0;

        t1->start(40);
      }
    }

    setMouseTracking(false);

    use_move_events = 0;

    cursor_above_object = 0;
  }
}


void UI_Flywheel::mouseMoveEvent(QMouseEvent *move_event)
{
  int i, mouse_x;

  if(use_move_events)
  {
    mouse_y = move_event->y();

    mouse_x = move_event->x();

    mouse_delta_y = mouse_y - mouse_old_y;

    mouse_old_y = mouse_y;

    if((mouse_y > height()) || (mouse_y < 0) || (mouse_x > width()) || (mouse_x < 0))
    {
      if((cursor_above_object == 1) && (!spinning))
      {
        if((dial_speed > 50) || (dial_speed < -50))
        {
          spinning = 1;

          holdoff = 0;

          t1->start(40);
        }
      }

      cursor_above_object = 0;
    }
    else
    {
      if(cursor_above_object == 1)
      {
        gettimeofday(&tv, NULL);

        time_old = time_new;
        time_new = tv.tv_usec;

        if(time_new > time_old)
        {
          dial_speed_arr[dial_speed_ptr] = (mouse_delta_y * 1000000) / (time_new - time_old);
        }
        else
        {
          dial_speed_arr[dial_speed_ptr] = (mouse_delta_y * 1000000) / (1000000 + time_new - time_old);
        }

        dial_speed_ptr++;
        dial_speed_ptr %= 4;

        dial_speed = 0;
        for(i=0; i<4; i++)
        {
          dial_speed += dial_speed_arr[i];
        }
        dial_speed /= 4;
      }

      spinning = 0;

      t1->stop();

      cursor_above_object = 1;

      wheel_rotate();
    }
  }
}






