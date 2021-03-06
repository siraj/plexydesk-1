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

#include "desktopmanager.h"

#include <QDebug>

#include <QPushButton>
#include <ck_space.h>
#include <ck_workspace.h>
#include <ck_text_editor.h>

class DesktopManager::PrivateDesktopManager {
public:
  PrivateDesktopManager() : m_scene(0) {}
  ~PrivateDesktopManager() {
     if (m_scene)
         delete m_scene;
  }

  QGraphicsScene *m_scene;
};

DesktopManager::DesktopManager(QWidget *parent)
    : cherry_kit::workspace(0, parent),
      priv(new PrivateDesktopManager) {

   priv->m_scene = new QGraphicsScene(this);
   setScene(priv->m_scene);
   setWindowFlags(Qt::Window);
}

DesktopManager::~DesktopManager() { delete priv; }

void DesktopManager::add_sample_text()
{
  cherry_kit::window *window = new cherry_kit::window();

  scene()->addItem(window);
  window->set_window_title("Hello world");
  cherry_kit::text_editor *edit = new cherry_kit::text_editor();
  cherry_kit::widget *widget = new cherry_kit::widget();
  widget->setGeometry(QRectF(0, 0, 200, 200));
  edit->setGeometry(QRectF(0, 0, 200, 200));
  window->set_window_content(edit);
  window->show();
}

void DesktopManager::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    if (current_active_space()) {
      desktop_controller_ref dock_controller =
          current_active_space()->controller("dockwidget");
      if (dock_controller) {
        QVariantMap menu_argument;
        menu_argument["menu_pos"] = mapToScene(event->pos());

        dock_controller->task().execute();
      }
    }
  }

  cherry_kit::workspace::mouseReleaseEvent(event);
}
