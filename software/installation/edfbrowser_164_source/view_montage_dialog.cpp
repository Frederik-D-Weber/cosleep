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



#include "view_montage_dialog.h"





UI_ViewMontagewindow::UI_ViewMontagewindow(QWidget *w_parent)
{
  mainwindow = (UI_Mainwindow *)w_parent;

  mtg_path[0] = 0;
  mtg_dir[0] = 0;

  if(mainwindow->recent_montagedir[0]!=0)
  {
    strcpy(mtg_dir, mainwindow->recent_montagedir);
  }

  ViewMontageDialog = new QDialog;

  ViewMontageDialog->setMinimumSize(500, 300);
  ViewMontageDialog->resize(500, 440);
  ViewMontageDialog->setWindowTitle("View montage");
  ViewMontageDialog->setModal(true);
  ViewMontageDialog->setAttribute(Qt::WA_DeleteOnClose, true);

  t_model = new QStandardItemModel(this);

  tree = new QTreeView;
  tree->setHeaderHidden(true);
  tree->setIndentation(30);
  tree->setSelectionMode(QAbstractItemView::NoSelection);
  tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree->setSortingEnabled(false);
  tree->setDragDropMode(QAbstractItemView::NoDragDrop);
  tree->setModel(t_model);
  tree->setAlternatingRowColors(true);

  SelectButton = new QPushButton;
  SelectButton->setFixedSize(100, 25);
  SelectButton->setText("Select montage");

  CloseButton = new QPushButton;
  CloseButton->setFixedSize(100, 25);
  CloseButton->setText("Close");

  hbox = new QHBoxLayout;
  hbox->addWidget(SelectButton, 0, Qt::AlignLeft);
  hbox->addWidget(CloseButton, 0, Qt::AlignRight);

  box = new QBoxLayout(QBoxLayout::TopToBottom, ViewMontageDialog);
  box->addWidget(tree);
  box->addLayout(hbox);

  QObject::connect(CloseButton,  SIGNAL(clicked()), ViewMontageDialog, SLOT(close()));
  QObject::connect(SelectButton, SIGNAL(clicked()), this,              SLOT(SelectButtonClicked()));

  ViewMontageDialog->exec();
}




