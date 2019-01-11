/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Teunis van Beelen
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





#include "utils.h"


#define FLT_ROUNDS 1



  /* removes extension including the dot */
void remove_extension_from_filename(char *str)
{
  int i, len;

  len = strlen(str);

  if(len < 1)
  {
    return;
  }

  for(i=len-1; i>=0; i--)
  {
    if((str[i]=='/') || (str[i]=='\\'))
    {
      return;
    }

    if(str[i]=='.')
    {
      str[i] = 0;

      return;
    }
  }
}


  /* size is size of destination, returns length of filename */
int get_filename_from_path(char *dest, const char *src, int size)
{
  int i, len;

  if(size<1)
  {
    return -1;
  }

  if(size<2)
  {
    dest[0] = 0;

    return 0;
  }

  len = strlen(src);

  if(len < 1)
  {
    dest[0] = 0;

    return 0;
  }

  for(i=len-1; i>=0; i--)
  {
    if((src[i]=='/') || (src[i]=='\\'))
    {
      break;
    }
  }

  i++;

  if(i == len)
  {
    dest[0] = 0;

    return 0;
  }

  strncpy(dest, src + i, size);

  dest[size-1] = 0;

  return strlen(dest);
}


  /* size is size of destination, returns length of directory */
  /* last character of destination is not a slash! */
int get_directory_from_path(char *dest, const char *src, int size)
{
  int i, len;

  if(size<1)
  {
    return -1;
  }

  if(size<2)
  {
    dest[0] = 0;

    return 0;
  }

  len = strlen(src);

  if(len < 1)
  {
    dest[0] = 0;

    return 0;
  }

  for(i=len-1; i>=0; i--)
  {
    if((src[i]=='/') || (src[i]=='\\'))
    {
      break;
    }
  }

  strncpy(dest, src, size);

  if(i < size)
  {
    dest[i] = 0;
  }
  else
  {
    dest[size-1] = 0;
  }

  return strlen(dest);
}



void convert_trailing_zeros_to_spaces(char *str)
{
  int i, j, len;

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='.')
    {
      for(j=(len-1); j>=0; j--)
      {
        if((str[j]!='.')&&(str[j]!='0'))
        {
          break;
        }

        if(str[j]=='.')
        {
          str[j] = ' ';
          break;
        }

        str[j] = ' ';
      }
      break;
    }
  }
}



void remove_trailing_spaces(char *str)
{
  int i, len;

  len = strlen(str);

  for(i=(len-1); i>=0; i--)
  {
    if(str[i]!=' ')  break;
  }

  str[i+1] = 0;
}



void remove_leading_spaces(char *str)
{
  int i, diff, len;

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i] != ' ')
    {
      break;
    }
  }

  if(!i)
  {
    return;
  }

  diff = i;

  for(; i<=len; i++)
  {
    str[i - diff] = str[i];
  }
}



void remove_trailing_zeros(char *str)
{
  int i, j,
      len,
      numberfound,
      dotfound,
      decimalzerofound,
      trailingzerofound=1;

  while(trailingzerofound)
  {
    numberfound = 0;
    dotfound = 0;
    decimalzerofound = 0;
    trailingzerofound = 0;

    len = strlen(str);

    for(i=0; i<len; i++)
    {
      if((str[i] < '0') || (str[i] > '9'))
      {
        if(decimalzerofound)
        {
          if(str[i-decimalzerofound-1] == '.')
          {
            decimalzerofound++;
          }

          for(j=i; j<(len+1); j++)
          {
            str[j-decimalzerofound] = str[j];
          }

          trailingzerofound = 1;

          break;
        }

        if(str[i] != '.')
        {
          numberfound = 0;
          dotfound = 0;
          decimalzerofound = 0;
        }
      }
      else
      {
        numberfound = 1;

        if(str[i] > '0')
        {
          decimalzerofound = 0;
        }
      }

      if((str[i] == '.') && numberfound)
      {
        dotfound = 1;
      }

      if((str[i] == '0') && dotfound)
      {
        decimalzerofound++;
      }
    }
  }

  if(decimalzerofound)
  {
    if(str[i-decimalzerofound-1] == '.')
    {
      decimalzerofound++;
    }

    for(j=i; j<(len+1); j++)
    {
      str[j-decimalzerofound] = str[j];
    }
  }

  if(len > 1)
  {
    if(!((str[len - 2] < '0') || (str[i] > '9')))
    {
       if(str[len - 1] == '.')
       {
         str[len - 1] = 0;
       }
    }
  }
}



