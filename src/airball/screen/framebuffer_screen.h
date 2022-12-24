#ifndef AIRBALL_SCREEN_FRAMEBUFFER_SCREEN_H
#define AIRBALL_SCREEN_FRAMEBUFFER_SCREEN_H

#include "AbstractScreen.h"

namespace airball {

class FramebufferScreen : public AbstractScreen {
public:
  FramebufferScreen();

  ~FramebufferScreen() override;

  virtual void flush() override {}

private:
  void setUpFb();
  void tearDownFb();

  int fbfd_;
  unsigned char *fbp_;
  int xres_;
  int yres_;
  long int screensize_;
  cairo_surface_t *cs_;
  cairo_t *cr_;
};

}  // namespace airball

#endif // AIRBALL_SCREEN_FRAMEBUFFER_SCREEN_H