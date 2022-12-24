#ifndef AIRBALL_FRAMEWORK_I_VIEW_H
#define AIRBALL_FRAMEWORK_I_VIEW_H

#include "IScreen.h"

namespace airball {

template <typename Model>
class IView {
public:
  virtual void paint(const Model& m, IScreen* screen) = 0;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_I_VIEW_H
