#include "AirballView.h"

#include <math.h>
#include <sstream>
#include "../util/units.h"
#include "widgets.h"

namespace airball {

constexpr double kMetersPerFoot = 0.3048;
constexpr double kSecondsPerMinute = 60;

class PaintCycle {
public:
  PaintCycle(const IAirballModel &model, IScreen *screen)
      : model_(model), screen_(screen) {}

  void paint();

private:
  struct VsiStep {
    double fpm;
    double thick;
  };

  void layout();
  void paintBackground();
  void paintRawAirballs();
  void paintRawAirball(
      const Point& center,
      const double radius,
      const double bright);
  void paintSmoothAirball();
  void paintAirballLowAirspeed(const Point& center);
  void paintAirballAirspeed(const Point& center, const double radius);
  void paintAirballAirspeedLimits(const Point& center);
  void paintAirballAirspeedLimitsNormal(const Point& center);
  void paintAirballAirspeedLimitsRotate(const Point& center);
  void paintAirballTrueAirspeed(const Point& center);
  void paintTotemPole();
  void paintTotemPoleLine();
  void paintTotemPoleAlphaX();
  void paintTotemPoleAlphaY();
  void paintCowCatcher();
  void paintVsi();
  void paintVsiTicMarks(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right,
      double radians_per_fpm);
  void paintVsiPointer(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right,
      double radians_per_fpm);
  void paintAltitude(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right);
  void paintBaroSetting(
      Point top_left,
      Point top_right,
      Point center_left,
      Point center_right,
      Point bottom_left,
      Point bottom_right);
  void paintNoFlightData();
  void paintUnitsAnnotation();
  void paintAdjusting();

  double alpha_to_y(const double alpha);
  double beta_to_x(const double beta);

  double alpha_degrees_to_y(const double alpha_degrees);
  double beta_degrees_to_x(const double beta_degrees);

  double airspeed_to_display_units(const double airspeed);
  double airspeed_to_radius(const double airspeed);
  double airspeed_display_units_to_radius(const double airspeed_display_units);

  const IAirballModel &model_;
  IScreen *screen_;

  constexpr double ce_floor(double x) {
    return static_cast<double>(static_cast<int64_t>(x));
  }

