#ifndef AIRBALL_SCREEN_ABSTRACT_SCREEN_H
#define AIRBALL_SCREEN_ABSTRACT_SCREEN_H

#include <cairo/cairo.h>

#include "../../framework/IScreen.h"

namespace airball {

/**
 * Encapsulates a physical screen to which an Airball Display can draw its
 * user interface.
 */
class AbstractScreen : public IScreen {
public:
  virtual ~AbstractScreen() = default;
  cairo_t *cr() const override { return cr_; }
  cairo_surface_t *cs() const override { return cs_; }

protected:
  void set_cr(cairo_t* cr) { cr_ = cr; }
  void set_cs(cairo_surface_t* cs) { cs_ = cs; }

private:
  cairo_t *cr_;
  cairo_surface_t *cs_;
};

} // namespace airball

#endif // AIRBALL_SCREEN_ABSTRACT_SCREEN_H
