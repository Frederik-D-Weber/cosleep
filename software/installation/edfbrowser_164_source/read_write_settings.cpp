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


#include "mainwindow.h"


void UI_Mainwindow::get_rgbcolor_settings(struct xml_handle *xml_hdl, const char *id, int cnt, QColor *rgb_color)
{
  char result[XML_STRBUFLEN];

  QColor tmp_color;


  if(xml_goto_nth_element_inside(xml_hdl, id, cnt))
  {
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "red", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setRed(atoi(result));

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "green", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setGreen(atoi(result));

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "blue", 0))
  {
    xml_go_up(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_go_up(xml_hdl);
    xml_go_up(xml_hdl);
    return;
  }
  tmp_color.setBlue(atoi(result));

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "alpha", 0))
  {
    if(!xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      tmp_color.setAlpha(atoi(result));
    }

    xml_go_up(xml_hdl);
  }

  *rgb_color = tmp_color;

  xml_go_up(xml_hdl);
}


void UI_Mainwindow::read_color_settings()
{
  char cfg_path[MAX_PATH_LENGTH],
       result[XML_STRBUFLEN];

  struct xml_handle *xml_hdl;

  cfg_path[0] = 0;

#ifdef Q_OS_WIN32
  strcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data());
  strcat(cfg_path, "\\");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "\\settings.xml");
#else
  strcpy(cfg_path, getenv("HOME"));
  strcat(cfg_path, "/.");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "/settings.xml");
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(!xml_hdl)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "colors", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  get_rgbcolor_settings(xml_hdl, "backgroundcolor", 0, &maincurve->backgroundcolor);

  get_rgbcolor_settings(xml_hdl, "small_ruler_color", 0, &maincurve->small_ruler_color);

  get_rgbcolor_settings(xml_hdl, "big_ruler_color", 0, &maincurve->big_ruler_color);

  get_rgbcolor_settings(xml_hdl, "mouse_rect_color", 0, &maincurve->mouse_rect_color);

  get_rgbcolor_settings(xml_hdl, "text_color", 0, &maincurve->text_color);

  get_rgbcolor_settings(xml_hdl, "baseline_color", 0, &maincurve->baseline_color);

  get_rgbcolor_settings(xml_hdl, "annot_marker_color", 0, &maincurve->annot_marker_color);

  get_rgbcolor_settings(xml_hdl, "annot_duration_color", 0, &maincurve->annot_duration_color);

  if(xml_goto_nth_element_inside(xml_hdl, "signal_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->signal_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "floating_ruler_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->floating_ruler_color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "blackwhite_printing", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->blackwhite_printing = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "show_annot_markers", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  show_annot_markers = atoi(result);

  xml_go_up(xml_hdl);

  if(!xml_goto_nth_element_inside(xml_hdl, "show_annot_duration", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    annotations_show_duration = atoi(result);

    xml_go_up(xml_hdl);
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "annotations_duration_background_type", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    annotations_duration_background_type = atoi(result);

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "show_baselines", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  show_baselines = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_1_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->crosshair_1.color = atoi(result);

  xml_go_up(xml_hdl);

  if(xml_goto_nth_element_inside(xml_hdl, "crosshair_2_color", 0))
  {
    xml_close(xml_hdl);
    return;
  }
  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }
  maincurve->crosshair_2.color = atoi(result);

  xml_close(xml_hdl);
}



void UI_Mainwindow::read_recent_file_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH],
       result[XML_STRBUFLEN];

  struct xml_handle *xml_hdl;

  cfg_path[0] = 0;

  QAction *act;

#ifdef Q_OS_WIN32
  strcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data());
  strcat(cfg_path, "\\");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "\\settings.xml");
