#ifndef AIRBALL_DISPLAY_AIRDATA_H
#define AIRBALL_DISPLAY_AIRDATA_H

#include <string>
#include "../../framework/Application.h"
#include "IAirdata.h"
#include "telemetry/ITelemetry.h"
#include "../util/LinearRateFilter.h"
#include "ISettings.h"
#include "IAirballModel.h"

namespace airball {

struct RawData;

class Airdata : public IAirdata {
public:
  Airdata(ISettings* settings);

  ~Airdata();

  [[nodiscard]] bool valid() const override;

  [[nodiscard]] double altitude() const override { return altitude_; }
  [[nodiscard]] double climb_rate() const override { return climb_rate_; }
  [[nodiscard]] const Ball& smooth_ball() const override { return smooth_ball_; }
  [[nodiscard]] const std::vector<Ball>& raw_balls() const override { return raw_balls_; }

  void update(ITelemetry::Message message) override;

private:
  void update(
      double qnh,
      double ball_time_constant,
      double vsi_time_constant);

  static constexpr uint kNumBalls = 20;

  ISettings* settings_;

  std::unique_ptr<RawData> raw_;

  bool valid_;
  std::chrono::system_clock::time_point lastUpdateTime_;

  Ball smooth_ball_;
  std::vector<Ball> raw_balls_;

  LinearRateFilter climb_rate_filter_;
  double climb_rate_;

  double altitude_;
  double pressure_altitude_;
};

} // namespace airball

#endif // AIRBALL_DISPLAY_AIRDATA_H
