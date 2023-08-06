#include "aerodynamics.h"

#include "../util/units.h"

#include <math.h>

namespace airball {

constexpr static double kDensityAirStandard = 1.225;
constexpr static double kDryAirGasConstant = 287.058;

double ias_to_q(double ias) {
  return 0.5 * kDensityAirStandard * pow(ias, 2);
}

double q_to_ias(double q) {
  return sqrt(2.0 * q / kDensityAirStandard);
}

double dry_air_density(double p, double t) {
  return p / (kDryAirGasConstant * celsius_to_kelvin(t));
}

double q_to_tas(double q, double p, double t) {
  return sqrt(2.0 * q / dry_air_density(p, t));
}

double pressure_to_altitude(double t, double p, double qnh) {
  return 44330.0 * (1.0 - pow(p / qnh, 1.0 / 5.255));
}

} // namespace airball
