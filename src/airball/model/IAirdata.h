#ifndef AIRBALL_APP_IAIRDATA_H
#define AIRBALL_APP_IAIRDATA_H

#include <vector>

namespace airball {

class IAirdata {
public:
  struct Ball {
    double alpha;
    double beta;
    double ias;
    double tas;
  };

  virtual double altitude() const = 0;
  virtual double climb_rate() const = 0;
  virtual bool valid() const = 0;
  virtual const Ball& smooth_ball() const = 0;
  virtual const std::vector<Ball>& raw_balls() const = 0;
};

} // namespace airball

#endif // AIRBALL_APP_IAIRDATA_H
