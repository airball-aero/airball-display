#ifndef AIRBALL_APP_AIRBALL_MODEL_H
#define AIRBALL_APP_AIRBALL_MODEL_H

#include "IAirdata.h"
#include "ISettings.h"

namespace airball {

class IAirballModel {
public:
  virtual const IAirdata* airdata() const = 0;
  virtual const ISettings* settings() const = 0;
};

} // namespace airball

#endif // AIRBALL_APP_AIRBALL_MODEL_H
