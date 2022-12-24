#ifndef AIRBALL_FRAMEWORK_I_SOUND_LAYER_H
#define AIRBALL_FRAMEWORK_I_SOUND_LAYER_H

#include <cstdint>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

namespace airball {

class ISoundLayer {
public:
  // Apply this layer to a PCM buffer of a given number of frames. The buffer
  // contains interleaved left and right values, and so is (2 * frames) long.
  virtual void apply(int16_t* buf, snd_pcm_uframes_t frames) = 0;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_I_SOUND_LAYER_H