void utf8_to_latin1(char *utf8_str)
{
  int i, j, len;

  unsigned char *str;


  str = (unsigned char *)utf8_str;

  len = strlen(utf8_str);

  if(!len)
  {
    return;
  }

  j = 0;

  for(i=0; i<len; i++)
  {
    if((str[i] < 32) || ((str[i] > 127) && (str[i] < 192)))
    {
      str[j++] = '.';

      continue;
    }

    if(str[i] > 223)
    {
      str[j++] = 0;

      return;  /* can only decode Latin-1 ! */
    }

    if((str[i] & 224) == 192)  /* found a two-byte sequence containing Latin-1, Greek, Cyrillic, Coptic, Armenian, Hebrew, etc. characters */
    {
      if((i + 1) == len)
      {
        str[j++] = 0;

        return;
      }

      if((str[i] & 252) != 192) /* it's not a Latin-1 character */
      {
        str[j++] = '.';

        i++;

        continue;
      }

      if((str[i + 1] & 192) != 128) /* UTF-8 violation error */
      {
        str[j++] = 0;

        return;
      }

      str[j] = str[i] << 6;
      str[j] += (str[i + 1] & 63);

      i++;
      j++;

      continue;
    }

    str[j++] = str[i];
  }

  if(j<len)
  {
    str[j] = 0;
  }
}



void latin1_to_utf8(char *latin1_str, int len)
{
  int i, j;

  unsigned char *str, tmp_str[1024];


  str = (unsigned char *)latin1_str;

  j = 0;

  for(i=0; i<len; i++)
  {
    tmp_str[j] = str[i];

    if(str[i]==0)  break;

    if(str[i]<32) tmp_str[j] = '.';

    if((str[i]>126)&&(str[i]<160))  tmp_str[j] = '.';

    if(str[i]>159)
    {
      if((len-j)<2)
      {
        tmp_str[j] = ' ';
      }
      else
      {
        tmp_str[j] = 192 + (str[i]>>6);
        j++;
        tmp_str[j] = 128 + (str[i]&63);
      }
    }

    j++;

    if(j>=len)  break;
  }

  for(i=0; i<len; i++)
  {
    str[i] = tmp_str[i];

    if(str[i]==0)  return;
  }
}



