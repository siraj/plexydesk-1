/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
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

#include "ck_session_sync.h"

#include <ck_data_sync.h>
#include <ck_disk_engine.h>

#include <QDebug>

namespace cherry_kit {
class session_sync::PrivSessionSync {
public:
  PrivSessionSync() : m_purged(0) {}
  ~PrivSessionSync() {}

  int m_session_id;
  QVariantMap m_session_data;
  std::string m_session_group_name;
  bool m_purged;

  std::function<void()> m_on_session_init_func;
  std::function<void()> m_on_session_update_func;
  std::function<void()> m_on_session_end_func;
};

session_sync::session_sync(const std::string &a_session_name,
                           const QVariantMap &a_data)
    : d(new PrivSessionSync) {

  d->m_session_id = -1;
  d->m_session_data = a_data;
  d->m_session_group_name = a_session_name;
}

void session_sync::session_init() {
  if (d->m_on_session_init_func)
    d->m_on_session_init_func();
}

void session_sync::set_session_data(const QString &a_key,
                                    const QVariant &a_data) {
  d->m_session_data[a_key] = a_data;
}

QVariant session_sync::session_data(const QString &a_key) const {
  if (!d->m_session_data.contains(a_key)) {
    return QVariant("");
  }
  return d->m_session_data[a_key];
}

QList<QString> session_sync::session_keys() const {
  return d->m_session_data.keys();
}

void session_sync::purge() { d->m_purged = true; }

bool session_sync::is_purged() { return d->m_purged; }

void session_sync::set_session_id(int a_id) { d->m_session_id = a_id; }

int session_sync::session_id() { return d->m_session_id; }

std::string session_sync::session_id_to_string() const {
  return QString("%1").arg(d->m_session_id).toStdString();
}

void session_sync::update_session() {
  if (d->m_on_session_update_func)
    d->m_on_session_update_func();
}

std::string session_sync::session_group_key() const {
  std::string key_name = d->m_session_group_name;
  std::transform(key_name.begin(), key_name.end(), key_name.begin(), ::tolower);
  return key_name + "_id";
}

void session_sync::bind_to_window(cherry_kit::window *a_window) {
  if (!a_window)
    return;

  /*
  a_window->on_window_closed([this](const cherry_kit::Window *a_window) {
      unbind_window(a_window);
  });
  */

  a_window->on_window_moved([this](const QPointF &a_pos) {
    std::string db_name(session_data("database_name").toByteArray().data());

    if (db_name.empty()) {
      qWarning() << Q_FUNC_INFO << "Null session database name";
      return;
    }

    save_session_attribute(db_name, d->m_session_group_name,
                           session_group_key(), session_id_to_string(), "x",
                           std::to_string(a_pos.x()));

    save_session_attribute(db_name, d->m_session_group_name,
                           session_group_key(), session_id_to_string(), "y",
                           std::to_string(a_pos.y()));
  });
}

void session_sync::unbind_window(const window *a_windows) {
  std::string db_name(session_data("database_name").toByteArray().data());

  purge();

  if (db_name.empty()) {
    qWarning() << Q_FUNC_INFO << "Null session database name";
    return;
  }

  delete_session_data(db_name, d->m_session_group_name, session_group_key(),
                      session_id_to_string());
}

void session_sync::delete_session_data(const std::string &a_session_name,
                                       const std::string &a_object_name,
                                       const std::string &a_object_key,
                                       const std::string &a_object_value) {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync(a_session_name);

  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
  sync->set_sync_engine(engine);

  sync->remove_object(a_object_name, a_object_key, a_object_value);
  qDebug() << Q_FUNC_INFO << "Removed Object";
  delete sync;
}

void session_sync::save_session_attribute(const std::string &a_session_name,
                                          const std::string &a_object_name,
                                          const std::string &a_object_key,
                                          const std::string &a_object_value,
                                          const std::string &a_key,
                                          const std::string &a_value) {
  cherry_kit::data_sync *sync = new cherry_kit::data_sync(a_session_name);
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();
  sync->set_sync_engine(engine);

  sync->on_object_found([=](cherry_kit::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {
    if (a_found) {
      a_object.set_property(a_key, a_value);
      sync->save_object(a_object);
      const QVariant value(a_value.c_str());
      set_session_data(QString::fromStdString(a_key), value);

    } else {
      qWarning() << Q_FUNC_INFO << "Object Not Found"
                 << "Key :" << QString::fromStdString(a_object_key)
                 << " Value :" << QString::fromStdString(a_object_value)
                 << " In Session :" << QString::fromStdString(a_session_name)
                 << " Of Object :" << QString::fromStdString(a_object_name);
    }
  });

  sync->find(a_object_name, a_object_key, a_object_value);

  delete sync;
}

void session_sync::on_session_init(std::function<void()> a_handler) {
  d->m_on_session_init_func = a_handler;
}

void session_sync::on_session_update(std::function<void()> a_handler) {
  d->m_on_session_update_func = a_handler;
}

void session_sync::on_session_end(std::function<void()> a_handler) {
  d->m_on_session_end_func = a_handler;
}
}