  std::string fontName_;
  double width_;
  double height_;
  double altimeterHeight_;
  double cowCatcherHeight_;
  double airballHeight_;
  double displayMargin_;
  double displayXMid_;
  double displayRegionWidth_;
  double displayRegionHalfWidth_;
  double speedLimitsRosetteHalfAngle_;
  double trueAirspeedRosetteHalfAngle_;
  double alphaRefRadius_;
  double alphaRefGapDegrees_;
  double lowSpeedThresholdAirballRadius_;
  double alphaRefTopAngle0_;
  double alphaRefTopAngle1_;
  double alphaRefBotAngle0_;
  double alphaRefBotAngle1_;
  double totemPoleAlphaUnit_;
  double statusRegionMargin_;
  int numCowCatcherLines_;
  Color background_;
  Color airballFill_;
  double rawAirballsMaxBrightness_;
  Stroke airballCrosshairsStroke_;
  double lowSpeedAirballStrokeWidth_;
  Color tasRingColor_;
  double tasRingStrokeWidth_;
  double tasThresholdRatio_;
  Stroke lowSpeedAirballStroke_;
  double lowSpeedAirballArcRadius_;
  Stroke totemPoleStroke_;
  Stroke cowCatcherStroke_;
  Stroke vfeStroke_;
  Stroke vnoStroke_;
  Stroke vneStroke_;
  Stroke vBackgroundStroke_;
  double iASTextFontSize_;
  Font iASTextFont_;
  double iASTextMargin_;
  Color iASTextColor_;
  int printBufSize_;
  Stroke noFlightDataStroke_;
  Font statusTextFont_;
  Color statusTextColor_;
  Color linkColor_;
  double vsiHeight_;
  std::vector<VsiStep> vsiStepsFpm_;
  double vsiTickLength_;
  double vsiKneeOffset_;
  Stroke vsiTickStrokeThin_;
  Stroke vsiPointerStroke_;
  Font altimeterFontLarge_;
  Font altimeterFontSmall_;
  Color altimeterTextColor_;
  Color altimeterBackgroundColor_;
  double altimeterBaselineRatio_;
  double altimeterNumberGap_;
  double baroLeftOffset_;
  Font baroFontSmall_;
  Color baroTextColor_;
};

void AirballView::paint(const IAirballModel &m, IScreen *screen) {
  PaintCycle(m, screen).paint();
}

void PaintCycle::layout() {
  fontName_ = std::string("Noto Sans");

  width_ = model_.settings()->screen_width();
  height_ = model_.settings()->screen_height();

  if (model_.settings()->show_altimeter()) {
    altimeterHeight_ = 40;
    airballHeight_ = height_ - altimeterHeight_;
  } else {
    altimeterHeight_ = 0;
    airballHeight_ = height_;
  }

  cowCatcherHeight_ = 24;

  displayMargin_ = 3;

  displayXMid_ = ce_floor(width_ / 2.0);

  displayRegionWidth_ = width_;
  displayRegionHalfWidth_ = displayRegionWidth_ / 2;

  speedLimitsRosetteHalfAngle_  = (15.0 / 2 / 180 * M_PI);
  trueAirspeedRosetteHalfAngle_ = (70.0 / 2 / 180 * M_PI);

  alphaRefRadius_ = 20;

  alphaRefGapDegrees_ = 40;

  lowSpeedThresholdAirballRadius_ = 0.05 * (width_ / 2.0);

  alphaRefTopAngle0_ = M_PI + (alphaRefGapDegrees_ / 2 / 180 * M_PI);
  alphaRefTopAngle1_ =      - (alphaRefGapDegrees_ / 2 / 180 * M_PI);

  alphaRefBotAngle0_ =        (alphaRefGapDegrees_ / 2 / 180 * M_PI);
  alphaRefBotAngle1_ = M_PI - (alphaRefGapDegrees_ / 2 / 180 * M_PI);

  totemPoleAlphaUnit_ = 20;

  numCowCatcherLines_ = 4;

  background_= Color(0, 0, 0);

  airballFill_ = Color(255, 255, 255);

  rawAirballsMaxBrightness_ = 0.375;

  airballCrosshairsStroke_ = Stroke(
      Color(128, 128, 128),
      2);

  lowSpeedAirballStrokeWidth_ = 4.0;

  tasRingColor_ = Color(255, 0, 255);

  tasRingStrokeWidth_ = 3.0;

  tasThresholdRatio_ = 0.25;

  lowSpeedAirballStroke_ = Stroke(
      Color(255, 255, 255),
      lowSpeedAirballStrokeWidth_);

  lowSpeedAirballArcRadius_ =
      lowSpeedThresholdAirballRadius_ - lowSpeedAirballStrokeWidth_ / 2.0;

  totemPoleStroke_ = Stroke(
      Color(255, 255, 0),
      3);

  cowCatcherStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  vfeStroke_ = Stroke(
      Color(255, 255, 255),
      3);

  vnoStroke_ = Stroke(
      Color(255, 255, 0),
      3);

  vneStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  vBackgroundStroke_ = Stroke(
      Color(0, 0, 0),
      6);

  iASTextFontSize_ =
      width_ / 6.0;

  iASTextFont_ = Font(
      fontName_,
      iASTextFontSize_);

  iASTextMargin_ =
      8;

  iASTextColor_ = Color(0, 0, 0);

  printBufSize_ = 128;

  noFlightDataStroke_ = Stroke(
      Color(255, 0, 0),
      3);

  statusTextFont_ = Font(fontName_, 12);
  statusTextColor_ = Color(255, 255, 255);

  linkColor_ = Color(0, 180, 180);

  vsiHeight_ = altimeterHeight_ - 2 * displayMargin_;

  vsiStepsFpm_.clear();
  vsiStepsFpm_.push_back(
      {
          .fpm = 500,
          .thick = 1,
      });
  vsiStepsFpm_.push_back(
      {
          .fpm = 1000,
          .thick = 2,
      });
  vsiStepsFpm_.push_back(
      {
          .fpm = 1500,
          .thick = 1,
      });
  vsiStepsFpm_.push_back(
      {
          .fpm = 2000,
          .thick = 2,
      });
  vsiStepsFpm_.push_back(
      {
          .fpm = 3000,
          .thick = 2,
      });

  vsiTickLength_ = 7;
  vsiKneeOffset_ = 3;
  vsiTickStrokeThin_ = Stroke(
      Color(255, 255, 255),
      2);
  vsiPointerStroke_ = Stroke(
      Color(255, 0, 255),
      4);
  altimeterFontLarge_ = Font(
      fontName_,
      width_ / 8);
  altimeterFontSmall_ = Font(
      fontName_,
      width_ / 12);
  altimeterTextColor_= Color(255, 255, 255);
  altimeterBackgroundColor_ = Color(64, 64, 64);
  altimeterBaselineRatio_ = 0.75;
  altimeterNumberGap_ = 7;

  statusRegionMargin_ = 5;

  baroLeftOffset_ =
      width_ / 12;
  baroFontSmall_ = Font(
      fontName_,
      width_ / 20);
  baroTextColor_ = Color(255, 255, 255);
}

double PaintCycle::alpha_to_y(const double alpha) {
  return alpha_degrees_to_y(radians_to_degrees(alpha));
}

double PaintCycle::alpha_degrees_to_y(const double alpha_degrees) {
  double ratio = (alpha_degrees - model_.settings()->alpha_min())
                 / (model_.settings()->alpha_max() - model_.settings()->alpha_min());
  return ratio * airballHeight_;
}

double PaintCycle::beta_to_x(const double beta) {
  return beta_degrees_to_x(radians_to_degrees(beta));
}

double PaintCycle::beta_degrees_to_x(const double beta_degrees) {
  double ratio = (beta_degrees + model_.settings()->beta_bias()) / model_.settings()->beta_full_scale();
  return displayRegionHalfWidth_ * (1.0 + ratio);
}

double PaintCycle::airspeed_to_display_units(const double airspeed) {
  if (model_.settings()->speed_units() == "knots") {
    return meters_per_second_to_knots(airspeed);
  }
  if (model_.settings()->speed_units() == "mph") {
    return meters_per_second_to_mph(airspeed);
  }
  return 0.0;
}

double PaintCycle::airspeed_to_radius(const double airspeed) {
  return airspeed_display_units_to_radius(airspeed_to_display_units(airspeed));
}

double PaintCycle::airspeed_display_units_to_radius(const double airspeed_display_units) {
  double ratio = airspeed_display_units / model_.settings()->v_full_scale();
  return ratio * width_ / 2;
}

void PaintCycle::paint() {
  layout();

  // cairo_push_group(screen_->cr());

  if (model_.settings()->rotate_screen()) {
    cairo_translate(screen_->cr(), 0, width_);
    cairo_rotate(screen_->cr(), -M_PI / 2);
  }

  cairo_save(screen_->cr());

  cairo_rectangle(screen_->cr(), 0, 0, width_, airballHeight_);
  cairo_clip(screen_->cr());

  paintBackground();

  if (model_.airdata()->valid()) {
    paintRawAirballs();
    paintSmoothAirball();
  } else {
    paintNoFlightData();
  }

  paintTotemPole();
  paintCowCatcher();
  paintUnitsAnnotation();
  paintAdjusting();

  cairo_restore(screen_->cr());

  if (model_.settings()->show_altimeter()) {
    cairo_save(screen_->cr());
    cairo_rectangle(screen_->cr(), 0, airballHeight_ + displayMargin_, width_, vsiHeight_);
    cairo_clip(screen_->cr());
    paintVsi();
    cairo_restore(screen_->cr());
  }

  // cairo_restore(screen_->cr());

  cairo_surface_flush(screen_->cs());
  screen_->flush();
}

void PaintCycle::paintBackground() {
  rectangle(
      screen_->cr(),
      Point(0, 0),
      Size(width_, height_),
      background_);
}

void PaintCycle::paintRawAirballs() {
  for (uint i = model_.airdata()->raw_balls().size(); i-- > 0; ) {
    uint bright_index = model_.airdata()->raw_balls().size() - i;
    double bright =
        ((double) bright_index) / ((double) model_.airdata()->raw_balls().size()) *
        rawAirballsMaxBrightness_;
    Point center(
        beta_to_x(model_.airdata()->raw_balls()[i].beta()),
        alpha_to_y((model_.airdata()->raw_balls()[i].alpha())));
    double radius = airspeed_to_radius(model_.airdata()->raw_balls()[i].ias());
    paintRawAirball(center, radius, bright);
  }
}

void PaintCycle::paintRawAirball(
    const Point& center,
    const double radius,
    const double bright) {
  disc(
      screen_->cr(),
      center,
      radius,
      airballFill_.with_brightness(bright));
}

void PaintCycle::paintSmoothAirball() {
  Point center(beta_to_x(model_.airdata()->smooth_ball().beta()), alpha_to_y((model_.airdata()->smooth_ball().alpha())));
  double radius = airspeed_to_radius(model_.airdata()->smooth_ball().ias());
  if (radius < lowSpeedThresholdAirballRadius_) {
    paintAirballLowAirspeed(center);
  } else {
    paintAirballAirspeed(center, radius);
    paintAirballTrueAirspeed(center);
    paintAirballAirspeedLimits(center);
  }
}

void PaintCycle::paintAirballLowAirspeed(const Point& center) {
  arc(
      screen_->cr(),
      center,
      lowSpeedAirballArcRadius_,
      0,
      2.0 * M_PI,
      lowSpeedAirballStroke_);
}

void PaintCycle::paintAirballAirspeed(const Point& center, const double radius) {
  // Determine the size of the airspeed text
  char airspeedText[printBufSize_];
  double ias_display_units_ =
      meters_per_second_to_mph(model_.airdata()->smooth_ball().ias());
  snprintf(
      airspeedText,
      printBufSize_,
      "%.0f",
      ias_display_units_);
  Size airspeedTextSize =
      text_size(screen_->cr(), airspeedText, iASTextFont_);
  Size airspeedBoundingBoxSize(
      airspeedTextSize.w() + 2 * iASTextMargin_,
      airspeedTextSize.h() + 2 * iASTextMargin_);

  double airspeedTickMarkLength = 4; // IHAB todo
  double airspeedTickMarkStrokeWidth = 3; // IHAB todo

  line(
      screen_->cr(),
      Point(center.x() - (airspeedBoundingBoxSize.w() / 2 + airspeedTickMarkLength), center.y()),
      Point(center.x() + (airspeedBoundingBoxSize.w() / 2 + airspeedTickMarkLength), center.y()),
      Stroke(airballFill_, airspeedTickMarkStrokeWidth)); // IHAB todo

  line(
      screen_->cr(),
      Point(center.x(), center.y() - (airspeedBoundingBoxSize.h() / 2 + airspeedTickMarkLength)),
      Point(center.x(), center.y() + (airspeedBoundingBoxSize.h() / 2 + airspeedTickMarkLength)),
      Stroke(airballFill_, airspeedTickMarkStrokeWidth)); // IHAB todo

  disc(
      screen_->cr(),
      center,
      radius,
      airballFill_);
  line(
      screen_->cr(),
      Point(center.x(), center.y() - radius),
      Point(center.x(), center.y() + radius),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(center.x() - radius, center.y()),
      Point(center.x() + radius, center.y()),
      airballCrosshairsStroke_);

  round_rectangle(
      screen_->cr(),
      Point(
          center.x() - airspeedBoundingBoxSize.w() / 2,
          center.y() - airspeedBoundingBoxSize.h() / 2),
      airspeedBoundingBoxSize,
      iASTextMargin_,
      airballFill_); // IHAB todo

  draw_text(
      screen_->cr(),
      airspeedText,
      center,
      TextReferencePoint::CENTER_MID_UPPERCASE,
      iASTextFont_,
      iASTextColor_);
}

void PaintCycle::paintAirballAirspeedLimits(const Point& center) {
  if (meters_per_second_to_mph(model_.airdata()->smooth_ball().ias()) < model_.settings()->v_r()) {
    paintAirballAirspeedLimitsRotate(center);
  } else {
    paintAirballAirspeedLimitsNormal(center);
  }
}

void PaintCycle::paintAirballAirspeedLimitsRotate(const Point& center) {
  if (model_.settings()->v_r() == 0) {
    return;
  }
  double r = airspeed_display_units_to_radius(model_.settings()->v_r());
  line(
      screen_->cr(),
      Point(
          center.x() - r,
          center.y()),
      Point(
          center.x() - r - totemPoleAlphaUnit_,
          center.y() + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() - r,
          center.y()),
      Point(
          center.x() - r - totemPoleAlphaUnit_,
          center.y() - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + totemPoleAlphaUnit_,
          center.y() + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x() + r,
          center.y()),
      Point(
          center.x() + r + totemPoleAlphaUnit_,
          center.y() - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() + totemPoleAlphaUnit_,
          center.y() + r + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() + r),
      Point(
          center.x() - totemPoleAlphaUnit_,
          center.y() + r + totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() + totemPoleAlphaUnit_,
          center.y() - r - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
  line(
      screen_->cr(),
      Point(
          center.x(),
          center.y() - r),
      Point(
          center.x() - totemPoleAlphaUnit_,
          center.y() - r - totemPoleAlphaUnit_),
      airballCrosshairsStroke_);
}

void PaintCycle::paintAirballAirspeedLimitsNormal(const Point& center) {
  if (model_.settings()->v_fe() > 0) {
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_fe()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vBackgroundStroke_);
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_fe()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vfeStroke_);
  }
  if (model_.settings()->v_no() > 0) {
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_no()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vBackgroundStroke_);
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_no()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vnoStroke_);
  }
  if (model_.settings()->v_ne() > 0) {
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_ne()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vBackgroundStroke_);
    rosette(
        screen_->cr(),
        center,
        airspeed_display_units_to_radius(model_.settings()->v_ne()),
        4,
        speedLimitsRosetteHalfAngle_,
        M_PI_4,
        vneStroke_);
  }
}