#else
  strcpy(cfg_path, getenv("HOME"));
  strcat(cfg_path, "/.");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "/settings.xml");
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(xml_hdl==NULL)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_montagedir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strncpy(recent_montagedir, result, MAX_PATH_LENGTH);
    recent_montagedir[MAX_PATH_LENGTH - 1] = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_savedir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strncpy(recent_savedir, result, MAX_PATH_LENGTH);
    recent_savedir[MAX_PATH_LENGTH - 1] = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_opendir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strncpy(recent_opendir, result, MAX_PATH_LENGTH);
    recent_opendir[MAX_PATH_LENGTH - 1] = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_colordir", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strncpy(recent_colordir, result, MAX_PATH_LENGTH);
    recent_opendir[MAX_PATH_LENGTH - 1] = 0;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_file", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    if(result[0] != 0)
    {
      strncpy(&recent_file_path[0][0], result, MAX_PATH_LENGTH);
      recent_file_path[0][MAX_PATH_LENGTH - 1] = 0;
      act = new QAction(QString::fromLocal8Bit(&recent_file_path[0][0]), recent_filesmenu);
      act->setData(QVariant(0));
      recent_filesmenu->addAction(act);

      for(i=1; i<MAX_RECENTFILES; i++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          break;
        }
        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }
        if(result[0] == 0)
        {
          break;
        }
        strncpy(&recent_file_path[i][0], result, MAX_PATH_LENGTH);
        recent_file_path[i][MAX_PATH_LENGTH - 1] = 0;
        act = new QAction(QString::fromLocal8Bit(&recent_file_path[i][0]), recent_filesmenu);
        act->setData(QVariant(i));
        recent_filesmenu->addAction(act);
      }
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "recent_file_mtg", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    strncpy(&recent_file_mtg_path[0][0], result, MAX_PATH_LENGTH);
    recent_file_mtg_path[0][MAX_PATH_LENGTH - 1] = 0;
    for(i=1; i<MAX_RECENTFILES; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        break;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }
      strncpy(&recent_file_mtg_path[i][0], result, MAX_PATH_LENGTH);
      recent_file_mtg_path[i][MAX_PATH_LENGTH - 1] = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "predefined_mtg_path", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }
    strncpy(&predefined_mtg_path[0][0], result, MAX_PATH_LENGTH);
    predefined_mtg_path[0][MAX_PATH_LENGTH - 1] = 0;

    for(i=1; i < MAXPREDEFINEDMONTAGES; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        break;
      }
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        break;
      }
      strncpy(&predefined_mtg_path[i][0], result, MAX_PATH_LENGTH);
      predefined_mtg_path[i][MAX_PATH_LENGTH - 1] = 0;
    }

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);
}



void UI_Mainwindow::read_general_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH],
       result[XML_STRBUFLEN];

  struct xml_handle *xml_hdl;

  cfg_path[0] = 0;

#ifdef Q_OS_WIN32
  strcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data());
  strcat(cfg_path, "\\");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "\\settings.xml");
#else
  strcpy(cfg_path, getenv("HOME"));
  strcat(cfg_path, "/.");
  strcat(cfg_path, PROGRAM_NAME);
  strcat(cfg_path, "/settings.xml");
