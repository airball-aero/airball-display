#include "st7789vi_screen.h"

#include <iostream>
#include <stdint.h>
#include <unistd.h>

namespace airball {

constexpr uint32_t kWidth = 240;
constexpr uint32_t kHeight = 320;
  
ST7789VIScreen::ST7789VIScreen() {
  w_.initialize();
  w_.set_brightness(255);
  data_ = (unsigned char *) malloc(sizeof(uint16_t) * kWidth * kHeight);
  set_cs(cairo_image_surface_create_for_data(
      data_,
      CAIRO_FORMAT_RGB16_565,
      kWidth,
      kHeight,
      cairo_format_stride_for_width(CAIRO_FORMAT_RGB16_565, kWidth)));
  set_cr(cairo_create(cs()));
}

ST7789VIScreen::~ST7789VIScreen() {
  cairo_destroy(cr());
  cairo_surface_destroy(cs());
  free(data_);
}

void ST7789VIScreen::setBrightness(double value) {
  value = std::max(value, 0.0);
  value = std::min(value, 1.0);
  w_.set_brightness(value * 255);
}

void ST7789VIScreen::flush() {
  w_.write_frame((uint16_t *) data_, kWidth * kHeight);
}
  
}  // namespace airball
