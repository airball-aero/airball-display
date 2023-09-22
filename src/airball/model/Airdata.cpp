#include "Airdata.h"

#include "aerodynamics.h"
#include "../util/units.h"

#include <iostream>

namespace airball {

constexpr double kSamplesPerSecond = 20;

constexpr static std::chrono::milliseconds
    kAirdataExpiryPeriod(250);

Airdata::Airdata(ISettings* settings)
    : settings_(settings),
      climb_rate_filter_(1),
      valid_(true),
      raw_balls_(kNumBalls),
      altitude_(0),
      climb_rate_(0) { }

static double
smooth(double current_value, double new_value, double time_constant) {
  double sample_time = 1.0 / kSamplesPerSecond;
  double factor = exp(-1.0 * sample_time / time_constant);
  return ((1.0 - factor) * new_value) + (factor * current_value);
}

void Airdata::update(const ITelemetry::Airdata d) {
  update(
      degrees_to_radians(d.alpha),
      degrees_to_radians(d.beta),
      d.q,
      d.p,
      d.t,
      settings_->baro_setting() * kPascalsPerInHg,
      settings_->ball_time_constant(),
      settings_->vsi_time_constant());
}

void Airdata::update(
    const double alpha,
    const double beta,
    const double q,
    const double p,
    const double t,
    const double qnh,
    const double ball_time_constant,
    const double vsi_time_constant) {
  double q_corr = q * settings_->q_correction_factor();

  double new_ias = q_to_ias(q_corr);
  double new_tas = q_to_tas(q_corr, p, t);

  // If we allow NaN's to get through, they will "pollute" the smoothing
  // computation and every smoothed value thereafter will be NaN. This guard
  // is therefore necessary prior to using data as a smoothing filter input.
  double new_alpha = isnan(alpha) ? smooth_ball_.alpha() : alpha;
  double new_beta = isnan(beta) ? smooth_ball_.beta() : beta;
  new_ias = isnan(new_ias) ? smooth_ball_.ias() : new_ias;
  new_tas = isnan(new_tas) ? smooth_ball_.tas() : new_tas;

  smooth_ball_ = Ball(
      smooth(smooth_ball_.alpha(), alpha, ball_time_constant),
      smooth(smooth_ball_.beta(), beta, ball_time_constant),
      smooth(smooth_ball_.ias(), new_ias, ball_time_constant),
      smooth(smooth_ball_.tas(), new_tas, ball_time_constant));

  for (size_t i = raw_balls_.size() - 1; i > 0; i--) {
    raw_balls_[i] = raw_balls_[i - 1];
  }
  raw_balls_[0] = Ball(new_alpha, new_beta, new_ias, new_tas);

  pressure_altitude_ = pressure_to_altitude(t, p, QNH_STANDARD);

  int climbRateFilterSize = vsi_time_constant * kSamplesPerSecond;
  if (climbRateFilterSize != climb_rate_filter_.size()) {
    climb_rate_filter_ = LinearRateFilter(climbRateFilterSize);
  }

  climb_rate_filter_.put(pressure_altitude_);
  climb_rate_ = climb_rate_filter_.rate() / (1.0 / kSamplesPerSecond);

  altitude_ = pressure_to_altitude(t, p, qnh);

  valid_ = !isnan(alpha) && !isnan(beta);
  lastUpdateTime_ = std::chrono::system_clock::now();
}

bool Airdata::valid() const {
  return
      valid_ &&
      (std::chrono::system_clock::now() - lastUpdateTime_) < kAirdataExpiryPeriod;
}

} // namespace airball