void PaintCycle::paintAirballTrueAirspeed(const Point& center) {
  double tas_stroe_alpha_ = 0;
  if (model_.airdata()->smooth_ball().tas() <
      model_.airdata()->smooth_ball().ias() || model_.airdata()->smooth_ball().ias() == 0) {
    tas_stroe_alpha_ = 0;
  } else {
    double ias_squared =
        model_.airdata()->smooth_ball().ias() * model_.airdata()->smooth_ball().ias();
    double tas_squared =
        model_.airdata()->smooth_ball().tas() * model_.airdata()->smooth_ball().tas();
    double ratio = (tas_squared - ias_squared) / ias_squared;
    tas_stroe_alpha_ = (ratio > tasThresholdRatio_)
                       ? 1.0 : (ratio / tasThresholdRatio_);
  }
  rosette(
      screen_->cr(),
      center,
      airspeed_to_radius(model_.airdata()->smooth_ball().tas()),
      4,
      trueAirspeedRosetteHalfAngle_,
      0,
      Stroke(
          tasRingColor_.with_alpha(tas_stroe_alpha_),
          tasRingStrokeWidth_));
}

void PaintCycle::paintTotemPole() {
  paintTotemPoleLine();
  paintTotemPoleAlphaX();
  paintTotemPoleAlphaY();
}

