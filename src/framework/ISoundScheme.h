#ifndef AIRBALL_FRAMEWORK_I_SOUND_SCHEME_H
#define AIRBALL_FRAMEWORK_I_SOUND_SCHEME_H

#include "ISoundMixer.h"

namespace airball {

template <typename Model>
class ISoundScheme {
public:
  virtual ~ISoundScheme() = default;
  virtual void install(ISoundMixer* mixer) = 0;
  virtual void remove(ISoundMixer* mixer) = 0;
  virtual void update(const Model& m, ISoundMixer* mixer) = 0;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_I_SOUND_SCHEME_H