void UI_ViewMontagewindow::SelectButtonClicked()
{
  int signalcomps_read=0,
      signals_read,
      signal_cnt,
      filters_read,
      color,
      filter_cnt=0,
      spike_filter_cnt=0,
      ravg_filter_cnt=0,
      fidfilter_cnt=0,
      islpf,
      factor[MAXSIGNALS],
      signalcomps,
      screen_offset,
      order,
      model,
      type,
      size,
      polarity=1,
      holdoff=100,
      plif_powerlinefrequency,
      plif_linear_threshold;

  char result[XML_STRBUFLEN],
       composition_txt[2048],
       label[256];

  double frequency,
         frequency2,
         voltpercm,
         ripple,
         timescale,
         d_tmp,
         velocity;

  QStandardItem *parentItem,
                *signalItem,
                *filterItem;

  struct xml_handle *xml_hdl;


  strcpy(mtg_path, QFileDialog::getOpenFileName(0, "Choose a montage", QString::fromLocal8Bit(mtg_dir), "Montage files (*.mtg *.MTG)").toLocal8Bit().data());

  if(!strcmp(mtg_path, ""))
  {
    return;
  }

  get_directory_from_path(mtg_dir, mtg_path, MAX_PATH_LENGTH);

  xml_hdl = xml_get_handle(mtg_path);
  if(xml_hdl==NULL)
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "Can not open file for reading.");
    messagewindow.exec();
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], PROGRAM_NAME "_montage"))
  {
    QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
    messagewindow.exec();
    xml_close(xml_hdl);
    return;
  }

  ViewMontageDialog->setWindowTitle(mtg_path);

  t_model->clear();

  parentItem = t_model->invisibleRootItem();

  for(signalcomps=0; ; signalcomps++)
  {
    xml_goto_root(xml_hdl);

    signals_read = 0;

    filter_cnt = 0;
    ravg_filter_cnt = 0;
    fidfilter_cnt = 0;

    if(xml_goto_nth_element_inside(xml_hdl, "signalcomposition", signalcomps_read))
    {
      break;
    }

    if(xml_goto_nth_element_inside(xml_hdl, "num_of_signals", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    signal_cnt = atoi(result);
    if((signal_cnt<1)||(signal_cnt>256))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }

    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "voltpercm", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    voltpercm = atof(result);

    xml_go_up(xml_hdl);
    if(xml_goto_nth_element_inside(xml_hdl, "screen_offset", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    screen_offset = atoi(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "polarity", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      polarity = atoi(result);
      if(polarity != -1)
      {
        polarity = 1;
      }
      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    color = atoi(result);
    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "spike_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      spike_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ravg_filter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      ravg_filter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "fidfilter_cnt", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      fidfilter_cnt = atoi(result);
      xml_go_up(xml_hdl);
    }

    composition_txt[0] = 0;

    if(!(xml_goto_nth_element_inside(xml_hdl, "alias", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(result[0] != 0)
      {
        strcpy(composition_txt, "alias: ");
        strcat(composition_txt, result);
        strcat(composition_txt, "   ");
      }
      xml_go_up(xml_hdl);
    }

    for(signals_read=0; signals_read<signal_cnt; signals_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "signal", signals_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "factor", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      factor[signals_read] = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "label", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      strcpy(label, result);

      sprintf(composition_txt + strlen(composition_txt), "%+ix %s", factor[signals_read], label);

      remove_trailing_spaces(composition_txt);

      strcat(composition_txt, "   ");

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(polarity == -1)
    {
      strcat(composition_txt, "inverted: yes");
    }

    signalItem = new QStandardItem(composition_txt);

    parentItem->appendRow(signalItem);

    sprintf(composition_txt, "amplitude: %f", voltpercm);

    remove_trailing_zeros(composition_txt);

    sprintf(composition_txt + strlen(composition_txt), "/cm  offset: %f", (double)screen_offset * mainwindow->pixelsizefactor * voltpercm);

    remove_trailing_zeros(composition_txt);

    strcat(composition_txt, "  color: ");

    switch(color)
    {
      case Qt::white       : strcat(composition_txt, "white");
                             signalItem->setIcon(QIcon(":/images/white_icon_16x16"));
                             break;
      case Qt::black       : strcat(composition_txt, "black");
                             signalItem->setIcon(QIcon(":/images/black_icon_16x16"));
                             break;
      case Qt::red         : strcat(composition_txt, "red");
                             signalItem->setIcon(QIcon(":/images/red_icon_16x16"));
                             break;
      case Qt::darkRed     : strcat(composition_txt, "dark red");
                             signalItem->setIcon(QIcon(":/images/darkred_icon_16x16"));
                             break;
      case Qt::green       : strcat(composition_txt, "green");
                             signalItem->setIcon(QIcon(":/images/green_icon_16x16"));
                             break;
      case Qt::darkGreen   : strcat(composition_txt, "dark green");
                             signalItem->setIcon(QIcon(":/images/darkgreen_icon_16x16"));
                             break;
      case Qt::blue        : strcat(composition_txt, "blue");
                             signalItem->setIcon(QIcon(":/images/blue_icon_16x16"));
                             break;
      case Qt::darkBlue    : strcat(composition_txt, "dark blue");
                             signalItem->setIcon(QIcon(":/images/darkblue_icon_16x16"));
                             break;
      case Qt::cyan        : strcat(composition_txt, "cyan");
                             signalItem->setIcon(QIcon(":/images/cyan_icon_16x16"));
                             break;
      case Qt::darkCyan    : strcat(composition_txt, "dark cyan");
                             signalItem->setIcon(QIcon(":/images/darkcyan_icon_16x16"));
                             break;
      case Qt::magenta     : strcat(composition_txt, "magenta");
                             signalItem->setIcon(QIcon(":/images/magenta_icon_16x16"));
                             break;
      case Qt::darkMagenta : strcat(composition_txt, "dark magenta");
                             signalItem->setIcon(QIcon(":/images/darkmagenta_icon_16x16"));
                             break;
      case Qt::yellow      : strcat(composition_txt, "yellow");
                             signalItem->setIcon(QIcon(":/images/yellow_icon_16x16"));
                             break;
      case Qt::darkYellow  : strcat(composition_txt, "dark yellow");
                             signalItem->setIcon(QIcon(":/images/darkyellow_icon_16x16"));
                             break;
      case Qt::gray        : strcat(composition_txt, "gray");
                             signalItem->setIcon(QIcon(":/images/gray_icon_16x16"));
                             break;
      case Qt::darkGray    : strcat(composition_txt, "dark gray");
                             signalItem->setIcon(QIcon(":/images/darkgray_icon_16x16"));
                             break;
      case Qt::lightGray   : strcat(composition_txt, "light gray");
                             signalItem->setIcon(QIcon(":/images/lightgray_icon_16x16"));
                             break;
    }

    signalItem->appendRow(new QStandardItem(composition_txt));

    filterItem = new QStandardItem("Filters");

    filterItem->setIcon(QIcon(":/images/filter_lowpass_small.png"));

    signalItem->appendRow(filterItem);

    for(filters_read=0; filters_read<spike_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "spike_filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "velocity", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (velocity)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      velocity = atof(result);
      if(velocity < 0.0001)  velocity = 0.0001;
      if(velocity > 10E9)  velocity = 10E9;

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "holdoff", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (holdoff)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      holdoff = atoi(result);
      if(holdoff < 10)  holdoff = 10;
      if(holdoff > 1000)  holdoff = 1000;

      sprintf(composition_txt, "Spike: Velocity: %.8f", velocity);

      remove_trailing_zeros(composition_txt);

      sprintf(composition_txt + strlen(composition_txt), "  Hold-off: %i milli-Sec.", holdoff);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "LPF", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (LPF)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      islpf = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency = atof(result);

      if(islpf)
      {
        sprintf(composition_txt, "LPF: %fHz", frequency);
      }
      else
      {
        sprintf(composition_txt, "HPF: %fHz", frequency);
      }

      remove_trailing_zeros(composition_txt);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<ravg_filter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "ravg_filter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (filter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (type)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "size", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (size)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      size = atoi(result);

      if(type == 0)
      {
        sprintf(composition_txt, "highpass moving average %ismpls", size);
      }

      if(type == 1)
      {
        sprintf(composition_txt, "lowpass moving average %ismpls", size);
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    for(filters_read=0; filters_read<fidfilter_cnt; filters_read++)
    {
      if(xml_goto_nth_element_inside(xml_hdl, "fidfilter", filters_read))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (fidfilter)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }

      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (type)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "frequency2", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (frequency2)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      frequency2 = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "ripple", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (ripple)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      ripple = atof(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "order", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (order)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      order = atoi(result);

      xml_go_up(xml_hdl);
      if(xml_goto_nth_element_inside(xml_hdl, "model", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file. (model)");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      model = atoi(result);

      if(type == 0)
      {
        if(model == 0)
        {
          sprintf(composition_txt, "highpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          sprintf(composition_txt, "highpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          sprintf(composition_txt, "highpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 1)
      {
        if(model == 0)
        {
          sprintf(composition_txt, "lowpass Butterworth %fHz %ith order  ", frequency, order);
        }

        if(model == 1)
        {
          sprintf(composition_txt, "lowpass Chebyshev %fHz %ith order %fdB ripple  ", frequency, order, ripple);
        }

        if(model == 2)
        {
          sprintf(composition_txt, "lowpass Bessel %fHz %ith order  ", frequency, order);
        }
      }

      if(type == 2)
      {
        sprintf(composition_txt, "notch %fHz Q-factor %i  ", frequency, order);
      }

      if(type == 3)
      {
        if(model == 0)
        {
          sprintf(composition_txt, "bandpass Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          sprintf(composition_txt, "bandpass Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          sprintf(composition_txt, "bandpass Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      if(type == 4)
      {
        if(model == 0)
        {
          sprintf(composition_txt, "bandstop Butterworth %f-%fHz %ith order  ", frequency, frequency2, order);
        }

        if(model == 1)
        {
          sprintf(composition_txt, "bandstop Chebyshev %f-%fHz %ith order %fdB ripple  ", frequency, frequency2, order, ripple);
        }

        if(model == 2)
        {
          sprintf(composition_txt, "bandstop Bessel %f-%fHz %ith order  ", frequency, frequency2, order);
        }
      }

      remove_trailing_zeros(composition_txt);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "plif_ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "plf", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_powerlinefrequency = atoi(result);
      if((plif_powerlinefrequency != 0) && (plif_powerlinefrequency != 1))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_powerlinefrequency *= 10;
      plif_powerlinefrequency += 50;
      xml_go_up(xml_hdl);

      if(xml_goto_nth_element_inside(xml_hdl, "linear_threshold", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      plif_linear_threshold = atoi(result);
      if((plif_linear_threshold < 10) || (plif_linear_threshold > 200))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      xml_go_up(xml_hdl);

      sprintf(composition_txt, "Powerline interference removal: %iHz  threshold: %iuV", plif_powerlinefrequency, plif_linear_threshold);

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "ecg_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      if(type == 1)
      {
        sprintf(composition_txt, "ECG heartrate detection");
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "zratio_filter", 0))
    {
      if(xml_goto_nth_element_inside(xml_hdl, "type", 0))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
        messagewindow.exec();
        xml_close(xml_hdl);
        return;
      }
      type = atoi(result);

      xml_go_up(xml_hdl);

      if(type == 1)
      {
        if(xml_goto_nth_element_inside(xml_hdl, "crossoverfreq", 0))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
          messagewindow.exec();
          xml_close(xml_hdl);
          return;
        }
        d_tmp = atof(result);

        sprintf(composition_txt, "Z-ratio  cross-over frequency is %.1f", d_tmp);
      }

      filterItem->appendRow(new QStandardItem(composition_txt));

      xml_go_up(xml_hdl);
      xml_go_up(xml_hdl);
    }

    signalcomps_read++;
  }

  xml_goto_root(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "pagetime", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      QMessageBox messagewindow(QMessageBox::Critical, "Error", "There seems to be an error in this montage file.");
      messagewindow.exec();
      xml_close(xml_hdl);
      return;
    }
    timescale = atof(result);
    timescale /= TIME_DIMENSION;
    sprintf(composition_txt, "timescale: %f seconds", timescale);
    remove_trailing_zeros(composition_txt);
    parentItem->appendRow(new QStandardItem(composition_txt));
  }

  xml_close(xml_hdl);

  tree->expandAll();
}













