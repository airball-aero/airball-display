#ifndef AIRBALL_DISPLAY_SOUND_LAYER_H
#define AIRBALL_DISPLAY_SOUND_LAYER_H

#include <cstdint>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include "../../framework/ISoundLayer.h"

namespace airball {

class sound_layer : public ISoundLayer {
public:
  explicit sound_layer() = default;
  virtual ~sound_layer() = default;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_SOUND_LAYER_H
