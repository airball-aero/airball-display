#ifndef AIRBALL_DISPLAY_SOUND_MIXER_H
#define AIRBALL_DISPLAY_SOUND_MIXER_H

#include <vector>
#include <condition_variable>
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <memory>
#include <mutex>
#include <thread>

#include "../../framework/ISoundMixer.h"

#include "sound_layer.h"

namespace airball {

class sound_mixer : public ISoundMixer {
public:
  explicit sound_mixer(const std::string& device_name);
  ~sound_mixer() override;

  void set_layers(std::vector<ISoundLayer*> layers) override;

  snd_pcm_uframes_t actual_period_size() override;

  snd_pcm_uframes_t seconds_to_frames(double seconds) override;

  snd_pcm_uframes_t frequency_to_period(double cycles_per_second) override;

  snd_pcm_uframes_t octaves_to_period(double base_cycles_per_second,
                                      double octaves) override;

private:
  unsigned int actual_rate();

  bool start();
  void loop();

  sound_mixer(const sound_mixer&) = delete;
  sound_mixer& operator=(const sound_mixer&) = delete;

  const std::string device_name_;
  std::vector<ISoundLayer*> layers_;

  std::mutex mut_;
  std::condition_variable start_;
  bool done_;

  std::thread server_;

  snd_pcm_t* handle_;
  unsigned int actual_rate_;
  snd_pcm_uframes_t actual_period_size_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_MIXER_H
