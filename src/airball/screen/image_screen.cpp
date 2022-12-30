#include "image_screen.h"

#include <csignal>
#include <stdio.h>

namespace airball {

ImageScreen::ImageScreen(int w, int h)
    : image_index_(0) {
  set_cs(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h));
  set_cr(cairo_create(cs()));
}

void write_to_png(cairo_surface_t* cs, std::string file_name) {
  cairo_surface_write_to_png(cs, file_name.c_str());
  cairo_surface_flush(cs);
}

void ImageScreen::flush() {
  char buf[128];
  snprintf(buf, sizeof(buf), "image%08ld.png", image_index_);
  write_to_png(cs(), buf);
  sync();
  image_index_++;
}

}  // namespace airball