void PaintCycle::paintTotemPoleLine() {
  if (model_.settings()->declutter()) {
    line(
        screen_->cr(),
        Point(displayXMid_, 0),
        Point(displayXMid_,airballHeight_),
        totemPoleStroke_);
  } else {
    line(
        screen_->cr(),
        Point(displayXMid_, 0),
        Point(displayXMid_,
              alpha_degrees_to_y(model_.settings()->alpha_ref()) - alphaRefRadius_),
        totemPoleStroke_);
    line(
        screen_->cr(),
        Point(displayXMid_,
              alpha_degrees_to_y(model_.settings()->alpha_ref()) + alphaRefRadius_),
        Point(displayXMid_, airballHeight_),
        totemPoleStroke_);
    arc(
        screen_->cr(),
        Point(displayXMid_, alpha_degrees_to_y(model_.settings()->alpha_ref())),
        alphaRefRadius_,
        alphaRefTopAngle0_,
        alphaRefTopAngle1_,
        totemPoleStroke_);
    arc(
        screen_->cr(),
        Point(displayXMid_, alpha_degrees_to_y(model_.settings()->alpha_ref())),
        alphaRefRadius_,
        alphaRefBotAngle0_,
        alphaRefBotAngle1_,
        totemPoleStroke_);
  }
}