void latin1_to_ascii(char *str, int len)
{
  int i, value;

  for(i=0; i<len; i++)
  {
    value = *((unsigned char *)(str + i));

    if((value>31)&&(value<127))
    {
      continue;
    }

    switch(value)
    {
      case 128 : str[i] = 'E';  break;

      case 130 : str[i] = ',';  break;

      case 131 : str[i] = 'F';  break;

      case 132 : str[i] = '\"';  break;

      case 133 : str[i] = '.';  break;

      case 134 : str[i] = '+';  break;

      case 135 : str[i] = '+';  break;

      case 136 : str[i] = '^';  break;

      case 137 : str[i] = 'm';  break;

      case 138 : str[i] = 'S';  break;

      case 139 : str[i] = '<';  break;

      case 140 : str[i] = 'E';  break;

      case 142 : str[i] = 'Z';  break;

      case 145 : str[i] = '`';  break;

      case 146 : str[i] = '\'';  break;

      case 147 : str[i] = '\"';  break;

      case 148 : str[i] = '\"';  break;

      case 149 : str[i] = '.';  break;

      case 150 : str[i] = '-';  break;

      case 151 : str[i] = '-';  break;

      case 152 : str[i] = '~';  break;

      case 154 : str[i] = 's';  break;

      case 155 : str[i] = '>';  break;

      case 156 : str[i] = 'e';  break;

      case 158 : str[i] = 'z';  break;

      case 159 : str[i] = 'Y';  break;

      case 171 : str[i] = '<';  break;

      case 180 : str[i] = '\'';  break;

      case 181 : str[i] = 'u';  break;

      case 187 : str[i] = '>';  break;

      case 191 : str[i] = '\?';  break;

      case 192 : str[i] = 'A';  break;

      case 193 : str[i] = 'A';  break;

      case 194 : str[i] = 'A';  break;

      case 195 : str[i] = 'A';  break;

      case 196 : str[i] = 'A';  break;

      case 197 : str[i] = 'A';  break;

      case 198 : str[i] = 'E';  break;

      case 199 : str[i] = 'C';  break;

      case 200 : str[i] = 'E';  break;

      case 201 : str[i] = 'E';  break;

      case 202 : str[i] = 'E';  break;

      case 203 : str[i] = 'E';  break;

      case 204 : str[i] = 'I';  break;

      case 205 : str[i] = 'I';  break;

      case 206 : str[i] = 'I';  break;

      case 207 : str[i] = 'I';  break;

      case 208 : str[i] = 'D';  break;

      case 209 : str[i] = 'N';  break;

      case 210 : str[i] = 'O';  break;

      case 211 : str[i] = 'O';  break;

      case 212 : str[i] = 'O';  break;

      case 213 : str[i] = 'O';  break;

      case 214 : str[i] = 'O';  break;

      case 215 : str[i] = 'x';  break;

      case 216 : str[i] = 'O';  break;

      case 217 : str[i] = 'U';  break;

      case 218 : str[i] = 'U';  break;

      case 219 : str[i] = 'U';  break;

      case 220 : str[i] = 'U';  break;

      case 221 : str[i] = 'Y';  break;

      case 222 : str[i] = 'I';  break;

      case 223 : str[i] = 's';  break;

      case 224 : str[i] = 'a';  break;

      case 225 : str[i] = 'a';  break;

      case 226 : str[i] = 'a';  break;

      case 227 : str[i] = 'a';  break;

      case 228 : str[i] = 'a';  break;

      case 229 : str[i] = 'a';  break;

      case 230 : str[i] = 'e';  break;

      case 231 : str[i] = 'c';  break;

      case 232 : str[i] = 'e';  break;

      case 233 : str[i] = 'e';  break;

      case 234 : str[i] = 'e';  break;

      case 235 : str[i] = 'e';  break;

      case 236 : str[i] = 'i';  break;

      case 237 : str[i] = 'i';  break;

      case 238 : str[i] = 'i';  break;

      case 239 : str[i] = 'i';  break;

      case 240 : str[i] = 'd';  break;

      case 241 : str[i] = 'n';  break;

      case 242 : str[i] = 'o';  break;

      case 243 : str[i] = 'o';  break;

      case 244 : str[i] = 'o';  break;

      case 245 : str[i] = 'o';  break;

      case 246 : str[i] = 'o';  break;

      case 247 : str[i] = '-';  break;

      case 248 : str[i] = '0';  break;

      case 249 : str[i] = 'u';  break;

      case 250 : str[i] = 'u';  break;

      case 251 : str[i] = 'u';  break;

      case 252 : str[i] = 'u';  break;

      case 253 : str[i] = 'y';  break;

      case 254 : str[i] = 't';  break;

      case 255 : str[i] = 'y';  break;

      default  : str[i] = ' ';  break;
    }
  }
}


int antoi(const char *input_str, int len)
{
  char str[4096];

  if(len > 4095)  len = 4095;

  strncpy(str, input_str, len);

  str[len] = 0;

  return atoi_nonlocalized(str);
}


