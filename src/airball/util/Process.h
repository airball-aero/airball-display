#ifndef SRC_AIRBALL_UTIL_EVENT_QUEUE_H
#define SRC_AIRBALL_UTIL_EVENT_QUEUE_H

#include <deque>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace airball {

class Process {
public:
  Process();
  ~Process();

protected:
  void start();
  void finish();
  void enqueue(std::function<void()> f);

private:
  std::deque<std::function<void()>> q_;
  std::thread t_;
  std::mutex mu_;
  std::mutex join_mu_;
  bool finished_;
  std::condition_variable wait_;
};

} // namespace airball

#endif //SRC_AIRBALL_UTIL_EVENT_QUEUE_H
