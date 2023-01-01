#include "one_shot_timer.h"

#include <iostream>

namespace airball {

const auto kResolution = std::chrono::milliseconds(1);

OneShotTimer::OneShotTimer(
    std::chrono::steady_clock::duration duration,
    std::function<void()> callback)
    : callback_(callback),
      running_(true) {
  end_ = std::chrono::steady_clock::now() + duration;
  thread_ = std::thread([this]() { run(); });
}

OneShotTimer::~OneShotTimer() {
  cancel();
  thread_.join();
}

void OneShotTimer::cancel() {
  std::lock_guard<std::mutex> lock(mu_);
  running_ = false;
}

void OneShotTimer::run() {
  while (true) {
    {
      std::lock_guard<std::mutex> lock(mu_);
      if (!running_) {
        return;
      }
      if (std::chrono::steady_clock::now() > end_) {
        callback_();
        return;
      }
    }
    std::this_thread::sleep_for(kResolution);
  }
}

}  // namespace airball
