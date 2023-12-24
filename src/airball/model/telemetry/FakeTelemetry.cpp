#include "FakeTelemetry.h"

#include <chrono>
#include <thread>
#include <math.h>

#include "TelemetryIds.h"

namespace airball {

const static auto kSendDelay = std::chrono::milliseconds(20);
const static auto kPeriodAirdata = std::chrono::milliseconds (5000);

struct Model {
  const double min;
  const double max;
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

constexpr static Model kAirdataP {
    .min = 40000,
    .max = 70000,
};

constexpr static Model kAirdataT {
    .min =   0,
    .max =  20,
};

double interpolate_value(double phase_ratio, const Model &m) {
  double factor = (sin(phase_ratio * 2.0 * M_PI) + 1.0) / 2.0;
  return m.min + factor * (m.max - m.min);
}

double compute_phase_ratio() {
  const std::chrono::milliseconds t =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now().time_since_epoch());
  return (double) (t.count() % kPeriodAirdata.count()) / (double) kPeriodAirdata.count();
}

ITelemetry::Message make_message(uint16_t id, uint32_t seq, float value) {
  ITelemetry::Message m {
    .id = id,
    .data = { 0 },
  };
  *((uint32_t*) m.data) = seq;
  *((float*) (m.data + 4)) = value;
  return m;
}

void FakeTelemetry::make_airdata() {
  const double phase_ratio = compute_phase_ratio();
  ready_to_receive_.push_back(
      make_message(
          TelemetryIds::AIRDATA_ALPHA,
          seq_counter_,
          interpolate_value(phase_ratio, kAirdataAlpha)));
  ready_to_receive_.push_back(
      make_message(
          TelemetryIds::AIRDATA_BETA,
          seq_counter_,
          interpolate_value(phase_ratio, kAirdataBeta)));
  ready_to_receive_.push_back(
      make_message(
          TelemetryIds::AIRDATA_Q,
          seq_counter_,
          interpolate_value(phase_ratio, kAirdataQ)));
  ready_to_receive_.push_back(
      make_message(
          TelemetryIds::AIRDATA_P,
          seq_counter_,
          interpolate_value(phase_ratio, kAirdataP)));
  ready_to_receive_.push_back(
      make_message(
          TelemetryIds::AIRDATA_T,
          seq_counter_,
          interpolate_value(phase_ratio, kAirdataT)));
  seq_counter_++;
}

FakeTelemetry::FakeTelemetry()
    : seq_counter_(0) {}

ITelemetry::Message FakeTelemetry::receive() {
  if (ready_to_receive_.empty()) {
    std::this_thread::sleep_for(kSendDelay);
    make_airdata();
  }
  Message m = ready_to_receive_.front();
  ready_to_receive_.pop_front();
  return m;
}

void FakeTelemetry::send(ITelemetry::Message s) {
  // Do nothing
}

void FakeTelemetry::send(std::vector<ITelemetry::Message> m) {
  // Do nothing
}

}  // namespace airball
