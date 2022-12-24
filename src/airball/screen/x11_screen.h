#ifndef AIRBALL_SCREEN_X11_SCREEN_H
#define AIRBALL_SCREEN_X11_SCREEN_H

#include "AbstractScreen.h"

namespace airball {

class X11Screen : public AbstractScreen {
public:
  X11Screen(const int x, const int y);

  ~X11Screen() override;

  virtual void flush() override {}
};


}  // namespace

#endif // AIRBALL_SCREEN_X11_SCREEN_H
