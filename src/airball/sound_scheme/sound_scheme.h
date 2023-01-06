#ifndef AIRBALL_DISPLAY_SOUND_SCHEME_H
#define AIRBALL_DISPLAY_SOUND_SCHEME_H

#include "../../framework/ISoundScheme.h"
#include "../../framework/ISoundMixer.h"
#include "../model/IAirballModel.h"

namespace airball {

class sound_scheme : public ISoundScheme<IAirballModel> {
public:
  void install(ISoundMixer* mixer) override;
  void remove(ISoundMixer* mixer) override;
  void update(const IAirballModel& model, ISoundMixer* mixer) override;
private:
  std::unique_ptr<ISoundScheme<IAirballModel>> soundScheme_;
};

} // namespace airball

#endif //AIRBALL_DISPLAY_SOUND_SCHEME_H
