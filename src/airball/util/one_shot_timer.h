#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace airball {

/**
 * A OneShotTimer allows a client to arrange for a given function to be called
 * after a given period of wall clock time has elapsed.
 */
class OneShotTimer {
public:
  /**
   * Create a new Timer.
   *
   * @param interval the interval that the timer will wait before calling the
   *     supplied function.
   * @param callback the function that will be called when the timer fires.
   */
  OneShotTimer(
      std::chrono::steady_clock::duration duration,
      std::function<void()> callback);
  ~OneShotTimer();

  /**
   * Cancel the timer. The timer cannot be re-used.
   */
  void cancel();

private:
  void run();

  std::mutex mu_;
  std::thread thread_;
  bool running_;
  const std::function<void()> callback_;
  std::chrono::steady_clock::time_point end_;
};

}  // namespace airball