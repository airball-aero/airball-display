#ifndef AIRBALL_SCREEN_IMAGE_SCREEN_H
#define AIRBALL_SCREEN_IMAGE_SCREEN_H

#include <string>
#include "AbstractScreen.h"

namespace airball {

/**
 * Encapsulates a Screen which paints to an in-memory buffered image.
 */
class ImageScreen : public AbstractScreen {
public:

  /**
   * Creates a new ImageScreen.
   *
   * @param w the width of the Screen.
   * @param h the height of the Screen.
   */
  ImageScreen(int w, int h);

  void flush() override;

  void setBrightness(double value) override {}

private:
  unsigned long image_index_;
};

}  // namespace airball

#endif // AIRBALL_SCREEN_IMAGE_SCREEN_H
