#ifndef AIRBALL_LINEAR_RATE_FILTER_H_
#define AIRBALL_LINEAR_RATE_FILTER_H_

#include <vector>
#include <deque>

namespace airball {

class LinearRateFilter {
public:
  explicit LinearRateFilter(int size);

  [[nodiscard]] int size() const { return size_; }
  [[nodiscard]] double rate() const { return rate_; }

  void put(double y);

private:
  void compute_rate();

  std::vector<double> values_x_;
  std::deque<double> values_y_q_;

  int size_;
  double rate_;
};

} // namespace airball

#endif // AIRBALL_LINEAR_RATE_FILTER_H_
