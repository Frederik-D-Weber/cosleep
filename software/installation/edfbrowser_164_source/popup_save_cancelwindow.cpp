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




#include "popup_save_cancelwindow.h"



UI_CancelSavewindow::UI_CancelSavewindow(const char *title, const char *str,
                                         const char *button1txt, const char *button2txt,
                                         int *button_nr)
{
  int i,
      len,
      max=0,
      labelwidth = 0,
      labelheight = 25;

  buttonnr = button_nr;

  len = strlen(str);

  for(i=0; i<len; i++)
  {
    if(str[i]=='\n')
    {
      labelheight += 20;
      if(max>labelwidth) labelwidth = max;
      max = 0;
    }
    else
    {
      max++;
    }
  }

  if(max>labelwidth) labelwidth = max;

  labelwidth *= 6;
  labelwidth += 10;
  if(labelwidth<260)  labelwidth = 260;

  MessageDialog = new QDialog;

  MessageDialog->setMinimumSize(labelwidth + 40, labelheight + 85);
  MessageDialog->setMaximumSize(labelwidth + 40, labelheight + 85);
  MessageDialog->setWindowTitle(title);
  MessageDialog->setModal(true);
  MessageDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  Label1 = new QLabel(MessageDialog);
  Label1->setGeometry(20, 20, labelwidth, labelheight);
  Label1->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  Label1->setText(str);

  pushButton1 = new QPushButton(MessageDialog);
  pushButton1->setGeometry(20, labelheight + 40, 100, 25);
  pushButton1->setText(button1txt);

  pushButton2 = new QPushButton(MessageDialog);
  pushButton2->setGeometry(140, labelheight + 40, 100, 25);
  pushButton2->setText(button2txt);

  QObject::connect(pushButton1, SIGNAL(clicked()), this, SLOT(button1Clicked()));
  QObject::connect(pushButton2, SIGNAL(clicked()), this, SLOT(button2Clicked()));

  MessageDialog->exec();
}



void UI_CancelSavewindow::button1Clicked()
{
  *buttonnr = 0;

  MessageDialog->close();
}



void UI_CancelSavewindow::button2Clicked()
{
  *buttonnr = 1;

  MessageDialog->close();
}











