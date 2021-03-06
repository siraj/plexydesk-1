#include "desktopwindow.h"

#include <QImage>
#include <QPixmapCache>
#include <QPainter>
#include <ck_image_io.h>

class desktop_window::PrivateDesktopWindow {
public:
  PrivateDesktopWindow()
      : m_background_buffer(0), m_background_width(0), m_background_height(0),
        m_seamless(0), m_desktop_mode(kNone),
        m_image_service(new cherry_kit::image_io(0, 0)) {}
  ~PrivateDesktopWindow() {
    if (m_background_buffer)
      free(m_background_buffer);

		if (m_image_service)
			delete m_image_service;

    qDebug() << Q_FUNC_INFO;
  }

  QImage m_background_texture;
  unsigned char *m_background_buffer;
  int m_background_width;
  int m_background_height;
  bool m_seamless;
  cherry_kit::image_io *m_image_service;
  DesktopScalingMode m_desktop_mode;
};

void desktop_window::reset_window_background() {
  memset(priv->m_background_buffer, 0,
         (4 * priv->m_background_height * priv->m_background_width));
}

bool desktop_window::is_seamless() { return priv->m_seamless; }

void desktop_window::set_seamless(bool mode) {
  priv->m_seamless = mode;
  update();
}

void desktop_window::set_desktop_scale_mode(
    desktop_window::DesktopScalingMode a_desktop_mode) {
  priv->m_desktop_mode = a_desktop_mode;
}

desktop_window::DesktopScalingMode desktop_window::desktop_scale_mode() const {
  return priv->m_desktop_mode;
}

desktop_window::desktop_window()
    : cherry_kit::window(0), priv(new PrivateDesktopWindow) {
  set_window_type(cherry_kit::window::kFramelessWindow);

  priv->m_image_service->on_ready([this](
      cherry_kit::image_io::buffer_load_status_t a_load_state,
      cherry_kit::image_io *a_image_service) {

    if (a_load_state == cherry_kit::image_io::kSuccess) {
      cherry_kit::io_surface *ck_image_surface_ref = a_image_service->surface();

      priv->m_background_texture = QImage(
          ck_image_surface_ref->buffer, ck_image_surface_ref->width,
          ck_image_surface_ref->height, QImage::Format_ARGB32_Premultiplied);
      update();
    } else {
      qWarning() << Q_FUNC_INFO << "Failed loading image!";
    }
  });

  on_window_resized([this](window *a_window, int a_width, int a_height) {
    if (priv->m_background_buffer) {
      /*
    //reset_window_background();
    p_window->m_background_width = a_width;
    p_window->m_background_height = a_height;
    p_window->m_background_buffer = (unsigned char *)realloc(
        p_window->m_background_buffer, (a_width, a_height * 4));
    //reset_window_background();
    */
    }
  });
}

desktop_window::~desktop_window() { delete priv; }

void desktop_window::set_background(const std::string &a_image_name) {
  /*
  QImage background(a_image_name);
  p_window->m_background_width = background.width();
  p_window->m_background_height = background.height();

  p_window->m_background_buffer = background.bits();
  */

  // if (!o_window->m_background_texture.load(a_image_name))
  // qDebug() << Q_FUNC_INFO << "Failed to load image";

  // update();

  if (!priv->m_image_service)
    return;

  priv->m_image_service->create(a_image_name);
}

void desktop_window::set_background(const QImage &a_image_name) {
  priv->m_background_texture = a_image_name;
  update();
}

void desktop_window::paint_view(QPainter *a_ctx, const QRectF &a_rect) {
  if (priv->m_seamless) {
    a_ctx->setBackgroundMode(Qt::TransparentMode);
    a_ctx->setCompositionMode(QPainter::CompositionMode_Source);
    a_ctx->fillRect(a_rect, Qt::transparent);
    widget::paint_view(a_ctx, a_rect);
    return;
  }

  a_ctx->save();
  a_ctx->setRenderHints(QPainter::SmoothPixmapTransform |
                        QPainter::HighQualityAntialiasing |
                        QPainter::Antialiasing);
  QRectF draw_rect;
  draw_rect.setX(a_rect.x());
  draw_rect.setY(a_rect.y());
  draw_rect.setWidth(a_rect.width());
  draw_rect.setHeight(a_rect.height());
  a_ctx->drawImage(draw_rect, priv->m_background_texture);
  a_ctx->restore();
}