#endif

  xml_hdl = xml_get_handle(cfg_path);
  if(xml_hdl==NULL)
  {
    return;
  }

  if(strcmp(xml_hdl->elementname[xml_hdl->level], "config"))
  {
    xml_close(xml_hdl);
    return;
  }

  if(!xml_goto_nth_element_inside(xml_hdl, "cfg_app_version", 0))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    strncpy(cfg_app_version, result, 16);
    cfg_app_version[16] = 0;

    xml_go_up(xml_hdl);
  }

  if(xml_goto_nth_element_inside(xml_hdl, "UI", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_goto_nth_element_inside(xml_hdl, "pixelsizefactor", 0))
  {
    xml_close(xml_hdl);
    return;
  }

  if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
  {
    xml_close(xml_hdl);
    return;
  }

  pixelsizefactor = atof(result);
  if(pixelsizefactor < 0.00001)
  {
    pixelsizefactor = 0.0294382;
  }

  xml_go_up(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "auto_dpi", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    auto_dpi = atoi(result);
    if((auto_dpi < 0) || (auto_dpi > 1))
    {
      auto_dpi = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "x_pixelsizefactor", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    x_pixelsizefactor = atof(result);
    if(x_pixelsizefactor < 0.00001)
    {
      x_pixelsizefactor = 0.0294382;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "clip_to_pane", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    clip_to_pane = atoi(result);
    if((clip_to_pane < 0) || (clip_to_pane > 1))
    {
      clip_to_pane = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "auto_reload_mtg", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    auto_reload_mtg = atoi(result);
    if((auto_reload_mtg < 0) || (auto_reload_mtg > 1))
    {
      auto_reload_mtg = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "read_biosemi_status_signal", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    read_biosemi_status_signal = atoi(result);
    if((read_biosemi_status_signal < 0) || (read_biosemi_status_signal > 1))
    {
      read_biosemi_status_signal = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "read_nk_trigger_signal", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    read_nk_trigger_signal = atoi(result);
    if((read_nk_trigger_signal < 0) || (read_nk_trigger_signal > 1))
    {
      read_nk_trigger_signal = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "maxfilesize_to_readin_annotations", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    maxfilesize_to_readin_annotations = atoll(result);
    if((maxfilesize_to_readin_annotations < 104857600LL) || (maxfilesize_to_readin_annotations > 10485760000LL))
    {
      maxfilesize_to_readin_annotations = 10485760000LL;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "use_threads", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    use_threads = atoi(result);
    if((use_threads < 0) || (use_threads > 1))
    {
      use_threads = 1;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrummarkerblock", 0)))
  {
    if(xml_goto_nth_element_inside(xml_hdl, "items", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > MAXSPECTRUMMARKERS) || (atoi(result) < 0))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->items = atoi(result);

    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "method", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > 2) || (atoi(result) < 0))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->method = atoi(result);

    xml_go_up(xml_hdl);

    if(!xml_goto_nth_element_inside(xml_hdl, "auto_adjust", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->auto_adjust = atoi(result);

      if((spectrum_colorbar->auto_adjust > 1) || (spectrum_colorbar->auto_adjust < 0))
      {
        spectrum_colorbar->auto_adjust = 1;
      }

      xml_go_up(xml_hdl);
    }

    if(!xml_goto_nth_element_inside(xml_hdl, "max_colorbar_value", 0))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->max_colorbar_value = atof(result);

      if((spectrum_colorbar->max_colorbar_value > 100000.0) || (spectrum_colorbar->max_colorbar_value < 0.0001))
      {
        spectrum_colorbar->max_colorbar_value = 1.0;
      }

      xml_go_up(xml_hdl);
    }

    if(xml_goto_nth_element_inside(xml_hdl, "frequency", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atof(result) > 1000000.0) || (atof(result) < 0.00001))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->freq[0] = atof(result);

    for(i=1; i < spectrum_colorbar->items; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        xml_close(xml_hdl);
        return;
      }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if(i < spectrum_colorbar->items)
      {
        if((atof(result) > 1000000.0) || (atof(result) <= spectrum_colorbar->freq[i-1]))
        {
          xml_close(xml_hdl);
          return;
        }
      }

      spectrum_colorbar->freq[i] = atof(result);
    }

    xml_go_up(xml_hdl);

    if(xml_goto_nth_element_inside(xml_hdl, "color", 0))
    {
      xml_close(xml_hdl);
      return;
    }

    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if((atoi(result) > 18) || (atoi(result) < 2))
    {
      xml_close(xml_hdl);
      return;
    }

    spectrum_colorbar->color[0] = atoi(result);

    for(i=1; i < spectrum_colorbar->items; i++)
    {
      if(xml_goto_next_element_with_same_name(xml_hdl))
      {
        xml_close(xml_hdl);
        return;
      }

      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      if((atoi(result) > 18) || (atoi(result) < 2))
      {
        xml_close(xml_hdl);
        return;
      }

      spectrum_colorbar->color[i] = atoi(result);
    }

    xml_go_up(xml_hdl);

    if(!(xml_goto_nth_element_inside(xml_hdl, "label", 0)))
    {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strncpy(spectrum_colorbar->label[0], result, 16);
      spectrum_colorbar->label[0][16] = 0;

      for(i=1; i < spectrum_colorbar->items; i++)
      {
        if(xml_goto_next_element_with_same_name(xml_hdl))
        {
          xml_close(xml_hdl);
          return;
        }

        if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
        {
          xml_close(xml_hdl);
          return;
        }

        strncpy(spectrum_colorbar->label[i], result, 16);
        spectrum_colorbar->label[i][16] = 0;
      }

      xml_go_up(xml_hdl);
    }
  }

  xml_go_up(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "maxdftblocksize", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    maxdftblocksize = atoi(result);

    if(maxdftblocksize > (32768 * 512))
    {
      maxdftblocksize = 32768 * 512;
    }

    if(maxdftblocksize < 10)
    {
      maxdftblocksize = 1000;
    }
  }

  xml_go_up(xml_hdl);

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotations_import_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "format", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->format = atoi(result);

      if((import_annotations_var->format < 0) || (import_annotations_var->format > 4))
      {
        import_annotations_var->format = 1;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "onsettimeformat", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->onsettimeformat = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "onsetcolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->onsetcolumn = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "descriptioncolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->descriptioncolumn = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "useduration", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->useduration = atoi(result);

      if(import_annotations_var->useduration != 1)
      {
        import_annotations_var->useduration = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "durationcolumn", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->durationcolumn = atoi(result);

      if(import_annotations_var->durationcolumn < 0)
      {
        import_annotations_var->durationcolumn = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "datastartline", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->datastartline = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "separator", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strncpy(import_annotations_var->separator, result, 3);

      import_annotations_var->separator[3] = 0;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "dceventbittime", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->dceventbittime = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "triggerlevel", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->triggerlevel = atof(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "manualdescription", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->manualdescription = atoi(result);

      if((import_annotations_var->manualdescription < 0) || (import_annotations_var->manualdescription > 1))
      {
        import_annotations_var->manualdescription = 0;
      }

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "description", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strncpy(import_annotations_var->description, result, 20);

      import_annotations_var->description[20] = 0;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "ignoreconsecutive", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      import_annotations_var->ignoreconsecutive = atoi(result);

      if(import_annotations_var->ignoreconsecutive != 1)
      {
        import_annotations_var->ignoreconsecutive = 0;
      }

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotations_export_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "separator", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->separator = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "format", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->format = atoi(result);

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "duration", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      export_annotations_var->duration = atoi(result);

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "live_stream_update_interval", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    live_stream_update_interval = atoi(result);
    if((live_stream_update_interval < 100) || (live_stream_update_interval > 3000))
    {
      live_stream_update_interval = 500;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "powerlinefreq", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    powerlinefreq = atoi(result);
    if((powerlinefreq != 50) && (powerlinefreq != 60))
    {
      powerlinefreq = 50;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mousewheelsens", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mousewheelsens = atoi(result);
    if((mousewheelsens < 0) || (mousewheelsens > 100))
    {
      mousewheelsens = 10;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_period", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    average_period = atoi(result);

    if(average_period < 1)  average_period = 300;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_ratio", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    average_ratio = atoi(result);
    if((average_ratio < 0) || (average_ratio > 2))
    {
      average_ratio = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_upsidedown", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      average_upsidedown = 1;
    }
    else
    {
      average_upsidedown = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "average_bw", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      average_bw = 1;
    }
    else
    {
      average_bw = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_bw", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_bw = 1;
    }
    else
    {
      spectrum_bw = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_sqrt", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_sqrt = 1;
    }
    else
    {
      spectrum_sqrt = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrum_vlog", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrum_vlog = 1;
    }
    else
    {
      spectrum_vlog = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_sqrt", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_sqrt = 1;
    }
    else
    {
      spectrumdock_sqrt = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_colorbars", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_colorbars = 1;
    }
    else
    {
      spectrumdock_colorbars = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "spectrumdock_vlog", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    if(atoi(result) == 1)
    {
      spectrumdock_vlog = 1;
    }
    else
    {
      spectrumdock_vlog = 0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.zscore_page_len", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.zscore_page_len = atoi(result);

    if((z_score_var.zscore_page_len < 10) || (z_score_var.zscore_page_len > 60))
    {
      z_score_var.zscore_page_len = 30;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.z_threshold", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.z_threshold = atof(result);

    if((z_score_var.z_threshold < -0.5) || (z_score_var.z_threshold > 0.5))
    {
      z_score_var.z_threshold = 0.0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.crossoverfreq", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.crossoverfreq = atof(result);

    if((z_score_var.crossoverfreq < 5.0) || (z_score_var.crossoverfreq > 9.5))
    {
      z_score_var.crossoverfreq = 7.5;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.z_hysteresis", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.z_hysteresis = atof(result);

    if((z_score_var.z_hysteresis < 0.0) || (z_score_var.z_hysteresis > 0.25))
    {
      z_score_var.z_hysteresis = 0.0;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "z_score_var.zscore_error_detection", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    z_score_var.zscore_error_detection = atoi(result);

    if((z_score_var.zscore_error_detection < 50) || (z_score_var.zscore_error_detection > 100))
    {
      z_score_var.zscore_error_detection = 50;
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "raw2edf_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "sf", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.sf = atoi(result);
      if(raw2edf_var.sf < 1)  raw2edf_var.sf = 1;
      if(raw2edf_var.sf > 1000000)  raw2edf_var.sf = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "chns", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.chns = atoi(result);
      if(raw2edf_var.chns < 1)  raw2edf_var.chns = 1;
      if(raw2edf_var.chns > 256)  raw2edf_var.chns = 256;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "phys_max", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.phys_max = atoi(result);
      if(raw2edf_var.phys_max < 1)  raw2edf_var.phys_max = 1;
      if(raw2edf_var.phys_max > 10000000)  raw2edf_var.phys_max = 10000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "straightbinary", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.straightbinary = atoi(result);
      if(raw2edf_var.straightbinary < 0)  raw2edf_var.straightbinary = 0;
      if(raw2edf_var.straightbinary > 1)  raw2edf_var.straightbinary = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "endianness", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.endianness = atoi(result);
      if(raw2edf_var.endianness < 0)  raw2edf_var.endianness = 0;
      if(raw2edf_var.endianness > 1)  raw2edf_var.endianness = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "samplesize", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.samplesize = atoi(result);
      if(raw2edf_var.samplesize < 1)  raw2edf_var.samplesize = 1;
      if(raw2edf_var.samplesize > 2)  raw2edf_var.samplesize = 2;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "offset", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.offset = atoi(result);
      if(raw2edf_var.offset < 0)  raw2edf_var.offset = 0;
      if(raw2edf_var.offset > 1000000)  raw2edf_var.offset = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "skipblocksize", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.skipblocksize = atoi(result);
      if(raw2edf_var.skipblocksize < 0)  raw2edf_var.skipblocksize = 0;
      if(raw2edf_var.skipblocksize > 1000000)  raw2edf_var.skipblocksize = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "skipbytes", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      raw2edf_var.skipbytes = atoi(result);
      if(raw2edf_var.skipbytes < 1)  raw2edf_var.skipbytes = 1;
      if(raw2edf_var.skipbytes > 1000000)  raw2edf_var.skipbytes = 1000000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "phys_dim", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      strncpy(raw2edf_var.phys_dim, result, 16);
      latin1_to_ascii(raw2edf_var.phys_dim, 16);
      raw2edf_var.phys_dim[15] = 0;
      remove_leading_spaces(raw2edf_var.phys_dim);
      remove_trailing_spaces(raw2edf_var.phys_dim);

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "annotfilter_var", 0)))
  {
    if(!(xml_goto_nth_element_inside(xml_hdl, "tmin", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->tmin = atoi(result);
      if(annot_filter->tmin < 1)  annot_filter->tmin = 1;
      if(annot_filter->tmin > 500000)  annot_filter->tmin = 500000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "tmax", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->tmax = atoi(result);
      if(annot_filter->tmax < 1)  annot_filter->tmax = 1;
      if(annot_filter->tmax > 500000)  annot_filter->tmax = 500000;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "invert", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->invert = atoi(result);
      if(annot_filter->invert < 0)  annot_filter->invert = 0;
      if(annot_filter->invert > 1)  annot_filter->invert = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hide_other", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->hide_other = atoi(result);
      if(annot_filter->hide_other < 0)  annot_filter->hide_other = 0;
      if(annot_filter->hide_other > 1)  annot_filter->hide_other = 1;

      xml_go_up(xml_hdl);
    }

    if(!(xml_goto_nth_element_inside(xml_hdl, "hide_in_list_only", 0)))
    {
      if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
      {
        xml_close(xml_hdl);
        return;
      }

      annot_filter->hide_in_list_only = atoi(result);
      if(annot_filter->hide_in_list_only < 0)  annot_filter->hide_in_list_only = 0;
      if(annot_filter->hide_in_list_only > 1)  annot_filter->hide_in_list_only = 1;

      xml_go_up(xml_hdl);
    }

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "check_for_updates", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    check_for_updates = atoi(result);
    if((check_for_updates < 0) || (check_for_updates > 1))  check_for_updates = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "viewtime_indicator_type", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    viewtime_indicator_type = atoi(result);
    if((viewtime_indicator_type < 0) || (viewtime_indicator_type > 2))  viewtime_indicator_type = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "mainwindow_title_type", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    mainwindow_title_type = atoi(result);
    if((mainwindow_title_type < 0) || (mainwindow_title_type > 2))  mainwindow_title_type = 1;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "default_amplitude", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    default_amplitude = atof(result);
    if((default_amplitude < 0.001) || (default_amplitude > 10000000))  default_amplitude = 100;

    xml_go_up(xml_hdl);
  }

  if(!(xml_goto_nth_element_inside(xml_hdl, "linear_interpolation", 0)))
  {
    if(xml_get_content_of_element(xml_hdl, result, XML_STRBUFLEN))
    {
      xml_close(xml_hdl);
      return;
    }

    linear_interpol = atoi(result);
    if(linear_interpol != 1)
    {
      linear_interpol = 0;
    }

    xml_go_up(xml_hdl);
  }

  xml_close(xml_hdl);
}


void UI_Mainwindow::write_settings()
{
  int i;

  char cfg_path[MAX_PATH_LENGTH],
       str[1024];

  FILE *cfgfile;

  cfg_path[0] = 0;

#ifdef Q_OS_WIN32
  strcpy(cfg_path, specialFolder(CSIDL_APPDATA).toLocal8Bit().data());
  strcat(cfg_path, "\\");
  strcat(cfg_path, PROGRAM_NAME);
  mkdir(cfg_path);
  strcat(cfg_path, "\\settings.xml");
#else
  strcpy(cfg_path, getenv("HOME"));
  strcat(cfg_path, "/.");
  strcat(cfg_path, PROGRAM_NAME);
  mkdir(cfg_path,  S_IRWXU);
  strcat(cfg_path, "/settings.xml");
#endif

  cfgfile = fopeno(cfg_path, "wb");
  if(cfgfile)
  {
    fprintf(cfgfile, "<?xml version=\"1.0\"?>\n"
                     "<config>\n"
                     "  <cfg_app_version>" PROGRAM_NAME " " PROGRAM_VERSION "</cfg_app_version>\n"
                     "  <UI>\n"
                     "    <colors>\n");

    fprintf(cfgfile, "      <backgroundcolor>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </backgroundcolor>\n",
                    maincurve->backgroundcolor.red(),
                    maincurve->backgroundcolor.green(),
                    maincurve->backgroundcolor.blue());

    fprintf(cfgfile, "      <small_ruler_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </small_ruler_color>\n",
                    maincurve->small_ruler_color.red(),
                    maincurve->small_ruler_color.green(),
                    maincurve->small_ruler_color.blue());

    fprintf(cfgfile, "      <big_ruler_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </big_ruler_color>\n",
                    maincurve->big_ruler_color.red(),
                    maincurve->big_ruler_color.green(),
                    maincurve->big_ruler_color.blue());

    fprintf(cfgfile, "      <mouse_rect_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </mouse_rect_color>\n",
                    maincurve->mouse_rect_color.red(),
                    maincurve->mouse_rect_color.green(),
                    maincurve->mouse_rect_color.blue());

    fprintf(cfgfile, "      <text_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </text_color>\n",
                    maincurve->text_color.red(),
                    maincurve->text_color.green(),
                    maincurve->text_color.blue());

    fprintf(cfgfile, "      <baseline_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </baseline_color>\n",
                    maincurve->baseline_color.red(),
                    maincurve->baseline_color.green(),
                    maincurve->baseline_color.blue());

    fprintf(cfgfile, "      <annot_marker_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "      </annot_marker_color>\n",
                    maincurve->annot_marker_color.red(),
                    maincurve->annot_marker_color.green(),
                    maincurve->annot_marker_color.blue());

    fprintf(cfgfile, "      <annot_duration_color>\n"
                    "        <red>%i</red>\n"
                    "        <green>%i</green>\n"
                    "        <blue>%i</blue>\n"
                    "        <alpha>%i</alpha>\n"
                    "      </annot_duration_color>\n",
                    maincurve->annot_duration_color.red(),
                    maincurve->annot_duration_color.green(),
                    maincurve->annot_duration_color.blue(),
                    maincurve->annot_duration_color.alpha());

    fprintf(cfgfile, "      <signal_color>%i</signal_color>\n",
                    maincurve->signal_color);

    fprintf(cfgfile, "      <crosshair_1_color>%i</crosshair_1_color>\n",
                    maincurve->crosshair_1.color);

    fprintf(cfgfile, "      <crosshair_2_color>%i</crosshair_2_color>\n",
                    maincurve->crosshair_2.color);

    fprintf(cfgfile, "      <floating_ruler_color>%i</floating_ruler_color>\n",
                    maincurve->floating_ruler_color);

    fprintf(cfgfile, "      <blackwhite_printing>%i</blackwhite_printing>\n",
                    maincurve->blackwhite_printing);

    fprintf(cfgfile, "      <show_annot_markers>%i</show_annot_markers>\n",
                    show_annot_markers);

    fprintf(cfgfile, "      <show_annot_duration>%i</show_annot_duration>\n",
                    annotations_show_duration);

    fprintf(cfgfile, "      <annotations_duration_background_type>%i</annotations_duration_background_type>\n",
                    annotations_duration_background_type);

    fprintf(cfgfile, "      <show_baselines>%i</show_baselines>\n",
                    show_baselines);

    fprintf(cfgfile, "    </colors>\n");

    fprintf(cfgfile, "    <clip_to_pane>%i</clip_to_pane>\n",
                    clip_to_pane);

    fprintf(cfgfile, "    <auto_reload_mtg>%i</auto_reload_mtg>\n",
                    auto_reload_mtg);

    fprintf(cfgfile, "    <read_biosemi_status_signal>%i</read_biosemi_status_signal>\n",
                    read_biosemi_status_signal);

    fprintf(cfgfile, "    <read_nk_trigger_signal>%i</read_nk_trigger_signal>\n",
                    read_nk_trigger_signal);

    fprintf(cfgfile, "    <use_threads>%i</use_threads>\n",
                    use_threads);

#ifdef Q_OS_WIN32
    __mingw_fprintf(cfgfile, "    <maxfilesize_to_readin_annotations>%lli</maxfilesize_to_readin_annotations>\n",
                    maxfilesize_to_readin_annotations);
#else
    fprintf(cfgfile, "    <maxfilesize_to_readin_annotations>%lli</maxfilesize_to_readin_annotations>\n",
                    maxfilesize_to_readin_annotations);
#endif

    fprintf(cfgfile, "    <pixelsizefactor>%.10f</pixelsizefactor>\n    <auto_dpi>%i</auto_dpi>\n    <x_pixelsizefactor>%.10f</x_pixelsizefactor>\n    <recent_dir>",
                     pixelsizefactor, auto_dpi, x_pixelsizefactor);

    xml_fwrite_encode_entity(cfgfile, recent_opendir);

    fprintf(cfgfile, "</recent_dir>\n");

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      fprintf(cfgfile, "    <recent_file>");

      xml_fwrite_encode_entity(cfgfile, &recent_file_path[i][0]);

      fprintf(cfgfile, "</recent_file>\n");
    }

    for(i=0; i<MAX_RECENTFILES; i++)
    {
      fprintf(cfgfile, "    <recent_file_mtg>");

      xml_fwrite_encode_entity(cfgfile, &recent_file_mtg_path[i][0]);

      fprintf(cfgfile, "</recent_file_mtg>\n");
    }

    fprintf(cfgfile, "    <recent_montagedir>");

    xml_fwrite_encode_entity(cfgfile, recent_montagedir);

    fprintf(cfgfile, "</recent_montagedir>\n");

    fprintf(cfgfile, "    <recent_savedir>");

    xml_fwrite_encode_entity(cfgfile, recent_savedir);

    fprintf(cfgfile, "</recent_savedir>\n");

    fprintf(cfgfile, "    <recent_opendir>");

    xml_fwrite_encode_entity(cfgfile, recent_opendir);

    fprintf(cfgfile, "</recent_opendir>\n");

    fprintf(cfgfile, "    <recent_colordir>");

    xml_fwrite_encode_entity(cfgfile, recent_colordir);

    fprintf(cfgfile, "</recent_colordir>\n");

    for(i=0; i < MAXPREDEFINEDMONTAGES; i++)
    {
      fprintf(cfgfile, "    <predefined_mtg_path>");

      xml_fwrite_encode_entity(cfgfile, &predefined_mtg_path[i][0]);

      fprintf(cfgfile, "</predefined_mtg_path>\n");
    }

    fprintf(cfgfile, "    <spectrummarkerblock>\n");

    fprintf(cfgfile, "      <items>%i</items>\n", spectrum_colorbar->items);

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <frequency>%f</frequency>\n", spectrum_colorbar->freq[i]);
    }

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <color>%i</color>\n", spectrum_colorbar->color[i]);
    }

    for(i=0; i < MAXSPECTRUMMARKERS; i++)
    {
      fprintf(cfgfile, "      <label>");

      xml_fwrite_encode_entity(cfgfile, spectrum_colorbar->label[i]);

      fprintf(cfgfile, "</label>\n");
    }

    fprintf(cfgfile, "      <method>%i</method>\n", spectrum_colorbar->method);

    fprintf(cfgfile, "      <auto_adjust>%i</auto_adjust>\n", spectrum_colorbar->auto_adjust);

    fprintf(cfgfile, "      <max_colorbar_value>%.8f</max_colorbar_value>\n", spectrum_colorbar->max_colorbar_value);

    fprintf(cfgfile, "    </spectrummarkerblock>\n");

    fprintf(cfgfile, "    <maxdftblocksize>%i</maxdftblocksize>\n", maxdftblocksize);

    fprintf(cfgfile, "    <annotations_import_var>\n");

    fprintf(cfgfile, "      <format>%i</format>\n", import_annotations_var->format);

    fprintf(cfgfile, "      <onsettimeformat>%i</onsettimeformat>\n", import_annotations_var->onsettimeformat);

    fprintf(cfgfile, "      <onsetcolumn>%i</onsetcolumn>\n", import_annotations_var->onsetcolumn);

    fprintf(cfgfile, "      <descriptioncolumn>%i</descriptioncolumn>\n", import_annotations_var->descriptioncolumn);

    fprintf(cfgfile, "      <useduration>%i</useduration>\n", import_annotations_var->useduration);

    fprintf(cfgfile, "      <durationcolumn>%i</durationcolumn>\n", import_annotations_var->durationcolumn);

    fprintf(cfgfile, "      <datastartline>%i</datastartline>\n", import_annotations_var->datastartline);

    xml_strcpy_encode_entity(str, import_annotations_var->separator);

    fprintf(cfgfile, "      <separator>%s</separator>\n", str);

    fprintf(cfgfile, "      <dceventbittime>%i</dceventbittime>\n", import_annotations_var->dceventbittime);

    fprintf(cfgfile, "      <triggerlevel>%.12f</triggerlevel>\n", import_annotations_var->triggerlevel);

    fprintf(cfgfile, "      <manualdescription>%i</manualdescription>\n", import_annotations_var->manualdescription);

    xml_strcpy_encode_entity(str, import_annotations_var->description);

    fprintf(cfgfile, "      <description>%s</description>\n", str);

    fprintf(cfgfile, "      <ignoreconsecutive>%i</ignoreconsecutive>\n", import_annotations_var->ignoreconsecutive);

    fprintf(cfgfile, "    </annotations_import_var>\n");

    fprintf(cfgfile, "    <annotations_export_var>\n");

    fprintf(cfgfile, "      <separator>%i</separator>\n", export_annotations_var->separator);

    fprintf(cfgfile, "      <format>%i</format>\n", export_annotations_var->format);

    fprintf(cfgfile, "      <duration>%i</duration>\n", export_annotations_var->duration);

    fprintf(cfgfile, "    </annotations_export_var>\n");

    fprintf(cfgfile, "    <live_stream_update_interval>%i</live_stream_update_interval>\n", live_stream_update_interval);

    fprintf(cfgfile, "    <powerlinefreq>%i</powerlinefreq>\n", powerlinefreq);

    fprintf(cfgfile, "    <mousewheelsens>%i</mousewheelsens>\n", mousewheelsens);

    fprintf(cfgfile, "    <average_period>%i</average_period>\n", average_period);

    fprintf(cfgfile, "    <average_ratio>%i</average_ratio>\n", average_ratio);

    fprintf(cfgfile, "    <average_upsidedown>%i</average_upsidedown>\n", average_upsidedown);

    fprintf(cfgfile, "    <average_bw>%i</average_bw>\n", average_bw);

    fprintf(cfgfile, "    <spectrum_bw>%i</spectrum_bw>\n", spectrum_bw);

    fprintf(cfgfile, "    <spectrum_sqrt>%i</spectrum_sqrt>\n", spectrum_sqrt);

    fprintf(cfgfile, "    <spectrum_vlog>%i</spectrum_vlog>\n", spectrum_vlog);

    fprintf(cfgfile, "    <spectrumdock_sqrt>%i</spectrumdock_sqrt>\n", spectrumdock_sqrt);

    fprintf(cfgfile, "    <spectrumdock_colorbars>%i</spectrumdock_colorbars>\n", spectrumdock_colorbars);

    fprintf(cfgfile, "    <spectrumdock_vlog>%i</spectrumdock_vlog>\n", spectrumdock_vlog);

    fprintf(cfgfile, "    <z_score_var.crossoverfreq>%f</z_score_var.crossoverfreq>\n", z_score_var.crossoverfreq);

    fprintf(cfgfile, "    <z_score_var.z_threshold>%f</z_score_var.z_threshold>\n", z_score_var.z_threshold);

    fprintf(cfgfile, "    <z_score_var.zscore_page_len>%i</z_score_var.zscore_page_len>\n", z_score_var.zscore_page_len);

    fprintf(cfgfile, "    <z_score_var.zscore_error_detection>%i</z_score_var.zscore_error_detection>\n", z_score_var.zscore_error_detection);

    fprintf(cfgfile, "    <z_score_var.z_hysteresis>%f</z_score_var.z_hysteresis>\n", z_score_var.z_hysteresis);

    fprintf(cfgfile, "    <raw2edf_var>\n");

    fprintf(cfgfile, "      <sf>%i</sf>\n", raw2edf_var.sf);

    fprintf(cfgfile, "      <chns>%i</chns>\n", raw2edf_var.chns);

    fprintf(cfgfile, "      <phys_max>%i</phys_max>\n", raw2edf_var.phys_max);

    fprintf(cfgfile, "      <straightbinary>%i</straightbinary>\n", raw2edf_var.straightbinary);

    fprintf(cfgfile, "      <endianness>%i</endianness>\n", raw2edf_var.endianness);

    fprintf(cfgfile, "      <samplesize>%i</samplesize>\n", raw2edf_var.samplesize);

    fprintf(cfgfile, "      <offset>%i</offset>\n", raw2edf_var.offset);

    fprintf(cfgfile, "      <skipblocksize>%i</skipblocksize>\n", raw2edf_var.skipblocksize);

    fprintf(cfgfile, "      <skipbytes>%i</skipbytes>\n", raw2edf_var.skipbytes);

    xml_strcpy_encode_entity(str, raw2edf_var.phys_dim);

    fprintf(cfgfile, "      <phys_dim>%s</phys_dim>\n", str);

    fprintf(cfgfile, "    </raw2edf_var>\n");

    fprintf(cfgfile, "    <annotfilter_var>\n");

    fprintf(cfgfile, "      <tmin>%i</tmin>\n", annot_filter->tmin);

    fprintf(cfgfile, "      <tmax>%i</tmax>\n", annot_filter->tmax);

    fprintf(cfgfile, "      <invert>%i</invert>\n", annot_filter->invert);

    fprintf(cfgfile, "      <hide_other>%i</hide_other>\n", annot_filter->hide_other);

    fprintf(cfgfile, "      <hide_in_list_only>%i</hide_in_list_only>\n", annot_filter->hide_in_list_only);

    fprintf(cfgfile, "    </annotfilter_var>\n");

    fprintf(cfgfile, "    <check_for_updates>%i</check_for_updates>\n", check_for_updates);

    fprintf(cfgfile, "    <viewtime_indicator_type>%i</viewtime_indicator_type>\n", viewtime_indicator_type);

    fprintf(cfgfile, "    <mainwindow_title_type>%i</mainwindow_title_type>\n", mainwindow_title_type);

    fprintf(cfgfile, "    <default_amplitude>%f</default_amplitude>\n", default_amplitude);

    fprintf(cfgfile, "    <linear_interpolation>%i</linear_interpolation>\n", linear_interpol);

    fprintf(cfgfile, "  </UI>\n</config>\n");

    fclose(cfgfile);
  }
}


















