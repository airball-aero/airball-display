#include <iostream>
#include "gflags/gflags.h"

#include "../../framework/Application.h"
#include "../model/IAirballModel.h"
#include "../screen/x11_screen.h"

const std::string kScreenX11 = "x11";
const std::string kScreenSt7789vi = "st7789vi";
DEFINE_string(screen, kScreenX11, "Screen implementation (x11, st7789vi)");

namespace airball {

// In general, there is one and only concrete class AbView because that is
// the abstraction of the UI that we are building here.

IScreen* buildScreen() {
  if (FLAGS_screen == kScreenX11) {
    return new X11Screen(400, 300);
  }
  #ifdef AIRBALL_BCM2835
  if (FLAGS_screen = kScreenSt7789vi) {
    return new St7789viScreen(400, 300);
  }
  #endif

  exit(-1);
}

class AirballApplication : public Application<IAirballModel> {
public:
  AirballApplication() = default;
  virtual ~AirballApplication() = default;

protected:
  void initialize() override {
    setView(nullptr);
    setScreen(std::unique_ptr<IScreen>(buildScreen()));

    // Call these here ...
    // void set_model(std::unique_ptr<Model> m) { model_ = std::move(m); }
    // void set_view(std::unique_ptr<View<Model>> v) { view_ = std::move(v); };
    // void set_screen(std::unique_ptr<Screen> s) { screen_ = std::move(s); };
    // void set_frame_interval(std::chrono::duration<double, std::milli> frame_interval) { frame_interval_ = frame_interval; }

    // We have access to command line arguments right here
    // Use these to construct the stuff we want
    // REMEMBER: Some aspects of the "model" may have access to other aspects! So for example, various
    // internal pieces of the Model may want access to the Settings, but the UI also wants access
    // to the Settings. So basically there is a Model graph, and the UI sees a subgraph of this, which
    // is exposed via interfaces.

    // It is arguable that AbModel (or whatever) can be a concrete class -- so long as its components are
    // configurable via the command line arguments. Which I guess is fine, right? You don't 100% need
    // interfaces for everything. Though of course it's nice....

    // Then we relinquish control and let things work from here on
  }
};

} // namespace airball

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  airball::AirballApplication app;
  app.run();
}