void PaintCycle::paintTotemPoleAlphaX() {
  if (model_.settings()->declutter()) {
    return;
  }
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      Point(
          displayXMid_ - 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      Point(
          displayXMid_ - 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x()) - totemPoleAlphaUnit_) ,
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      Point(
          displayXMid_ + 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 2 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x())),
      Point(
          displayXMid_ + 3 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_x()) - totemPoleAlphaUnit_) ,
      totemPoleStroke_);
}

void PaintCycle::paintTotemPoleAlphaY() {
  if (model_.settings()->declutter()) {
    return;
  }
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 4 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      Point(
          displayXMid_ - 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ - 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      Point(
          displayXMid_ - 6 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y()) - totemPoleAlphaUnit_),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 4 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      Point(
          displayXMid_ + 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      totemPoleStroke_);
  line(
      screen_->cr(),
      Point(
          displayXMid_ + 5 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y())),
      Point(
          displayXMid_ + 6 * totemPoleAlphaUnit_,
          alpha_degrees_to_y(model_.settings()->alpha_y()) - totemPoleAlphaUnit_),
      totemPoleStroke_);
}

void PaintCycle::paintCowCatcher() {
  if (model_.settings()->declutter()) {
    return;
  }
  double xStep =
      (displayRegionWidth_ - (2 * displayMargin_)) /
      (2 * numCowCatcherLines_);
  double yStall = alpha_degrees_to_y(model_.settings()->alpha_stall());
  for (int i = 0; i < numCowCatcherLines_; i++) {
    line(
        screen_->cr(),
        Point(
            displayXMid_ + i * xStep,
            yStall),
        Point(
            displayXMid_ + (i + 1) * xStep,
            yStall + cowCatcherHeight_),
        cowCatcherStroke_);
    line(
        screen_->cr(),
        Point(
            displayXMid_ - i * xStep,
            yStall),
        Point(
            displayXMid_ - (i + 1) * xStep,
            yStall + cowCatcherHeight_),
        cowCatcherStroke_);
  }
  line(
      screen_->cr(),
      Point(
          displayXMid_ - (numCowCatcherLines_ - 1) * xStep,
          yStall),
      Point(
          displayXMid_ + (numCowCatcherLines_ - 1) * xStep,
          yStall),
      cowCatcherStroke_);
}

