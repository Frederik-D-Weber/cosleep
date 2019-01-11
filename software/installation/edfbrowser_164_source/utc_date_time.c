/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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





#include "utc_date_time.h"




void date_time_to_utc(long long *seconds, struct date_time_struct date_time)
{
  *seconds = (date_time.day - 1) * 86400LL;

  if(date_time.month>1)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>2)
  {
    if(date_time.year%4)
    {
      *seconds += 2419200LL;
    }
    else
    {
      *seconds += 2505600LL;
    }
  }

  if(date_time.month>3)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>4)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>5)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>6)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>7)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>8)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>9)
  {
    *seconds += 2592000LL;
  }

  if(date_time.month>10)
  {
    *seconds += 2678400LL;
  }

  if(date_time.month>11)
  {
    *seconds += 2592000LL;
  }

  *seconds += (date_time.year - 1970) * 31536000LL;
  *seconds += ((date_time.year - 1970 + 1) / 4) * 86400LL;

  *seconds += date_time.hour * 3600;
  *seconds += date_time.minute * 60;
  *seconds += date_time.second;

  switch(date_time.month)
  {
    case  1 : strcpy(date_time.month_str, "JAN");
              break;
    case  2 : strcpy(date_time.month_str, "FEB");
              break;
    case  3 : strcpy(date_time.month_str, "MAR");
              break;
    case  4 : strcpy(date_time.month_str, "APR");
              break;
    case  5 : strcpy(date_time.month_str, "MAY");
              break;
    case  6 : strcpy(date_time.month_str, "JUN");
              break;
    case  7 : strcpy(date_time.month_str, "JUL");
              break;
    case  8 : strcpy(date_time.month_str, "AUG");
              break;
    case  9 : strcpy(date_time.month_str, "SEP");
              break;
    case 10 : strcpy(date_time.month_str, "OCT");
              break;
    case 11 : strcpy(date_time.month_str, "NOV");
              break;
    case 12 : strcpy(date_time.month_str, "DEC");
              break;
    default : strcpy(date_time.month_str, "ERR");
              break;
  }
}



void utc_to_date_time(long long seconds, struct date_time_struct *date_time)
{
  date_time->year = 1970;

  while(1)
  {
    if(date_time->year%4)
    {
      if(seconds>=31536000)
      {
        date_time->year++;

        seconds -= 31536000;
      }
      else
      {
        break;
      }
    }
    else
    {
      if(seconds>=31622400)
      {
        date_time->year++;

        seconds -= 31622400;
      }
      else
      {
        break;
      }
    }
  }

  date_time->month = 1;

  if(seconds>=2678400)
  {
    date_time->month++;

    seconds -= 2678400;

    if(date_time->year%4)
    {
      if(seconds>=2419200)
      {
        date_time->month++;

        seconds -= 2419200;

        if(seconds>=2678400)
        {
          date_time->month++;

          seconds -= 2678400;

          if(seconds>=2592000)
          {
            date_time->month++;

            seconds -= 2592000;

            if(seconds>=2678400)
            {
              date_time->month++;

              seconds -= 2678400;

              if(seconds>=2592000)
              {
                date_time->month++;

                seconds -= 2592000;

                if(seconds>=2678400)
                {
                  date_time->month++;

                  seconds -= 2678400;

                  if(seconds>=2678400)
                  {
                    date_time->month++;

                    seconds -= 2678400;

                    if(seconds>=2592000)
                    {
                      date_time->month++;

                      seconds -= 2592000;

                      if(seconds>=2678400)
                      {
                        date_time->month++;

                        seconds -= 2678400;

                        if(seconds>=2592000)
                        {
                          date_time->month++;

                          seconds -= 2592000;

                          if(seconds>=2678400)
                          {
                            date_time->month++;

                            seconds -= 2678400;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else
    {
      if(seconds>=2505600)
      {
        date_time->month++;

        seconds -= 2505600;

        if(seconds>=2678400)
        {
          date_time->month++;

          seconds -= 2678400;

          if(seconds>=2592000)
          {
            date_time->month++;

            seconds -= 2592000;

            if(seconds>=2678400)
            {
              date_time->month++;

              seconds -= 2678400;

              if(seconds>=2592000)
              {
                date_time->month++;

                seconds -= 2592000;

                if(seconds>=2678400)
                {
                  date_time->month++;

                  seconds -= 2678400;

                  if(seconds>=2678400)
                  {
                    date_time->month++;

                    seconds -= 2678400;

                    if(seconds>=2592000)
                    {
                      date_time->month++;

                      seconds -= 2592000;

                      if(seconds>=2678400)
                      {
                        date_time->month++;

                        seconds -= 2678400;

                        if(seconds>=2592000)
                        {
                          date_time->month++;

                          seconds -= 2592000;

                          if(seconds>=2678400)
                          {
                            date_time->month++;

                            seconds -= 2678400;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  date_time->day = (seconds / 86400) + 1;

  seconds %= 86400;

  date_time->hour = seconds / 3600;

  seconds %= 3600;

  date_time->minute = seconds / 60;

  seconds %= 60;

  date_time->second = seconds;

  switch(date_time->month)
  {
    case  1 : strcpy(date_time->month_str, "JAN");
              break;
    case  2 : strcpy(date_time->month_str, "FEB");
              break;
    case  3 : strcpy(date_time->month_str, "MAR");
              break;
    case  4 : strcpy(date_time->month_str, "APR");
              break;
    case  5 : strcpy(date_time->month_str, "MAY");
              break;
    case  6 : strcpy(date_time->month_str, "JUN");
              break;
    case  7 : strcpy(date_time->month_str, "JUL");
              break;
    case  8 : strcpy(date_time->month_str, "AUG");
              break;
    case  9 : strcpy(date_time->month_str, "SEP");
              break;
    case 10 : strcpy(date_time->month_str, "OCT");
              break;
    case 11 : strcpy(date_time->month_str, "NOV");
              break;
    case 12 : strcpy(date_time->month_str, "DEC");
              break;
    default : strcpy(date_time->month_str, "ERR");
              break;
  }
}