/* minimum is the minimum digits that will be printed (minus sign not included), leading zero's will be added if necessary */
/* if sign is zero, only negative numbers will have the sign '-' character */
/* if sign is one, the sign '+' or '-' character will always be printed */
/* returns the amount of characters printed */
int fprint_int_number_nonlocalized(FILE *file, int q, int minimum, int sign)
{
  int flag=0, z, i, j=0, base = 1000000000;

  if(minimum < 0)
  {
    minimum = 0;
  }

  if(minimum > 9)
  {
    flag = 1;
  }

  if(q < 0)
  {
    fputc('-', file);

    j++;

    q = -q;
  }
  else
  {
    if(sign)
    {
      fputc('+', file);

      j++;
    }
  }

  for(i=10; i; i--)
  {
    if(minimum == i)
    {
      flag = 1;
    }

    z = q / base;

    q %= base;

    if(z || flag)
    {
      fputc('0' + z, file);

      j++;

      flag = 1;
    }

    base /= 10;
  }

  if(!flag)
  {
    fputc('0', file);

    j++;
  }

  return j;
}


/* minimum is the minimum digits that will be printed (minus sign not included), leading zero's will be added if necessary */
/* if sign is zero, only negative numbers will have the sign '-' character */
/* if sign is one, the sign '+' or '-' character will always be printed */
/* returns the amount of characters printed */
int fprint_ll_number_nonlocalized(FILE *file, long long q, int minimum, int sign)
{
  int flag=0, z, i, j=0;

  long long base = 1000000000000000000LL;

  if(minimum < 0)
  {
    minimum = 0;
  }

  if(minimum > 18)
  {
    flag = 1;
  }

  if(q < 0LL)
  {
    fputc('-', file);

    j++;

    q = -q;
  }
  else
  {
    if(sign)
    {
      fputc('+', file);

      j++;
    }
  }

  for(i=19; i; i--)
  {
    if(minimum == i)
    {
      flag = 1;
    }

    z = q / base;

    q %= base;

    if(z || flag)
    {
      fputc('0' + z, file);

      j++;

      flag = 1;
    }

    base /= 10LL;
  }

  if(!flag)
  {
    fputc('0', file);

    j++;
  }

  return j;
}


/* minimum is the minimum digits that will be printed (minus sign not included), leading zero's will be added if necessary */
/* if sign is zero, only negative numbers will have the sign '-' character */
/* if sign is one, the sign '+' or '-' character will always be printed */
/* returns the amount of characters printed */
int sprint_int_number_nonlocalized(char *str, int q, int minimum, int sign)
{
  int flag=0, z, i, j=0, base = 1000000000;

  if(minimum < 0)
  {
    minimum = 0;
  }

  if(minimum > 9)
  {
    flag = 1;
  }

  if(q < 0)
  {
    str[j++] = '-';

    q = -q;
  }
  else
  {
    if(sign)
    {
      str[j++] = '+';
    }
  }

  for(i=10; i; i--)
  {
    if(minimum == i)
    {
      flag = 1;
    }

    z = q / base;

    q %= base;

    if(z || flag)
    {
      str[j++] = '0' + z;

      flag = 1;
    }

    base /= 10;
  }

  if(!flag)
  {
    str[j++] = '0';
  }

  str[j] = 0;

  return j;
}


/* minimum is the minimum digits that will be printed (minus sign not included), leading zero's will be added if necessary */
/* if sign is zero, only negative numbers will have the sign '-' character */
/* if sign is one, the sign '+' or '-' character will always be printed */
/* returns the amount of characters printed */
int sprint_ll_number_nonlocalized(char *str, long long q, int minimum, int sign)
{
  int flag=0, z, i, j=0;

  long long base = 1000000000000000000LL;

  if(minimum < 0)
  {
    minimum = 0;
  }

  if(minimum > 18)
  {
    flag = 1;
  }

  if(q < 0LL)
  {
    str[j++] = '-';

    q = -q;
  }
  else
  {
    if(sign)
    {
      str[j++] = '+';
    }
  }

  for(i=19; i; i--)
  {
    if(minimum == i)
    {
      flag = 1;
    }

    z = q / base;

    q %= base;

    if(z || flag)
    {
      str[j++] = '0' + z;

      flag = 1;
    }

    base /= 10LL;
  }

  if(!flag)
  {
    str[j++] = '0';
  }

  str[j] = 0;

  return j;
}


