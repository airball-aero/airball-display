#ifndef AIRBALL_DISPLAY_AIRDATA_H
#define AIRBALL_DISPLAY_AIRDATA_H

#include <string>
#include "../../framework/Application.h"
#include "../model/IAirdata.h"
#include "telemetry/telemetry.h"
#include "../util/cubic_rate_filter.h"
#include "ISettings.h"
#include "IAirballModel.h"
#include "telemetry/ILineReader.h"

namespace airball {

class Airdata : public IAirdata {
public:
  Airdata(IEventQueue* eventQueue,
          std::unique_ptr<ILineReader> lineReader,
          ISettings* settings);

  ~Airdata();

  double altitude() const override { return altitude_; }
  double climb_rate() const override { return climb_rate_; }
  bool valid() const override { return valid_; }
  const Ball& smooth_ball() const override { return smooth_ball_; }
  const std::vector<Ball>& raw_balls() const override { return raw_balls_; }

private:
  void start();
  void update(Telemetry::Sample sample);

  void update(
      double alpha,
      double beta,
      double q,
      double p,
      double t,
      double qnh,
      double ball_smoothing_factor,
      double vsi_smoothing_factor);

  static constexpr int kSamplesPerSecond = 20;
  static constexpr uint kNumBalls = 20;

  IEventQueue* eventQueue_;
  std::unique_ptr<ILineReader> lineReader_;
  ISettings* settings_;

  bool valid_;

  Ball smooth_ball_;
  std::vector<Ball> raw_balls_;

  cubic_rate_filter climb_rate_filter_;
  double climb_rate_;

  double altitude_;
  double pressure_altitude_;

  std::thread updateThread_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_AIRDATA_H
