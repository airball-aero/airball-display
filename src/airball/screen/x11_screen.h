#ifndef AIRBALL_SCREEN_X11_SCREEN_H
#define AIRBALL_SCREEN_X11_SCREEN_H

#include <thread>
#include <X11/Xlib.h>
#include "AbstractScreen.h"

namespace airball {

class X11Screen : public AbstractScreen {
public:
  X11Screen(int width, int height);

  ~X11Screen() override;

  void flush() override;

  void setBrightness(double value) override {}
};


}  // namespace

#endif // AIRBALL_SCREEN_X11_SCREEN_H