int sprint_number_nonlocalized(char *str, double nr)
{
  int flag=0, z, i, j=0, q, base = 1000000000;

  double var;

  q = (int)nr;

  var = nr - q;

  if(nr < 0.0)
  {
    str[j++] = '-';

    if(q < 0)
    {
      q = -q;
    }
  }

  for(i=10; i; i--)
  {
    z = q / base;

    q %= base;

    if(z || flag)
    {
      str[j++] = '0' + z;

      flag = 1;
    }

    base /= 10;
  }

  if(!flag)
  {
    str[j++] = '0';
  }

  base = 100000000;

  var *= (base * 10);

  q = (int)var;

  if(q < 0)
  {
    q = -q;
  }

  if(!q)
  {
    str[j] = 0;

    return j;
  }

  str[j++] = '.';

  for(i=9; i; i--)
  {
    z = q / base;

    q %= base;

    str[j++] = '0' + z;

    base /= 10;
  }

  str[j] = 0;

  j--;

  for(; j>0; j--)
  {
    if(str[j] == '0')
    {
      str[j] = 0;
    }
    else
    {
      j++;

      break;
    }
  }

  return j;
}


double atof_nonlocalized(const char *str)
{
  int i=0, dot_pos=-1, decimals=0, sign=1;

  double value, value2=0.0;


  value = atoi_nonlocalized(str);

  while(str[i] == ' ')
  {
    i++;
  }

  if((str[i] == '+') || (str[i] == '-'))
  {
    if(str[i] == '-')
    {
      sign = -1;
    }

    i++;
  }

  for(; ; i++)
  {
    if(str[i] == 0)
    {
      break;
    }

    if(((str[i] < '0') || (str[i] > '9')) && (str[i] != '.'))
    {
      break;
    }

    if(dot_pos >= 0)
    {
      if((str[i] >= '0') && (str[i] <= '9'))
      {
        decimals++;
      }
      else
      {
        break;
      }
    }

    if(str[i] == '.')
    {
      if(dot_pos < 0)
      {
        dot_pos = i;
      }
    }
  }

  if(decimals)
  {
    value2 = atoi_nonlocalized(str + dot_pos + 1) * sign;

    i = 1;

    while(decimals--)
    {
      i *= 10;
    }

    value2 /= i;
  }

  return value + value2;
}


int atoi_nonlocalized(const char *str)
{
  int i=0, value=0, sign=1;

  while(str[i] == ' ')
  {
    i++;
  }

  if((str[i] == '+') || (str[i] == '-'))
  {
    if(str[i] == '-')
    {
      sign = -1;
    }

    i++;
  }

  for( ; ; i++)
  {
    if(str[i] == 0)
    {
      break;
    }

    if((str[i] < '0') || (str[i] > '9'))
    {
      break;
    }

    value *= 10;

    value += (str[i] - '0');
  }

  return value * sign;
}


long long atoll_x(const char *str, int dimension)
{
  int i,
      radix,
      negative=0;

  long long value=0LL;

  while(*str==' ')
  {
    str++;
  }

  if(*str=='-')
  {
    negative = 1;
    str++;
  }
  else
  {
    if(*str=='+')
    {
      str++;
    }
  }

  for(i=0; ; i++)
  {
    if(str[i]=='.')
    {
      str += (i + 1);

      break;
    }

    if((str[i]<'0') || (str[i]>'9'))
    {
      if(negative)
      {
        return value * dimension * -1LL;
      }
      else
      {
        return value * dimension;
      }
    }

    value *= 10LL;

    value += str[i] - '0';
  }

  radix = 1;

  for(i=0; radix<dimension; i++)
  {
    if((str[i]<'0') || (str[i]>'9'))
    {
      break;
    }

    radix *= 10;

    value *= 10LL;

    value += str[i] - '0';
  }

  if(negative)
  {
    return value * (dimension / radix) * -1LL;
  }
  else
  {
    return value * (dimension / radix);
  }
}



int is_integer_number(char *str)
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





