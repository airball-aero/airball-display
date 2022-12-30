#ifndef AIRBALL_DISPLAY_AIRDATA_H
#define AIRBALL_DISPLAY_AIRDATA_H

#include <string>
#include "../../framework/Application.h"
#include "../model/IAirdata.h"
#include "telemetry/ITelemetry.h"
#include "../util/LinearRateFilter.h"
#include "ISettings.h"
#include "IAirballModel.h"

namespace airball {

class Airdata : public IAirdata {
public:
  Airdata(IEventQueue* eventQueue,
          std::unique_ptr<ITelemetry> telemetry,
          ISettings* settings);

  ~Airdata();

  [[nodiscard]] double altitude() const override { return altitude_; }
  [[nodiscard]] double climb_rate() const override { return climb_rate_; }
  [[nodiscard]] bool valid() const override { return valid_; }
  [[nodiscard]] const Ball& smooth_ball() const override { return smooth_ball_; }
  [[nodiscard]] const std::vector<Ball>& raw_balls() const override { return raw_balls_; }

private:
  void start();
  void update(ITelemetry::Sample sample);

  void update(
      double alpha,
      double beta,
      double q,
      double p,
      double t,
      double qnh,
      double ball_smoothing_factor,
      double vsi_smoothing_factor);

  static constexpr int kSlineReaderamplesPerSecond = 20;
  static constexpr uint kNumBalls = 20;

  IEventQueue* eventQueue_;
  std::unique_ptr<ITelemetry> telemetry_;
  ISettings* settings_;

  bool valid_;

  Ball smooth_ball_;
  std::vector<Ball> raw_balls_;

  LinearRateFilter climb_rate_filter_;
  double climb_rate_;

  double altitude_;
  double pressure_altitude_;

  std::thread updateThread_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_AIRDATA_H
