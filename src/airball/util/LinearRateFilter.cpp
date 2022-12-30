#include "LinearRateFilter.h"

#include <cmath>

namespace airball {

LinearRateFilter::LinearRateFilter(int size)
    : size_(size),
      values_x_(0),
      values_y_q_(0),
      rate_(0) {
  for (int i = 0; i < size; i++) {
    values_x_.push_back(i);
  }
  for (int i = 0; i < size; i++) {
    values_y_q_.push_back(0);
  }
}

void LinearRateFilter::put(double y) {
  values_y_q_.push_back(y);
  values_y_q_.pop_front();
  compute_rate();
}

double average(const std::vector<double>& v) {
  double sum = 0;
  for (const double& i : v) {
    sum += i;
  }
  return sum / (double) v.size();
}

void LinearRateFilter::compute_rate() {
  const std::vector values_y(values_y_q_.begin(), values_y_q_.end());

  const double x_avg = average(values_x_);
  const double y_avg = average(values_y);

  double numerator = 0;
  double denominator = 0;

  for (int i = 0; i < size_; i++) {
    numerator += (values_x_[i] - x_avg) * (values_y[i] - y_avg);
  }
  for (int i = 0; i < size_; i++) {
    denominator += pow((values_x_[i] - x_avg), 2);
  }

  rate_ = numerator / denominator;
}

}