#include <iostream>
#include "stallfence_scheme.h"
#include "../util/units.h"

namespace airball {

constexpr double kPeriodRamp = 0.005; // seconds

constexpr double kFrequencyAlphaMin = 220; // hertz
constexpr double kOctavesMinToMax = 2; // octaves

constexpr double kPwmOnOffPeriodBase = 0.05; // seconds

constexpr double kPwmFrequencyAlphaMin = 1; // hertz
constexpr double kPwmFrequencyAlphaMax = 10; //hertz

constexpr double kBetaAmplificationFactor = 6.0;

stallfence_scheme::stallfence_scheme()
    : tone_(1),
      pwm_(1, 1, 1),
      balance_(1, 1, 1) { }

void stallfence_scheme::install(ISoundMixer* mixer) {
  mixer->set_layers({
                        &tone_,
                        &pwm_,
                        &balance_,
                    });
}
void stallfence_scheme::remove(ISoundMixer* mixer) {
  mixer->set_layers({});
}

void stallfence_scheme::update(const IAirballModel& model, ISoundMixer* mixer) {
  IAirdata::Ball ball = model.airdata()->smooth_ball();

  const double alpha = radians_to_degrees(ball.alpha());
  const double beta = radians_to_degrees(ball.beta());

  // Calculate non-dimensional ratios for alpha and beta
  double alpha_ratio =
      (alpha - model.settings()->alpha_min()) /
      (model.settings()->alpha_stall() - model.settings()->alpha_min());
  double beta_ratio =
      (beta + model.settings()->beta_bias()) / model.settings()->beta_full_scale();
  if (alpha > model.settings()->alpha_ref()) {
    beta_ratio *= kBetaAmplificationFactor *
        ((alpha - model.settings()->alpha_ref()) /
        (model.settings()->alpha_stall() - model.settings()->alpha_ref()));
  }
  beta_ratio = std::min(1.0, std::max(-1.0, beta_ratio));

  // Set base tone frequency based on alpha
  tone_.set_period(
      mixer->octaves_to_period(
          kFrequencyAlphaMin,
          alpha_ratio * kOctavesMinToMax));

  // Ramp period for all signals
  snd_pcm_uframes_t ramp_period =
      mixer->seconds_to_frames(kPeriodRamp);

  // Calculate and set PWM parameters based on alpha
  snd_pcm_uframes_t pwm_on_off_period_base =
      mixer->seconds_to_frames(kPwmOnOffPeriodBase);
  snd_pcm_uframes_t pwm_period = mixer->frequency_to_period(
      kPwmFrequencyAlphaMin +
      alpha_ratio * (kPwmFrequencyAlphaMax - kPwmFrequencyAlphaMin));
  pwm_period = std::min(
      pwm_period,
      mixer->frequency_to_period(kPwmFrequencyAlphaMin));
  snd_pcm_uframes_t pwm_on_period = 0;
  snd_pcm_uframes_t pwm_ramp_period = 0;
  if ((pwm_period / 2) < pwm_on_off_period_base) {
    // No "room" for a square wave; revert to continuous tone
    pwm_on_period = pwm_period;
    pwm_ramp_period = 0;
  } else {
    // We have "room" to do a PWM square wave
    pwm_on_period = pwm_period - pwm_on_off_period_base;
    pwm_ramp_period = ramp_period;
  }
  pwm_.set_parameters(pwm_period, pwm_on_period, pwm_ramp_period);

  // Calculate and set balance and combined volume based on alpha and beta
  balance_.set_ramp_period(ramp_period);
  if (alpha < model.settings()->alpha_ref()) {
    balance_.set_gains(
        model.settings()->audio_volume() *
        std::max(0.0, -beta_ratio * 0.5),
        model.settings()->audio_volume() *
        std::max(0.0, beta_ratio * 0.5));
  } else if (alpha < model.settings()->alpha_stall()) {
    double alpha_high_range_ratio =
        (alpha - model.settings()->alpha_ref()) /
        (model.settings()->alpha_stall() - model.settings()->alpha_ref());
    balance_.set_gains(
        model.settings()->audio_volume() *
        std::min(1.0, (-beta_ratio * 0.5 + alpha_high_range_ratio) / 2.0),
        model.settings()->audio_volume() *
        std::min(1.0, (beta_ratio * 0.5 + alpha_high_range_ratio) / 2.0));
  } else {
    balance_.set_gains(
        model.settings()->audio_volume() *
        std::min(1.0, (-beta_ratio + 1) / 2.0),
        model.settings()->audio_volume() *
        std::min(1.0, (beta_ratio + 1) / 2.0));
  }
}

} // namespace airball