int is_number(char *str)
{
  int i=0, len, hasspace=0, hassign=0, digit=0, hasdot=0, hasexp=0;

  len = strlen(str);

  if(!len)  return 1;

  if((str[0]=='+')||(str[0]=='-'))
  {
    hassign++;
    i++;
  }

  for(; i<len; i++)
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
    if(++i==len)
    {
      return 1;
    }

    if((str[i]=='+')||(str[i]=='-'))
    {
      hassign++;
      i++;
    }

    for(; i<len; i++)
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


void strntolower(char *s, int n)
{
  int i;

  for(i=0; i<n; i++)
  {
    s[i] = tolower(s[i]);
  }
}


int round_125_cat(double value)
{
  if(value < 0)  value *= -1;

  if(value < 0.000001)  return 10;

  while(value > 1000)  value /=10;

  while(value < 100)  value *=10;

  if(value > 670)
  {
    return 10;
  }
  else if(value > 300)
    {
      return 50;
    }
    else if(value > 135)
      {
        return 20;
      }
      else
      {
        return 10;
      }

  return 10;
}


void hextoascii(char *str)
{
  int i, len;

  char scratchpad[4];

  len = strlen(str) / 2;

  for(i=0; i<len; i++)
  {
    scratchpad[0] = str[i*2];
    scratchpad[1] = str[(i*2)+1];
    scratchpad[2] = 0;

    str[i] = strtol(scratchpad, NULL, 16);
  }

  str[i] = 0;
}


void bintoascii(char *str)
{
  int i, len;

  char scratchpad[16];

  len = strlen(str) / 8;

  for(i=0; i<len; i++)
  {
    strncpy(scratchpad, str + (i * 8), 8);
    scratchpad[8] = 0;

    str[i] = strtol(scratchpad, NULL, 2);
  }

  str[i] = 0;
}


void bintohex(char *str)
{
  int i, len;

  char scratchpad[16];

  len = strlen(str) / 8;

  for(i=0; i<len; i++)
  {
    strncpy(scratchpad, str + (i * 8), 8);
    scratchpad[8] = 0;

    sprintf(str + (i * 2), "%02x", (unsigned int)strtol(scratchpad, NULL, 2));
  }

  str[i * 2] = 0;
}


void asciitohex(char *dest, const char *str)
{
  int i, tmp, len;

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    tmp = ((unsigned char *)str)[i] / 16;
    if(tmp < 10)
    {
      dest[i*2] = tmp + '0';
    }
    else
    {
      dest[i*2] = tmp + 'a' - 10;
    }

    tmp = ((unsigned char *)str)[i] % 16;
    if(tmp < 10)
    {
      dest[(i*2)+1] = tmp + '0';
    }
    else
    {
      dest[(i*2)+1] = tmp + 'a' - 10;
    }
  }

  dest[i*2] = 0;
}


void asciitobin(char *dest, const char *str)
{
  int i, j, len;

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    for(j=0; j<8; j++)
    {
      if(((unsigned char *)str)[i] & (1<<(7-j)))
      {
        dest[(i*8)+j] = '1';
      }
      else
      {
        dest[(i*8)+j] = '0';
      }
    }
  }

  dest[i*8] = 0;
}


void hextobin(char *dest, const char *str)
{
  int i, j, len;

  unsigned int tmp;

  char scratchpad[4];

  len = strlen(str) / 2;

  for(i=0; i<len; i++)
  {
    scratchpad[0] = str[i*2];
    scratchpad[1] = str[(i*2)+1];
    scratchpad[2] = 0;

    tmp = strtol(scratchpad, NULL, 16);

    for(j=0; j<8; j++)
    {
      if(tmp & (1<<(7-j)))
      {
        dest[(i*8)+j] = '1';
      }
      else
      {
        dest[(i*8)+j] = '0';
      }
    }
  }

  dest[i * 8] = 0;
}


double round_to_3digits(double val)
{
  int i, exp=0, polarity=1;

  if(!dblcmp(val, 0.0))
  {
    return 0;
  }

  if(val < 0)
  {
    polarity = -1;

    val *= -1;
  }

  while(val < 99.999)
  {
    val *= 10;

    exp--;
  }

  while(val > 999.999)
  {
    val /= 10;

    exp++;
  }

  val = nearbyint(val);

  for(i=0; i<exp; i++)
  {
    val *= 10;
  }

  for(i=0; i>exp; i--)
  {
    val /= 10;
  }

  return val * polarity;
}


