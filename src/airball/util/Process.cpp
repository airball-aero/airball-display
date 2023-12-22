#include "Process.h"

namespace airball {

Process::Process() : finished_(false) {}

Process::~Process() {
  finish();
  std::unique_lock lock(join_mu_);
  if (t_.joinable()) {
    t_.join();
  }
}

void Process::start() {
  t_ = std::thread([this]() {
    while (true) {
      std::function<void()> f;
      {
        std::unique_lock<std::mutex> lock(mu_);
        while (true) {
          if (q_.empty() && finished_) {
            return;
          }
          if (q_.empty()) {
            wait_.wait(lock);
          } else {
            f = q_.front();
            q_.pop_front();
            break;
          }
        }
      }
      f();
    }
  });
}

void Process::finish() {
  std::unique_lock<std::mutex> lock(mu_);
  finished_ = true;
  wait_.notify_one();
}

void Process::enqueue(std::function<void()> f) {
  std::unique_lock<std::mutex> lock(mu_);
  q_.push_back(f);
  wait_.notify_one();
}

} // namespace airball
