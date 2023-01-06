#ifndef AIRBALL_DISPLAY_STALLFENCE_SCHEME_H
#define AIRBALL_DISPLAY_STALLFENCE_SCHEME_H

#include "sound_scheme.h"
#include "../sound_mixer/sine_layer.h"
#include "../sound_mixer/pwm_layer.h"
#include "../sound_mixer/balance_layer.h"

namespace airball {

class stallfence_scheme: public sound_scheme {
public:
  stallfence_scheme();
  virtual ~stallfence_scheme() = default;

  void install(ISoundMixer* mixer) override;
  void remove(ISoundMixer* mixer) override;
  void update(const IAirballModel& model, ISoundMixer* mixer) override;

private:
  sine_layer tone_;
  pwm_layer pwm_;
  balance_layer balance_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_STALLFENCE_SCHEME_H
