#ifndef AIRBALL_APP_IAIRDATA_H
#define AIRBALL_APP_IAIRDATA_H

#include <vector>

#include "telemetry/ITelemetry.h"

namespace airball {

class IAirdata {
public:
  class Ball {
  public:
    Ball() : alpha_(0), beta_(0), ias_(0), tas_(0) {}
    Ball(double alpha, double beta, double ias, double tas)
        : alpha_(alpha), beta_(beta), ias_(ias), tas_(tas) {}

    double alpha() const { return alpha_; }
    double beta() const { return beta_; }
    double ias() const { return ias_; }
    double tas() const { return tas_; }

  private:
    double alpha_;
    double beta_;
    double ias_;
    double tas_;
  };

  virtual void update(ITelemetry::Airdata sample) = 0;

  virtual double altitude() const = 0;
  virtual double climb_rate() const = 0;
  virtual bool valid() const = 0;
  virtual const Ball& smooth_ball() const = 0;
  virtual const std::vector<Ball>& raw_balls() const = 0;
};

} // namespace airball

#endif // AIRBALL_APP_IAIRDATA_H
