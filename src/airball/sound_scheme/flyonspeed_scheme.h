#ifndef AIRBALL_DISPLAY_FLYONSPEED_SCHEME_H
#define AIRBALL_DISPLAY_FLYONSPEED_SCHEME_H

#include "sound_scheme.h"
#include "../sound_mixer/sine_layer.h"
#include "../sound_mixer/pwm_layer.h"
#include "../sound_mixer/balance_layer.h"

namespace airball {

class flyonspeed_scheme : public sound_scheme {
public:
  explicit flyonspeed_scheme(std::string device_name,
                             ISettings* settings,
                             IAirdata* airdata);
  ~flyonspeed_scheme() override = default;

protected:
  void update() override;

private:
  sine_layer tone_;
  pwm_layer pwm_;
  balance_layer balance_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_FLYONSPEED_SCHEME_H
