#include "FakeTelemetry.h"

#include <chrono>
#include <thread>
#include <math.h>

namespace airball {

const static auto kSendDelay = std::chrono::milliseconds(20);
const static auto kPeriodAirdata = std::chrono::milliseconds (20000);

struct Model {
  const double min;
  const double max;
};

constexpr static Model kAirdataBaro {
    .min = 40000,
    .max = 70000,
};

constexpr static Model kAirdataOat {
    .min =   0,
    .max =  20,
};

constexpr static Model kAirdataAlpha {
    .min = 0,
    .max = 15,
};

constexpr static Model kAirdataBeta {
    .min = -5,
    .max = 5,
};

constexpr static Model kAirdataQ {
    .min = 140,
    .max = 1300,
};

double interpolate_value(double phase_ratio, const Model &m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m.min + factor * (m.max - m.min);
}

double compute_phase_ratio(const std::chrono::steady_clock::duration &period) {
  const long now = std::chrono::steady_clock::now().time_since_epoch().count();
  return (double) (now % period.count()) / (double) period.count();
}

ITelemetry::Sample make_airdata(unsigned long seq) {
  const double phase_ratio = compute_phase_ratio(kPeriodAirdata);
  return ITelemetry::Sample {
    .type = ITelemetry::AIRDATA,
    .sequence = seq,
    .airdata = {
        .alpha = interpolate_value(phase_ratio, kAirdataAlpha),
        .beta = interpolate_value(phase_ratio, kAirdataBeta),
        .q = interpolate_value(phase_ratio, kAirdataQ),
        .p = interpolate_value(phase_ratio, kAirdataBaro),
        .t = interpolate_value(phase_ratio, kAirdataOat),
    },
  };
}

FakeTelemetry::FakeTelemetry()
    : seq_counter_(0) {}

ITelemetry::Sample FakeTelemetry::get() {
  std::this_thread::sleep_for(kSendDelay);
  return make_airdata(seq_counter_++);
}

}  // namespace airball