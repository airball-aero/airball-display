#ifndef AIRBALL_FRAMEWORK_I_SCREEN_H
#define AIRBALL_FRAMEWORK_I_SCREEN_H

#include <cairo/cairo.h>

namespace airball {

/**
 * Encapsulates a physical screen to which an Airball application can draw its
 * user interface.
 */
class IScreen {
public:
  // Return a Cairo context allowing an application to draw to the Screen.
  virtual cairo_t *cr() const = 0;

  // Return a Cairo surface corresponding to the context cr().
  virtual cairo_surface_t *cs() const = 0;

  // Indicate to this Screen that the current image is to be flushed.
  virtual void flush() = 0;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_I_SCREEN_H
