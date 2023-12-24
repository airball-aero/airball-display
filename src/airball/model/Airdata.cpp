#include "Airdata.h"

#include "aerodynamics.h"
#include "../util/units.h"
#include "telemetry/TelemetryIds.h"

#include <iostream>

namespace airball {

constexpr double kSamplesPerSecond = 20;

constexpr static std::chrono::milliseconds
    kAirdataExpiryPeriod(250);

struct AirdataMessage {
  uint16_t id;
  uint32_t seq;
  float value;
};

AirdataMessage toAirdataMessage(ITelemetry::Message message) {
  AirdataMessage r;
  r.id = message.id;
  r.seq = *((uint32_t *) message.data);
  r.value = *((float *) (message.data + 4));
  return r;
}

Airdata::Airdata(ISettings* settings)
    : settings_(settings),
      climb_rate_filter_(1),
      valid_(true),
      raw_balls_(kNumBalls),
      altitude_(0),
      climb_rate_(0) { }

static double
smooth(double current_value, double new_value, double delta_t, double time_constant) {
  double factor = exp(-1.0 * delta_t / time_constant);
  return ((1.0 - factor) * new_value) + (factor * current_value);
}

void Airdata::update(const ITelemetry::Message message) {
  switch (message.id) {
    case TelemetryIds::AIRDATA_ALPHA:
    case TelemetryIds::AIRDATA_BETA:
    case TelemetryIds::AIRDATA_Q:
    case TelemetryIds::AIRDATA_P:
    case TelemetryIds::AIRDATA_T:
      break;
    default:
      return;
  }

  AirdataMessage am = toAirdataMessage(message);

  if (raw_ == nullptr || raw_->seq != am.seq) {
    raw_ = std::make_unique<RawData>(am.seq);
  }

  switch (am.id) {
    case TelemetryIds::AIRDATA_ALPHA:
      raw_->alpha.set(am.value);
      break;
    case TelemetryIds::AIRDATA_BETA:
      raw_->beta.set(am.value);
      break;
    case TelemetryIds::AIRDATA_Q:
      raw_->q.set(am.value);
      break;
    case TelemetryIds::AIRDATA_P:
      raw_->p.set(am.value);
      break;
    case TelemetryIds::AIRDATA_T:
      raw_->t.set(am.value);
      break;
    default:
      // Should never happen
      return;
  }

  if (raw_->assigned()) {
    update(
        settings_->baro_setting() * kPascalsPerInHg,
        settings_->ball_time_constant(),
        settings_->vsi_time_constant());
    raw_ = nullptr;
  }
}

void Airdata::update(
    const double qnh,
    const double ball_time_constant,
    const double vsi_time_constant) {
  auto now = std::chrono::system_clock::now();
  double delta_t = std::chrono::duration<double>(now - lastUpdateTime_).count();
  lastUpdateTime_ = now;

  double q_corr = raw_->q.get() * settings_->q_correction_factor();

  double new_ias = q_to_ias(q_corr);
  double new_tas = q_to_tas(q_corr, raw_->p.get(), raw_->t.get());

  double alpha = degrees_to_radians(raw_->alpha.get());
  double beta = degrees_to_radians(raw_->beta.get());

  // If we allow NaN's to get through, they will "pollute" the smoothing
  // computation and every smoothed value thereafter will be NaN. This guard
  // is therefore necessary prior to using data as a smoothing filter input.
  double new_alpha = isnan(alpha) ? smooth_ball_.alpha() : alpha;
  double new_beta = isnan(beta) ? smooth_ball_.beta() : beta;
  new_ias = isnan(new_ias) ? smooth_ball_.ias() : new_ias;
  new_tas = isnan(new_tas) ? smooth_ball_.tas() : new_tas;

  smooth_ball_ = Ball(
      smooth(smooth_ball_.alpha(), alpha, delta_t, ball_time_constant),
      smooth(smooth_ball_.beta(), beta, delta_t, ball_time_constant),
      smooth(smooth_ball_.ias(), new_ias, delta_t, ball_time_constant),
      smooth(smooth_ball_.tas(), new_tas, delta_t, ball_time_constant));

  for (size_t i = raw_balls_.size() - 1; i > 0; i--) {
    raw_balls_[i] = raw_balls_[i - 1];
  }
  raw_balls_[0] = Ball(new_alpha, new_beta, new_ias, new_tas);

  pressure_altitude_ = pressure_to_altitude(raw_->t.get(), raw_->p.get(), QNH_STANDARD);

  int climbRateFilterSize = vsi_time_constant * kSamplesPerSecond;
  if (climbRateFilterSize != climb_rate_filter_.size()) {
    climb_rate_filter_ = LinearRateFilter(climbRateFilterSize);
  }

  climb_rate_filter_.put(pressure_altitude_);
  climb_rate_ = climb_rate_filter_.rate() / (1.0 / kSamplesPerSecond);

  altitude_ = pressure_to_altitude(raw_->t.get(), raw_->p.get(), qnh);

  valid_ = !isnan(alpha) && !isnan(beta);
}

bool Airdata::valid() const {
  return
      valid_ &&
      (std::chrono::system_clock::now() - lastUpdateTime_) < kAirdataExpiryPeriod;
}

} // namespace airball