void PaintCycle::paintVsi() {
  double radians_per_fpm =
      atan(vsiHeight_ / 2 / width_) / vsiStepsFpm_[0].fpm;
  Point top_left(
      0,
      airballHeight_ + displayMargin_);
  Point top_right(
      width_,
      top_left.y());
  Point bottom_left(
      top_left.x(),
      top_left.y() + vsiHeight_);
  Point bottom_right(
      top_right.x(),
      bottom_left.y());
  Point center_left(
      top_left.x(),
      top_left.y() + vsiHeight_ / 2);
  Point center_right(
      top_right.x(),
      center_left.y());
  rectangle(
      screen_->cr(),
      top_left,
      Size(
          width_,
          vsiHeight_),
      altimeterBackgroundColor_);
  paintVsiTicMarks(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right,
      radians_per_fpm);
  paintVsiPointer(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right,
      radians_per_fpm);
  paintAltitude(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right);
  paintBaroSetting(
      top_left,
      top_right,
      center_left,
      center_right,
      bottom_left,
      bottom_right);
}

void PaintCycle::paintVsiTicMarks(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right,
    double radians_per_fpm) {
  line(
      screen_->cr(),
      top_right,
      Point(
          top_right.x(),
          top_right.y() + vsiTickLength_),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      top_right,
      Point(
          top_right.x() - vsiTickLength_,
          top_right.y()),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x(),
          bottom_right.y() - vsiTickLength_),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      bottom_right,
      Point(
          bottom_right.x() - vsiTickLength_,
          bottom_right.y()),
      vsiTickStrokeThin_);
  line(
      screen_->cr(),
      center_right,
      Point(
          center_right.x() - vsiTickLength_,
          center_right.y()),
      vsiTickStrokeThin_);
  for (auto i = vsiStepsFpm_.begin(); i < vsiStepsFpm_.end(); ++i) {
    double step_x =
        (center_left.y() - top_left.y()) /
        tan(i->fpm * radians_per_fpm);
    Stroke stroke(
        vsiTickStrokeThin_.color(),
        vsiTickStrokeThin_.width() * i->thick);
    line(
        screen_->cr(),
        Point(
            step_x,
            top_left.y()),
        Point(
            step_x,
            top_left.y() + vsiTickLength_),
        stroke);
    line(
        screen_->cr(),
        Point(
            step_x,
            bottom_left.y() - vsiTickLength_),
        Point(
            step_x,
            bottom_left.y()),
        stroke);
  }
}

