#include <iostream>
#include <vector>

#include "LinearRateFilter.h"
#include <cmath>

int main(int arg, char** argv) {
  airball::LinearRateFilter f(50);
  std::vector<double> yv;
  int ramp_n = 50;
  int hold_n = 100;
  int cycles = 3;

  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < ramp_n; j++) {
      double x = (double) j / (double) ramp_n * M_PI;
      double y = (cos(x) - 1) / 2;
      yv.push_back(y);
    }
    for (int j = 0; j < hold_n; j++) {
      yv.push_back(-1);
    }
    for (int j = 0; j < ramp_n; j++) {
      double x = ((double) j / (double) ramp_n * M_PI) + M_PI;
      double y = (cos(x) - 1) / 2;
      yv.push_back(y);
    }
    for (int j = 0; j < hold_n; j++) {
      yv.push_back(0);
    }
  }

  for (int i = 0; i < yv.size(); i++) {
    f.put(yv[i]);
    std::cout << yv[i] * 0.1 << "," << f.rate() << std::endl;
  }
}

