#include "image_screen.h"

#include <cairo/cairo-xlib.h>
#include <csignal>

namespace airball {

ImageScreen::ImageScreen(int w, int h) {
  set_cs(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h));
  set_cr(cairo_create(cs()));
}


void write_to_png(cairo_surface_t* cs, std::string file_name) {
  cairo_surface_write_to_png(cs, file_name.c_str());
  cairo_surface_flush(cs);
}

void ImageScreen::flush() {
  write_to_png(cs(), "image.png");
  sync();
}

}  // namespace airball