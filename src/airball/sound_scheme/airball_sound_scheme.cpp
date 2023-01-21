#include "airball_sound_scheme.h"
#include "stallfence_scheme.h"
#include "flyonspeed_scheme.h"

namespace airball {

const std::string kFlyonspeed = "flyonspeed";
const std::string kStallfence = "stallfence";

void airball_sound_scheme::install(ISoundMixer *mixer) {
  // We don't know what to install yet since we can't see the ISettings
}

void airball_sound_scheme::remove(ISoundMixer *mixer) {
  if (soundScheme_ != nullptr) {
    soundScheme_->remove(mixer);
    soundScheme_ = nullptr;
  }
}

void airball_sound_scheme::update(const IAirballModel &model, ISoundMixer *mixer) {
  bool is_installed = soundScheme_ != nullptr;
  bool is_stallfence = is_installed && dynamic_cast<stallfence_scheme*>(soundScheme_.get()) != nullptr;
  bool is_flyonspeed = is_installed && dynamic_cast<flyonspeed_scheme*>(soundScheme_.get()) != nullptr;

  if (model.settings()->sound_scheme() == kFlyonspeed && !is_flyonspeed) {
    remove(mixer);
    soundScheme_ = std::make_unique<flyonspeed_scheme>();
    soundScheme_->install(mixer);
  } else if (model.settings()->sound_scheme() == kStallfence && !is_stallfence) {
    remove(mixer);
    soundScheme_ = std::make_unique<stallfence_scheme>();
    soundScheme_->install(mixer);
  }

  if (soundScheme_ != nullptr) {
    soundScheme_->update(model, mixer);
  }
}

} // namespace airball