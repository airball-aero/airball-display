#include "widgets.h"

#include <math.h>
#include <cairo/cairo.h>
#include <iostream>

namespace airball {

std::ostream&
operator<<(std::ostream& os, const Color& c) {
  os << "Color(" << c.r() << "," << c.g() << "," << c.b() << "," << c.a() << ")";
  return os;
}

std::ostream&
operator<<(std::ostream& os, const Point& p) {
  os << "Point(" << p.x() << "," << p.y() << ")";
  return os;
}

std::ostream&
operator<<(std::ostream& os, const Size& s) {
  os << "Size(" << s.w() << "," << s.h() << ")";
  return os;
}

std::ostream&
operator<<(std::ostream& os, const Stroke& s) {
  os << "Stroke(" << s.color() << "," << s.width() << ")";
  return os;
}

std::ostream&
operator<<(std::ostream& os, const Font& f) {
  os << "Font(" << f.face() << "," << f.size() << ")";
  return os;
}

void Color::apply(cairo_t *cr) const {
  cairo_set_source_rgba(cr, r_, g_, b_, a_);
}

void Stroke::apply(cairo_t *cr) const {
  cairo_set_line_width(cr, width_);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  color_.apply(cr);
}

void Font::apply(cairo_t *cr) const {
  cairo_select_font_face(
      cr,
      face().c_str(),
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, size_);
}

void line(
    cairo_t* cr,
    const Point& start,
    const Point& end,
    const Stroke& stroke) {
  // std::cout << "line(" << start << "," << end << "," << stroke << ")" << std::endl;
  cairo_move_to(cr, start.x(), start.y());
  cairo_line_to(cr, end.x(), end.y());
  stroke.apply(cr);
  cairo_stroke(cr);
}

void arc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const double start_angle,
    const double end_angle,
    const Stroke& stroke) {
  // std::cout << "arc(" << center << "," << radius << "," << start_angle << "," << end_angle << ")" << std::endl;
  cairo_new_path(cr);
  cairo_arc(cr,
            center.x(),
            center.y(),
            radius,
            start_angle,
            end_angle);
  stroke.apply(cr);
  cairo_stroke(cr);
}

void disc(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const Color& fill) {
  // std::cout << "disc(" << center << "," << radius << "," << fill << ")" << std::endl;
  cairo_arc(cr,
            center.x(),
            center.y(),
            radius,
            0,
            2 * M_PI);
  fill.apply(cr);
  cairo_fill(cr);
}

void rosette(
    cairo_t* cr,
    const Point& center,
    const double radius,
    const int num_petals,
    const double petal_half_angle,
    const double start_angle,
    const Stroke& stroke) {
  double angle_increment = 2 * M_PI / ((double) num_petals);
  for (int i = 0; i < num_petals; i++) {
    double angle = start_angle + i * angle_increment;
    arc(cr,
        center,
        radius,
        angle - petal_half_angle,
        angle + petal_half_angle,
        stroke);
  }
}

void rectangle(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Color& fill) {
  // std::cout << "rectangle(" << top_left << "," << size << "," << fill << ")" << std::endl;
  cairo_rectangle(
      cr,
      top_left.x(),
      top_left.y(),
      size.w(),
      size.h());
  fill.apply(cr);
  cairo_fill(cr);
}

void round_rectangle(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const double corner_radius,
    const Color& fill) {
  rectangle(
      cr,
      Point(
          top_left.x() + corner_radius,
          top_left.y()),
      Size(
          size.w() - 2 * corner_radius,
          size.h()),
      fill);
  rectangle(
      cr,
      Point(
          top_left.x(),
          top_left.y() + corner_radius),
      Size(
          size.w(),
          size.h() - 2 * corner_radius),
      fill);
  disc(
      cr,
      Point(
          top_left.x() + corner_radius,
          top_left.y() + corner_radius),
      corner_radius,
      fill);
  disc(
      cr,
      Point(
          top_left.x() + size.w() - corner_radius,
          top_left.y() + corner_radius),
      corner_radius,
      fill);
  disc(
      cr,
      Point(
          top_left.x() + corner_radius,
          top_left.y() + size.h() - corner_radius),
      corner_radius,
      fill);
  disc(
      cr,
      Point(
          top_left.x() + size.w() - corner_radius,
          top_left.y() + size.h() - corner_radius),
      corner_radius,
      fill);
}

void box(
    cairo_t* cr,
    const Point& top_left,
    const Size& size,
    const Stroke& stroke) {
  // std::cout << "box(" << top_left << "," << size << "," << stroke << ")" << std::endl;
  cairo_rectangle(
      cr,
      top_left.x(),
      top_left.y(),
      size.w(),
      size.h());
  stroke.apply(cr);
  cairo_stroke(cr);
}

void polygon(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Stroke& stroke) {
  cairo_move_to(
      cr,
      corners[0].x(),
      corners[0].y());
  for (int i = 1; i < numCorners; i++) {
    cairo_line_to(
        cr,
        corners[i].x(),
        corners[i].y());
  }
  cairo_close_path(cr);
  stroke.apply(cr);
  cairo_stroke(cr);
}

void shape(
    cairo_t* cr,
    const int numCorners,
    const Point corners[],
    const Color& fill) {
  cairo_move_to(
      cr,
      corners[0].x(),
      corners[0].y());
  for (int i = 1; i < numCorners; i++) {
    cairo_line_to(
        cr,
        corners[i].x(),
        corners[i].y());
  }
  cairo_close_path(cr);
  fill.apply(cr);
  cairo_fill(cr);
}

Size text_size(
    cairo_t* cr,
    const std::string& str,
    const Font& font) {
  font.apply(cr);
  cairo_text_extents_t extents;
  cairo_text_extents (cr, str.c_str(), &extents);
  return {extents.width, extents.height};
}

static constexpr double kUppercaseVerticalOffsetRatio = 0.375;

void draw_text(
    cairo_t* cr,
    const std::string& str,
    const Point& point,
    const TextReferencePoint ref,
    const Font& font,
    const Color& color) {
//   std::cout << "text(\"" << str << "\"," << point << "," << ref << "," << font << ")" << std::endl;
  font.apply(cr);
  color.apply(cr);
  switch (ref) {
    case TOP_LEFT:
      cairo_move_to(
          cr,
          point.x(),
          point.y() + font.size());
      break;
    case CENTER_LEFT_UPPERCASE:
      cairo_move_to(
          cr,
          point.x(),
          point.y() + kUppercaseVerticalOffsetRatio * font.size());
      break;
    case TOP_RIGHT:
    case CENTER_RIGHT_UPPERCASE:
    case CENTER_MID_UPPERCASE:
      Size size = text_size(cr, str, font);
      switch (ref) {
        case TOP_RIGHT:
          cairo_move_to(
              cr,
              point.x() - size.w(),
              point.y() + font.size());
          break;
        case CENTER_RIGHT_UPPERCASE:
          cairo_move_to(
              cr,
              point.x() - size.w(),
              point.y() + kUppercaseVerticalOffsetRatio * font.size());
          break;
        case CENTER_MID_UPPERCASE:
          cairo_move_to(
              cr,
              point.x() - size.w() / 2,
              point.y() + kUppercaseVerticalOffsetRatio * font.size());
          break;
      }
      break;
  }
  cairo_show_text(cr, str.c_str());
}

} // namespace airball