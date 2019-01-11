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




#include "colordialog.h"



UI_ColorMenuDialog::UI_ColorMenuDialog(int *newColor, QWidget *w_parent)
{
  colormenu_dialog = new QDialog(w_parent);

  colormenu_dialog->setMinimumSize(105, 130);
  colormenu_dialog->setMaximumSize(105, 130);
  colormenu_dialog->setWindowTitle("Color");
  colormenu_dialog->setModal(true);
  colormenu_dialog->setAttribute(Qt::WA_DeleteOnClose, true);

  color = newColor;

  *color = -1;

  CloseButton = new QPushButton(colormenu_dialog);
  CloseButton->setGeometry(5, 105, 95, 20);
  CloseButton->setText("Close");

  ColorButton1 = new SpecialButton(colormenu_dialog);
  ColorButton1->setGeometry(5, 5, 20, 20);
  ColorButton1->setColor(Qt::black);

  ColorButton2 = new SpecialButton(colormenu_dialog);
  ColorButton2->setGeometry(30, 5, 20, 20);
  ColorButton2->setColor(Qt::red);

  ColorButton3 = new SpecialButton(colormenu_dialog);
  ColorButton3->setGeometry(55, 5, 20, 20);
  ColorButton3->setColor(Qt::darkRed);

  ColorButton4 = new SpecialButton(colormenu_dialog);
  ColorButton4->setGeometry(80, 5, 20, 20);
  ColorButton4->setColor(Qt::green);

  ColorButton5 = new SpecialButton(colormenu_dialog);
  ColorButton5->setGeometry(5, 30, 20, 20);
  ColorButton5->setColor(Qt::darkGreen);

  ColorButton6 = new SpecialButton(colormenu_dialog);
  ColorButton6->setGeometry(30, 30, 20, 20);
  ColorButton6->setColor(Qt::blue);

  ColorButton7 = new SpecialButton(colormenu_dialog);
  ColorButton7->setGeometry(55, 30, 20, 20);
  ColorButton7->setColor(Qt::darkBlue);

  ColorButton8 = new SpecialButton(colormenu_dialog);
  ColorButton8->setGeometry(80, 30, 20, 20);
  ColorButton8->setColor(Qt::cyan);

  ColorButton9 = new SpecialButton(colormenu_dialog);
  ColorButton9->setGeometry(5, 55, 20, 20);
  ColorButton9->setColor(Qt::darkCyan);

  ColorButton10 = new SpecialButton(colormenu_dialog);
  ColorButton10->setGeometry(30, 55, 20, 20);
  ColorButton10->setColor(Qt::magenta);

  ColorButton11 = new SpecialButton(colormenu_dialog);
  ColorButton11->setGeometry(55, 55, 20, 20);
  ColorButton11->setColor(Qt::darkMagenta);

  ColorButton12 = new SpecialButton(colormenu_dialog);
  ColorButton12->setGeometry(80, 55, 20, 20);
  ColorButton12->setColor(Qt::yellow);

  ColorButton13 = new SpecialButton(colormenu_dialog);
  ColorButton13->setGeometry(5, 80, 20, 20);
  ColorButton13->setColor(Qt::darkYellow);

  ColorButton14 = new SpecialButton(colormenu_dialog);
  ColorButton14->setGeometry(30, 80, 20, 20);
  ColorButton14->setColor(Qt::gray);

  ColorButton15 = new SpecialButton(colormenu_dialog);
  ColorButton15->setGeometry(55, 80, 20, 20);
  ColorButton15->setColor(Qt::darkGray);

  ColorButton16 = new SpecialButton(colormenu_dialog);
  ColorButton16->setGeometry(80, 80, 20, 20);
  ColorButton16->setColor(Qt::white);

  QObject::connect(CloseButton,   SIGNAL(clicked()),                colormenu_dialog, SLOT(close()));
  QObject::connect(ColorButton1,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton1_pressed(SpecialButton *)));
  QObject::connect(ColorButton2,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton2_pressed(SpecialButton *)));
  QObject::connect(ColorButton3,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton3_pressed(SpecialButton *)));
  QObject::connect(ColorButton4,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton4_pressed(SpecialButton *)));
  QObject::connect(ColorButton5,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton5_pressed(SpecialButton *)));
  QObject::connect(ColorButton6,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton6_pressed(SpecialButton *)));
  QObject::connect(ColorButton7,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton7_pressed(SpecialButton *)));
  QObject::connect(ColorButton8,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton8_pressed(SpecialButton *)));
  QObject::connect(ColorButton9,  SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton9_pressed(SpecialButton *)));
  QObject::connect(ColorButton10, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton10_pressed(SpecialButton *)));
  QObject::connect(ColorButton11, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton11_pressed(SpecialButton *)));
  QObject::connect(ColorButton12, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton12_pressed(SpecialButton *)));
  QObject::connect(ColorButton13, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton13_pressed(SpecialButton *)));
  QObject::connect(ColorButton14, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton14_pressed(SpecialButton *)));
  QObject::connect(ColorButton15, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton15_pressed(SpecialButton *)));
  QObject::connect(ColorButton16, SIGNAL(clicked(SpecialButton *)), this,             SLOT(ColorButton16_pressed(SpecialButton *)));

  colormenu_dialog->exec();
}


void UI_ColorMenuDialog::ColorButton1_pressed(SpecialButton *)
{
  *color = Qt::black;

  colormenu_dialog->close();
}


void UI_ColorMenuDialog::ColorButton2_pressed(SpecialButton *)
{
  *color = Qt::red;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton3_pressed(SpecialButton *)
{
  *color = Qt::darkRed;

  colormenu_dialog->close();
}


void UI_ColorMenuDialog::ColorButton4_pressed(SpecialButton *)
{
  *color = Qt::green;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton5_pressed(SpecialButton *)
{
  *color = Qt::darkGreen;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton6_pressed(SpecialButton *)
{
  *color = Qt::blue;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton7_pressed(SpecialButton *)
{
  *color = Qt::darkBlue;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton8_pressed(SpecialButton *)
{
  *color = Qt::cyan;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton9_pressed(SpecialButton *)
{
  *color = Qt::darkCyan;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton10_pressed(SpecialButton *)
{
  *color = Qt::magenta;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton11_pressed(SpecialButton *)
{
  *color = Qt::darkMagenta;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton12_pressed(SpecialButton *)
{
  *color = Qt::yellow;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton13_pressed(SpecialButton *)
{
  *color = Qt::darkYellow;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton14_pressed(SpecialButton *)
{
  *color = Qt::gray;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton15_pressed(SpecialButton *)
{
  *color = Qt::darkGray;

  colormenu_dialog->close();
}



void UI_ColorMenuDialog::ColorButton16_pressed(SpecialButton *)
{
  *color = Qt::white;

  colormenu_dialog->close();
}









