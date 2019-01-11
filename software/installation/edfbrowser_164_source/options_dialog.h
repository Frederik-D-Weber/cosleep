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




#ifndef OPTIONSFORM1_H
#define OPTIONSFORM1_H



#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QColor>
#include <QSpinBox>
#include <QPalette>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QStringList>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QComboBox>
#include <QString>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGroupBox>

#include "global.h"
#include "mainwindow.h"
#include "colordialog.h"
#include "special_button.h"
#include "signalcurve.h"
#include "edf_annot_list.h"


class UI_Mainwindow;



class UI_OptionsDialog : public QObject
{
  Q_OBJECT

public:
  UI_OptionsDialog(QWidget *parent=0);

  UI_Mainwindow *mainwindow;


private:

QDialog        *optionsdialog;

QScrollArea    *scrollarea;

QFrame         *frame;

QGridLayout    *scrolllayout;

QTabWidget     *tabholder;

QWidget        *tab1,
               *tab2,
               *tab3,
               *tab4;

QVBoxLayout    *mainLayout;

QHBoxLayout    *horLayout;

QPushButton    *CloseButton,
               *colorSchema_Blue_on_Gray_Button,
               *colorSchema_NK_Button,
               *colorSchema_Dark_Button,
               *ApplyButton,
               *DefaultButton2,
               *ApplyButton2,
               *saveColorSchemaButton,
               *loadColorSchemaButton;

SpecialButton  *BgColorButton,
               *SrColorButton,
               *BrColorButton,
               *MrColorButton,
               *TxtColorButton,
               *SigColorButton,
               *BaseColorButton,
               *Crh1ColorButton,
               *Crh2ColorButton,
               *FrColorButton,
               *AnnotMkrButton,
               *AnnotDurationButton,
               *slabel2_1,
               *slabel2_3;

QLabel         *label1,
               *label2,
               *label3,
               *label4,
               *label5,
               *label6,
               *label7,
               *label8,
               *label9,
               *label10,
               *label11,
               *label12,
               *label12_1,
               *label12_2,
               *label12_3,
               *label13,
               *label4_1,
               *label4_2,
               *label4_3,
               *label4_4,
               *label4_5,
               *label4_6,
               *label4_7,
               *label4_8,
               *label4_9,
               *label4_10,
               *label4_11,
               *label4_12,
               *label2_2,
               *label2_4,
               *label1_3,
               *label2_3,
               *label3_3,
               *label3_4,
               *label3_5;

QSpinBox       *spinbox2_1,
               *spinbox2_2,
               *spinbox3_1,
               *spinbox4_1,
               *spinbox4_2,
               *spinbox4_3;

QDoubleSpinBox *dspinbox3_2,
               *dspinbox4_4;

QComboBox      *combobox4_1,
               *combobox4_2,
               *combobox4_3;

QCheckBox      *checkbox1,
               *checkbox2,
               *checkbox3,
               *checkbox3_1,
               *checkbox4,
               *checkbox4_1,
               *checkbox4_2,
               *checkbox4_3,
               *checkbox4_4,
               *checkbox4_5,
               *checkbox4_6,
               *checkbox2_1,
               *checkbox2_2;

QTableWidget   *colorBarTable;

QRadioButton   *radiobutton1,
               *radiobutton2,
               *radiobutton3;

QLineEdit      *lineEdit3_1;

QGroupBox      *groupbox1;

void update_interface(void);

private slots:

void BgColorButtonClicked(SpecialButton *);
void SrColorButtonClicked(SpecialButton *);
void BrColorButtonClicked(SpecialButton *);
void MrColorButtonClicked(SpecialButton *);
void TxtColorButtonClicked(SpecialButton *);
void SigColorButtonClicked(SpecialButton *);
void BaseColorButtonClicked(SpecialButton *);
void Crh1ColorButtonClicked(SpecialButton *);
void Crh2ColorButtonClicked(SpecialButton *);
void FrColorButtonClicked(SpecialButton *);
void AnnotMkrButtonClicked(SpecialButton *);
void AnnotDurationButtonClicked(SpecialButton *);
void checkbox1Clicked(int);
void checkbox2Clicked(int);
void checkbox2_1Clicked(int);
void checkbox2_2Clicked(int);
void checkbox3Clicked(int);
void checkbox4Clicked(int);
void checkbox3_1Clicked(int);
void checkbox4_1Clicked(int);
void checkbox4_2Clicked(int);
void checkbox4_3Clicked(int);
void checkbox4_4Clicked(int);
void checkbox4_5Clicked(int);
void checkbox4_6Clicked(int);
void ApplyButtonClicked();
void colorBarButtonClicked(SpecialButton *);
void DefaultButton2Clicked();
void checkBoxChanged(int);
void ApplyButton2Clicked();
void radioButtonToggled(bool);
void spinBoxValueChanged(double);
void spinBox3_1ValueChanged(int);
void dspinBox3_2ValueChanged(double);
void labelEdited(const QString  &);
void calibrate_checkbox_stateChanged(int);
void spinBox4_1ValueChanged(int);
void combobox4_1IndexChanged(int);
void combobox4_2IndexChanged(int);
void combobox4_3IndexChanged(int);
void spinBox4_2ValueChanged(int);
void spinBox4_3ValueChanged(int);
void saveColorSchemaButtonClicked();
void loadColorSchemaButtonClicked();
void loadColorSchema_NK();
void loadColorSchema_Dark();
void loadColorSchema_blue_gray();
void dspinbox4_4ValueChanged(double);

};



#endif // OPTIONSFORM1_H