double round_up_step125(double val, double *ratio)
{
  int i, exp=0;

  double ltmp;

   while(val < 0.999)
  {
    val *= 10;

    exp--;
  }

  while(val > 9.999)
  {
    val /= 10;

    exp++;
  }

  val = nearbyint(val);

  if(val > 4.999)
  {
    ltmp = 10;

    if(ratio != NULL)
    {
      *ratio = 2;
    }
  }
  else if(val > 1.999)
    {
      ltmp = 5;

      if(ratio != NULL)
      {
        *ratio = 2.5;
      }
    }
    else
    {
      ltmp = 2;

      if(ratio != NULL)
      {
        *ratio = 2;
      }
    }

  for(i=0; i<exp; i++)
  {
    ltmp *= 10;
  }

  for(i=0; i>exp; i--)
  {
    ltmp /= 10;
  }

  if((ltmp < 1e-13) && (ltmp > -1e-13))
  {
    return 0;
  }

  return ltmp;
}


double round_down_step125(double val, double *ratio)
{
  int i, exp=0;

  double ltmp;

  while(val < 0.999)
  {
    val *= 10;

    exp--;
  }

  while(val > 9.999)
  {
    val /= 10;

    exp++;
  }

  val = nearbyint(val);

  if(val < 1.001)
  {
    ltmp = 0.5;

    if(ratio != NULL)
    {
      *ratio = 2;
    }
  }
  else if(val < 2.001)
    {
      ltmp = 1;

      if(ratio != NULL)
      {
        *ratio = 2;
      }
    }
    else if(val < 5.001)
      {
        ltmp = 2;

        if(ratio != NULL)
        {
          *ratio = 2.5;
        }
      }
      else
      {
        ltmp = 5;

        if(ratio != NULL)
        {
          *ratio = 2;
        }
      }

  for(i=0; i<exp; i++)
  {
    ltmp *= 10;
  }

  for(i=0; i>exp; i--)
  {
    ltmp /= 10;
  }

  if((ltmp < 1e-13) && (ltmp > -1e-13))
  {
    return 0;
  }

  return ltmp;
}


int convert_to_metric_suffix(char *buf, double value, int decimals)
{
  double ltmp;

  char suffix=' ';

  if(value < 0)
  {
      ltmp = value * -1;
  }
  else
  {
      ltmp = value;
  }

  if(ltmp > 0.999999e12 && ltmp < 0.999999e15)
  {
      ltmp = ltmp / 1e12;

      suffix = 'T';
  }
  else if(ltmp > 0.999999e9)
    {
        ltmp = ltmp / 1e9;

        suffix = 'G';
    }
    else if(ltmp > 0.999999e6)
      {
          ltmp = ltmp / 1e6;

          suffix = 'M';
      }
      else if(ltmp > 0.999999e3)
        {
          ltmp /= 1e3;

          suffix = 'K';
        }
        else if(ltmp > 0.999999e-3 && ltmp < 0.999999)
          {
            ltmp *= 1e3;

            suffix = 'm';
          }
          else if( ltmp > 0.999999e-6 && ltmp < 0.999999e-3)
            {
              ltmp *= 1e6;

              suffix = 'u';
            }
            else if(ltmp > 0.999999e-9 && ltmp < 0.999999e-6)
              {
                ltmp *= 1e9;

                suffix = 'n';
              }
              else if(ltmp > 0.999999e-12 && ltmp < 0.999999e-9)
                {
                  ltmp *= 1e12;

                  suffix = 'p';
                }

  if(value >= 0)
  {
    switch(decimals)
    {
      case 0: return sprintf(buf, "%.0f%c", ltmp, suffix);
              break;
      case 1: return sprintf(buf, "%.1f%c", ltmp, suffix);
              break;
      case 2: return sprintf(buf, "%.2f%c", ltmp, suffix);
              break;
      case 3: return sprintf(buf, "%.3f%c", ltmp, suffix);
              break;
      case 4: return sprintf(buf, "%.4f%c", ltmp, suffix);
              break;
      case 5: return sprintf(buf, "%.5f%c", ltmp, suffix);
              break;
      case 6: return sprintf(buf, "%.6f%c", ltmp, suffix);
              break;
      default: return sprintf(buf, "%.3f%c", ltmp, suffix);
              break;
    }
  }

  if(value < 0)
  {
    switch(decimals)
    {
      case 0: return sprintf(buf, "%.0f%c", ltmp * -1, suffix);
              break;
      case 1: return sprintf(buf, "%.1f%c", ltmp * -1, suffix);
              break;
      case 2: return sprintf(buf, "%.2f%c", ltmp * -1, suffix);
              break;
      default: return sprintf(buf, "%.3f%c", ltmp * -1, suffix);
              break;
    }
  }

  strcpy(buf, "0");

  return 1;
}


