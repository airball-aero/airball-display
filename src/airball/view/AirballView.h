#ifndef AIRBALL_DISPLAY_H
#define AIRBALL_DISPLAY_H

#include <cairo/cairo.h>

#include "../model/Airdata.h"
#include "../screen/AbstractScreen.h"
#include "../model/Settings.h"

namespace airball {

class AirballView : public IView<IAirballModel> {
public:
  void paint(const IAirballModel& m, IScreen* screen) override;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_H
