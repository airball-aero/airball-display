#include "x11_screen.h"

#include <cairo/cairo-xlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <math.h>

namespace airball {

X11Screen::X11Screen(const int xx, const int yy) {
  Display* display = XOpenDisplay(nullptr);
  Window w = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                 800, 600, 0, 0, 0);
  XSelectInput(display, w, StructureNotifyMask);
  XMapWindow(display, w);

  // Wait for the MapNotify event

  for(;;) {
    XEvent e;
    XNextEvent(display, &e);
    if (e.type == MapNotify) {
      break;
    }
  }

  set_cs(cairo_xlib_surface_create(
      display, w, DefaultVisual(display, DefaultScreen(display)), xx, yy));
  cairo_xlib_surface_set_size(cs(), xx, yy);
  set_cr(cairo_create(cs()));
}

void X11Screen::flush() {
  Display *display = cairo_xlib_surface_get_display(cs());
  while (XPending(display)) {
    XEvent ev;
    XNextEvent(display, &ev);
  }
  XFlush(display);
}

X11Screen::~X11Screen() {
  cairo_destroy(cr());
  Display *dsp = cairo_xlib_surface_get_display(cs());
  cairo_surface_destroy(cs());
  XCloseDisplay(dsp);
}

}  // namespace airball