int strtoipaddr(unsigned int *dest, const char *src)
{
  int i, err=1;

  unsigned int val;

  char *ptr,
       str[64];

  if(strlen(src) < 7)
  {
    return -1;
  }

  strncpy(str, src, 64);

  str[63] = 0;

  ptr = strtok(str, ".");

  if(ptr != NULL)
  {
    val = atoi(ptr) << 24;

    for(i=0; i<3; i++)
    {
      ptr = strtok(NULL, ".");

      if(ptr == NULL)
      {
        break;
      }

      val += atoi(ptr) << (16 - (i * 8));
    }

    err = 0;
  }

  if(err)
  {
    return -1;
  }

  *dest = val;

  return 0;
}


int dblcmp(double val1, double val2)
{
  long double diff = (long double)val1 - (long double)val2;

  if(diff > 1e-13)
  {
    return 1;
  }
  else if(-diff > 1e-13)
    {
      return -1;
    }
    else
    {
      return 0;
    }
}


int base64_dec(const void *src, void *dest, int len)
{
  int i, j, idx;

  const unsigned char *arr_in=(const unsigned char *)src;

  unsigned char *arr_out=(unsigned char *)dest;

  union{
    unsigned char four[4];
    unsigned short two[2];
    unsigned int one;
  } var;

  unsigned char base64[256]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
    0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,
    0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  var.one = 0;

  for(i=0, j=0, idx=0; i<len; i++)
  {
    if(arr_in[i] <= ' ')
    {
      continue;
    }

    if(arr_in[i] == '=')
    {
      break;
    }

    if(arr_in[i] != 'A')
    {
      if(base64[arr_in[i]] == 0)
      {
        return -1;
      }
    }

    var.one += base64[arr_in[i]];

    idx++;

    if(idx < 4)
    {
      var.one <<= 6;
    }
    else
    {
      idx = 0;

      arr_out[j++] = var.four[2];

      arr_out[j++] = var.four[1];

      arr_out[j++] = var.four[0];

      var.one = 0;
    }
  }

  if(idx == 2)
  {
    var.one >>= 6;

    arr_out[j++] = var.four[0];
  }
  else if(idx == 3)
    {
      var.one >>= 6;

      arr_out[j++] = var.four[1];

      arr_out[j++] = var.four[0];
    }

  return j;
}


/* returns also empty tokens */
char * strtok_r_e(char *str, const char *delim, char **saveptr)
{
  int i, j, delim_len;

  char *ptr;

  char *buf;

  if(delim == NULL)  return NULL;

  delim_len = strlen(delim);
  if(delim_len < 1)  return NULL;

  if(str != NULL)
  {
    buf = str;
  }
  else
  {
    buf = *saveptr;
  }

  for(i=0; ; i++)
  {
    if(buf[i] == 0)
    {
      if(i)
      {
        ptr = buf;

        buf += i;

        *saveptr = buf;

        return ptr;
      }
      else
      {
        return NULL;
      }
    }

    for(j=0; j<delim_len; j++)
    {
      if(buf[i] == delim[j])
      {
        buf[i] = 0;

        ptr = buf;

        buf += (i + 1);

        *saveptr = buf;

        return ptr;
      }
    }
  }

  return NULL;
}