void PaintCycle::paintVsiPointer(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right,
    double radians_per_fpm) {
  double climb_rate =
      model_.airdata()->climb_rate() / kMetersPerFoot * kSecondsPerMinute;
  climb_rate = fmin(climb_rate, vsiStepsFpm_.back().fpm);
  climb_rate = fmax(climb_rate, -vsiStepsFpm_.back().fpm);
  double angle = climb_rate * radians_per_fpm;
  if (fabs(climb_rate) <= vsiStepsFpm_[0].fpm) {
    line(
        screen_->cr(),
        center_left,
        Point(
            center_right.x(),
            center_left.y() - (center_right.x() - center_left.x()) * sin(angle)),
        vsiPointerStroke_);
  } else {
    double dx = (center_left.y() - top_left.y()) / tan(angle);
    Point nee_(
        center_left.x() + fabs(dx),
        dx < 0 ? bottom_left.y() : top_left.y());
    line(
        screen_->cr(),
        center_left,
        nee_,
        vsiPointerStroke_);
    Point a(
        nee_.x(),
        dx < 0
            ? nee_.y() - vsiKneeOffset_
            : nee_.y() + vsiKneeOffset_);
    Point b(
        center_right.x(),
        a.y());
    line(
        screen_->cr(),
        a,
        b,
        vsiPointerStroke_);
  }
}

