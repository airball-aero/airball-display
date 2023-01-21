#ifndef AIRBALL_FRAMEWORK_I_SOUND_MIXER_H
#define AIRBALL_FRAMEWORK_I_SOUND_MIXER_H

#include <vector>
#include <condition_variable>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <memory>
#include <mutex>
#include <thread>

#include "ISoundLayer.h"

namespace airball {

class ISoundMixer {
public:
  virtual ~ISoundMixer() = default;

  virtual void set_layers(std::vector<ISoundLayer*> layers) = 0;

  virtual snd_pcm_uframes_t actual_period_size() = 0;

  virtual snd_pcm_uframes_t seconds_to_frames(double seconds) = 0;

  virtual snd_pcm_uframes_t frequency_to_period(double cycles_per_second) = 0;

  virtual snd_pcm_uframes_t octaves_to_period(double base_cycles_per_second,
                                              double octaves) = 0;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_I_SOUND_MIXER_H
