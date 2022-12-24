#ifndef FRAMEWORK_VIEW_H
#define FRAMEWORK_VIEW_H

#include "IScreen.h"

namespace airball {

template <typename Model>
class IView {
public:
  virtual void paint(const Model& m, IScreen* screen) = 0;
};

} // namespace airball

#endif // FRAMEWORK_VIEW_H
