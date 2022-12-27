#ifndef SRC_FRAMEWORK_IEVENTQUEUE_H
#define SRC_FRAMEWORK_IEVENTQUEUE_H

#include <functional>

namespace airball {

class IEventQueue {
public:
  virtual void enqueue(std::function<void()> event) = 0;
};

} // namespace airball

#endif //SRC_FRAMEWORK_IEVENTQUEUE_H
