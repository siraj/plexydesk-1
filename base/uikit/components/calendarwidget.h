#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <widget.h>

namespace UIKit {
class CalendarWidget : public UIKit::Widget {
public:
  explicit CalendarWidget(QGraphicsObject *a_parent_ptr = 0);
  virtual ~CalendarWidget();

  virtual void set_date(const QDate &a_date);
  virtual QDate a_date() const;

  virtual void next();
  virtual void previous();
protected:
  virtual void clear();
  virtual void reset();

  void add_weekday_header(int i);
  virtual void paint_view(QPainter *painter, const QRectF &rect);
private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const d;
};
}

#endif // CALENDARWIDGET_H