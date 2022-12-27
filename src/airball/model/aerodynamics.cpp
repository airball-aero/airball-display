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

static constexpr double kAltK = 1.313e-05;
static constexpr double kAltN = 0.1903;
static constexpr double kAltPb = 29.92126;

// The formulae are given in English units. To maintain a consistent API
// throughout our system, the function inputs and outputs are in SI units. The
// tradeoff is we do a few more unit conversions than needed, in return for a
// hopefully less error-prone set of APIs.
double pressure_to_altitude(double t, double p, double qnh) {
  double ratio = (qnh / p);
  double t_kelvin = t + 273.15;
  double power = 1 / 5.257;
  return (pow(ratio, power) - 1) * t_kelvin / 0.0065;
}

} // namespace airball
