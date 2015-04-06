﻿/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  : *
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "timezone.h"
#include <widget.h>
#include <plexyconfig.h>

#include <QTimer>
#include <QTimeZone>
#include <QLocale>

#include <view_controller.h>
#include <item_view.h>
#include <label.h>
#include <button.h>
#include <lineedit.h>

class TimeZoneActivity::PrivateTimeZone
{
public:
  PrivateTimeZone() {}
  ~PrivateTimeZone()
  {
    if (m_window_ptr) {
      delete m_window_ptr;
    }
    qDebug() << Q_FUNC_INFO << "Delete TimeZone Activity";
  }

  UIKit::Window *m_window_ptr;
  UIKit::Widget *m_content_widget_ptr;
  UIKit::ItemView *m_timezone_browser_ptr;
  UIKit::LineEdit *m_filter_widget_ptr;

  QVariantMap m_result_data;
};

TimeZoneActivity::TimeZoneActivity(QGraphicsObject *aParent)
  : UIKit::DesktopActivity(aParent), m_priv_ptr(new PrivateTimeZone) {}

TimeZoneActivity::~TimeZoneActivity() { delete m_priv_ptr; }

void TimeZoneActivity::create_window(const QRectF &aWindowGeometry,
                                    const QString &aWindowTitle,
                                    const QPointF &aWindowPos)
{
  m_priv_ptr->m_window_ptr = new UIKit::Window();
  m_priv_ptr->m_window_ptr->set_window_title(aWindowTitle);

  m_priv_ptr->m_content_widget_ptr =
          new UIKit::Widget(m_priv_ptr->m_window_ptr);
  m_priv_ptr->m_content_widget_ptr->setGeometry(aWindowGeometry);

  m_priv_ptr->m_filter_widget_ptr =
      new UIKit::LineEdit(m_priv_ptr->m_content_widget_ptr);
  m_priv_ptr->m_filter_widget_ptr->setMinimumSize(aWindowGeometry.width() - 16,
                                                  32);
  m_priv_ptr->m_filter_widget_ptr->setGeometry(QRectF(8, 0, 0, 0));

  m_priv_ptr->m_timezone_browser_ptr =
    new UIKit::ItemView(m_priv_ptr->m_content_widget_ptr);
  /*
  m_priv_ptr->m_timezone_browser_ptr->setGeometry(
        QRectF(0, 0, aWindowGeometry.width(),
               aWindowGeometry.height() -
               m_priv_ptr->m_filter_widget_ptr->minimumHeight()));
               */
  m_priv_ptr->m_timezone_browser_ptr->set_view_geometry(
        QRectF(0, 0, aWindowGeometry.width() - 16,
               aWindowGeometry.height() -
               m_priv_ptr->m_filter_widget_ptr->minimumHeight()));

  m_priv_ptr->m_timezone_browser_ptr->setPos(
              18,
              m_priv_ptr->m_filter_widget_ptr->minimumHeight() + 8);

  m_priv_ptr->m_filter_widget_ptr->show();

  m_priv_ptr->m_window_ptr->set_window_content(
              m_priv_ptr->m_content_widget_ptr);

  set_geometry(aWindowGeometry);

  exec(aWindowPos);

  m_priv_ptr->m_filter_widget_ptr->on_insert([&](const QString &a_txt) {
      m_priv_ptr->m_timezone_browser_ptr->set_filter(a_txt);
  });

  m_priv_ptr->m_window_ptr->on_window_discarded(
              [this](UIKit::Window * aWindow) {
    discard_activity();
  });

  loadTimeZones();
}

QVariantMap TimeZoneActivity::result() const
{
    return m_priv_ptr->m_result_data;
}

void TimeZoneActivity::update_attribute(const QString &aName,
                                       const QVariant &aVariantData) {}

UIKit::Window *TimeZoneActivity::window() const
{
    return m_priv_ptr->m_window_ptr;
}

void TimeZoneActivity::cleanup()
{
  if (m_priv_ptr->m_window_ptr) {
    delete m_priv_ptr->m_window_ptr;
  }
  m_priv_ptr->m_window_ptr = 0;
}

void TimeZoneActivity::loadTimeZones()
{
  QStringList l_timezone_list;
  std::vector<UIKit::ModelViewItem *> _item_list;

  foreach(const QByteArray id,  QTimeZone::availableTimeZoneIds()) {
      QLocale::Country l_country_locale = QTimeZone(id).country();
      QString l_time_zone_lable_str =
          QLocale::countryToString(l_country_locale);
      l_time_zone_lable_str += " " + QTimeZone(id).displayName(
            QDateTime::currentDateTime(), QTimeZone::OffsetName);

      UIKit::Label *lTimeZoneLabelPtr =
          new UIKit::Label(m_priv_ptr->m_timezone_browser_ptr);
      UIKit::ModelViewItem *l_item = new UIKit::ModelViewItem();

      l_item->set_data("label", l_time_zone_lable_str);
      l_item->set_data("id", id);
      l_item->on_activated([&](UIKit::ModelViewItem *a_item) {
        if (a_item) {
            m_priv_ptr->m_result_data["timezone"] =
                a_item->data("label").toString();
            m_priv_ptr->m_result_data["id"] =
                a_item->data("id").toByteArray();
            activate_response();
          }
      });

      l_item->set_view(lTimeZoneLabelPtr);
      l_item->on_filter([&](const UIKit::Widget *a_view, const QString &a_keyword) {
          const UIKit::Label *_lbl_widget =
                  dynamic_cast<const UIKit::Label*> (a_view);

          if (_lbl_widget) {
              if (_lbl_widget->label().toLower().contains(a_keyword.toLower())) {
                  qDebug() << Q_FUNC_INFO << a_keyword << " VS " << _lbl_widget->label().toLower();
                  return 1;
              }
          }

          return 0;
      });

      lTimeZoneLabelPtr->set_alignment(Qt::AlignLeft);

      lTimeZoneLabelPtr->setMinimumSize(
            m_priv_ptr->m_timezone_browser_ptr->geometry().width(),
            32);
      lTimeZoneLabelPtr->set_size(
            QSizeF(
              m_priv_ptr->m_timezone_browser_ptr->boundingRect().width(),
              32));
      lTimeZoneLabelPtr->set_label(l_time_zone_lable_str);
      _item_list.push_back(l_item);
    }

  std::sort(_item_list.begin(), _item_list.end(),
            [](UIKit::ModelViewItem *a_a, UIKit::ModelViewItem *a_b) {
      return a_a->data("label").toString() < a_b->data("label").toString();
    });

  std::for_each(_item_list.begin(), _item_list.end(), [this](UIKit::ModelViewItem *a) {
   if (a) {
       m_priv_ptr->m_timezone_browser_ptr->insert(a);
   }
  });  
}
