#include "Airdata.h"

#include "aerodynamics.h"
#include "../util/units.h"

#include <iostream>

namespace airball {

constexpr int kClimbRateFilterSizeMin = 1;
constexpr int kClimbRateFilterSizeMax = 100;
constexpr double kSamplesPerSecond = 20;

Airdata::Airdata(IEventQueue* eventQueue,
                 std::unique_ptr<ITelemetry> telemetry,
                 ISettings* settings)
    : eventQueue_(eventQueue),
      telemetry_(std::move(telemetry)),
      settings_(settings),
      climb_rate_filter_(1),
      valid_(true),
      raw_balls_(kNumBalls),
      altitude_(0),
      climb_rate_(0) {
  start();
}

void Airdata::start() {
  updateThread_ = std::thread([&]() {
    while (true) {
      ITelemetry::Sample s = telemetry_->get();
      if (s.type == ITelemetry::AIRDATA) {
        eventQueue_->enqueue([this, s] () {
          update(s);
        });
      }
    }
  });
}

Airdata::~Airdata() {
  // TODO: Not correct
  updateThread_.join();
}

static double
smooth(double current_value, double new_value, double smoothing_factor) {
  return (smoothing_factor * new_value) +
         ((1.0 - smoothing_factor) * current_value);
}

static int computeClimbRateFilterSize(double vsi_smoothing_factor) {
  return (int) (kClimbRateFilterSizeMin +
         vsi_smoothing_factor *
         (kClimbRateFilterSizeMax - kClimbRateFilterSizeMin));
}

void Airdata::update(const ITelemetry::Sample d) {
  update(
      degrees_to_radians(d.airdata.alpha),
      degrees_to_radians(d.airdata.beta),
      d.airdata.q,
      d.airdata.p,
      d.airdata.t,
      settings_->baro_setting() * kPascalsPerInHg,
      settings_->ball_smoothing_factor(),
      settings_->vsi_smoothing_factor());
}

void Airdata::update(
    const double alpha,
    const double beta,
    const double q,
    const double p,
    const double t,
    const double qnh,
    const double ball_smoothing_factor,
    const double vsi_smoothing_factor) {
  double new_ias = q_to_ias(q);
  double new_tas = q_to_tas(q, p, t);

  // If we allow NaN's to get through, they will "pollute" the smoothing
  // computation and every smoothed value thereafter will be NaN. This guard
  // is therefore necessary prior to using data as a smoothing filter input.
  double new_alpha = isnan(alpha) ? smooth_ball_.alpha() : alpha;
  double new_beta = isnan(beta) ? smooth_ball_.beta() : beta;
  new_ias = isnan(new_ias) ? smooth_ball_.ias() : new_ias;
  new_tas = isnan(new_tas) ? smooth_ball_.tas() : new_tas;

  smooth_ball_ = Ball(
      smooth(smooth_ball_.alpha(), alpha, ball_smoothing_factor),
      smooth(smooth_ball_.beta(), beta, ball_smoothing_factor),
      smooth(smooth_ball_.ias(), new_ias, ball_smoothing_factor),
      smooth(smooth_ball_.tas(), new_tas, ball_smoothing_factor));

  for (size_t i = raw_balls_.size() - 1; i > 0; i--) {
    raw_balls_[i] = raw_balls_[i - 1];
  }
  raw_balls_[0] = Ball(new_alpha, new_beta, new_ias, new_tas);

  pressure_altitude_ = pressure_to_altitude(t, p, QNH_STANDARD);

  int climbRateFilterSize = computeClimbRateFilterSize(vsi_smoothing_factor);
  if (climbRateFilterSize != climb_rate_filter_.size()) {
    climb_rate_filter_ = LinearRateFilter(climbRateFilterSize);
  }

  climb_rate_filter_.put(pressure_altitude_);
  climb_rate_ = climb_rate_filter_.rate() / (1.0 / kSamplesPerSecond);

  altitude_ = pressure_to_altitude(t, p, qnh);

  valid_ = !isnan(alpha) && !isnan(beta);
}

} // namespace airball
