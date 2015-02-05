/***************************************************************************
*   Copyright (C) 2005,2006,2007 by Siraj Razick                          *
*   siraj@kdemail.net                                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
***************************************************************************/

#ifndef CLOCK
#define CLOCK

#include <desktopwidget.h>

#include <QTimer>

class ClockWidget : public UI::UIWidget
{
  Q_OBJECT

public:
  ClockWidget(QGraphicsObject *parent = 0);

  virtual ~ClockWidget();

  void paintView(QPainter *painter, const QRectF &r);

  void paintMinimizedView(QPainter *painter, const QRectF &rect);

  void updateTime(const QVariantMap &data);

private:
  void preRenderClockImages();

private:
  double mSecondValue;
  double mMinutesValue;
  double mHourValue;
  /*new svg clock */
  QPixmap mClockBackFace;
  QPixmap mClockMinuteHand;
  QPixmap mClockSecondHand;
  QPixmap mClockHourHand;
  QPixmap mClockScrew;
  QPixmap mClockGlass;
};
#endif