void PaintCycle::paintAltitude(
  Point top_left,
  Point top_right,
  Point center_left,
  Point center_right,
  Point bottom_left,
  Point bottom_right) {
  int altitude = (int) round(model_.airdata()->altitude() / kMetersPerFoot);
  int thousands = abs(altitude) / 1000;
  int last_three_digits = (abs(altitude) - (thousands * 1000)) / 10 * 10;
  Point baseline(
      center_left.x() + (center_right.x() - center_left.x())
                        * altimeterBaselineRatio_,
      center_left.y());
  char buf[printBufSize_];
  memset(buf, 0, sizeof(buf));
  // Print the thousands string
  if (thousands == 0) {
    if (altitude < 0) {
      // Print just a negative sign
      snprintf(
          buf,
          printBufSize_,
          "-");
    } else {
      // Leave the thousands blank
    }
  } else {
    if (altitude < 0) {
      snprintf(
          buf,
          printBufSize_,
          "-%d",
          thousands);
    } else {
      // Leave the thousands blank
      snprintf(
          buf,
          printBufSize_,
          "%d",
          thousands);
    }
  }
  draw_text(
      screen_->cr(),
      buf,
      Point(
          baseline.x() - altimeterNumberGap_,
          baseline.y()),
      TextReferencePoint::CENTER_RIGHT_UPPERCASE,
      altimeterFontLarge_,
      altimeterTextColor_);
  snprintf(
      buf,
      printBufSize_,
      "%03d",
      last_three_digits);
  draw_text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      altimeterFontSmall_,
      altimeterTextColor_);
}

void PaintCycle::paintBaroSetting(
    Point top_left,
    Point top_right,
    Point center_left,
    Point center_right,
    Point bottom_left,
    Point bottom_right) {
  Point baseline(
      center_left.x() + baroLeftOffset_,
      center_left.y());
  char buf[printBufSize_];
  snprintf(
      buf,
      printBufSize_,
      "%04.2f",
      model_.settings()->baro_setting());
  draw_text(
      screen_->cr(),
      buf,
      baseline,
      TextReferencePoint::CENTER_LEFT_UPPERCASE,
      baroFontSmall_,
      baroTextColor_);
}

void PaintCycle::paintNoFlightData() {
  line(
      screen_->cr(),
      Point(0, 0),
      Point(width_, airballHeight_),
      noFlightDataStroke_);
  line(
      screen_->cr(),
      Point(width_, 0),
      Point(0, airballHeight_),
      noFlightDataStroke_);
}

void PaintCycle::paintUnitsAnnotation() {
  std::stringstream buf;
  buf << model_.settings()->speed_units();
  if (model_.settings()->show_altimeter()) {
    buf << " ft fpm inHg";
  }
  draw_text(
      screen_->cr(),
      buf.str(),
      Point(statusRegionMargin_, statusRegionMargin_),
      TextReferencePoint ::TOP_LEFT,
      statusTextFont_,
      statusTextColor_);
}

void PaintCycle::paintAdjusting() {
  if (model_.settings()->adjustment() == ISettings::ADJUSTMENT_NONE) {
    return;
  }
  std::stringstream buf;
  buf << "ADJ: ";
  switch (model_.settings()->adjustment()) {
    case ISettings::ADJUSTMENT_AUDIO_VOLUME:
      buf << "VOL";
      break;
    case ISettings::ADJUSTMENT_BARO_SETTING:
      buf << "BARO";
      break;
    case ISettings::ADJUSTMENT_SCREEN_BRIGHTNESS:
      buf << "BRT";
      break;
    default:
      return;
  }
  draw_text(
      screen_->cr(),
      buf.str(),
      Point(width_ - statusRegionMargin_, statusRegionMargin_),
      TextReferencePoint ::TOP_RIGHT,
      statusTextFont_,
      statusTextColor_);
}

} // namespace airball
