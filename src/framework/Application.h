#ifndef AIRBALL_FRAMEWORK_APPLICATION_H
#define AIRBALL_FRAMEWORK_APPLICATION_H

#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

#include "IScreen.h"
#include "IView.h"
#include "ISoundScheme.h"
#include "IEventQueue.h"

namespace airball {

template <typename Model>
class Application {
public:
  Application()
      : running_(true) {
    eventQueue_.reset(new EventQueueImpl(&eventsMu_, &events_));
  }

  virtual ~Application() = default;

  bool running() {
    std::lock_guard<std::mutex> lock(runningMu_);
    return running_;
  }

  void run() {
    initialize();
    // soundScheme_->install(soundMixer_.get());
    while (running()) {
      auto start = std::chrono::steady_clock::now();
      std::vector<std::function<void()>> currentEvents;
      {
        std::lock_guard<std::mutex> lock(eventsMu_);
        currentEvents = std::move(events_);
      }
      for (const auto & event : currentEvents) {
        event();
      }
      view_->paint(*model_, screen_.get());
      screen_->flush();
      // soundScheme_->update(*model_, soundMixer_.get());
      std::this_thread::sleep_for(
          frameInterval_ -
          (std::chrono::steady_clock::now() - start));
    }
    // soundScheme_->remove(soundMixer_.get());
  }

  void stop() {
    std::lock_guard<std::mutex> lock(runningMu_);
    running_ = false;
  }

protected:
  // The application's data model
  void setModel(std::unique_ptr<Model> m) { model_ = std::move(m); }

  // The user interfaces for visuals and sound
  void setView(std::unique_ptr<IView<Model>> v) { view_ = std::move(v); };
  void setSoundScheme(std::unique_ptr<ISoundScheme<Model>> s) { soundScheme_ = std::move(s); };

  // The output devices for visuals and sound
  void setScreen(std::unique_ptr<IScreen> s) { screen_ = std::move(s); };
  void setSoundMixer(std::unique_ptr<ISoundMixer> m) { soundMixer_ = std::move(m); }

  void setFrameInterval(std::chrono::duration<double, std::milli> i) { frameInterval_ = i; }

  IEventQueue* eventQueue() { return eventQueue_.get(); }

  virtual void initialize() = 0;

private:
  class EventQueueImpl : public IEventQueue {
  public:
    EventQueueImpl(std::mutex* mu,
                   std::vector<std::function<void()>>* q)
        : mu_(mu), q_(q) {}

    void enqueue(std::function<void()> event) override {
      std::lock_guard<std::mutex> lock(*mu_);
      q_->push_back(event);
    }

  private:
    std::mutex* mu_;
    std::vector<std::function<void()>>* q_;
  };

  std::unique_ptr<Model> model_;
  std::unique_ptr<IView<Model>> view_;
  std::unique_ptr<ISoundScheme<Model>> soundScheme_;

  std::unique_ptr<IScreen> screen_;
  std::unique_ptr<ISoundMixer> soundMixer_;

  std::chrono::duration<double, std::milli> frameInterval_;

  std::unique_ptr<IEventQueue> eventQueue_;

  std::mutex eventsMu_;
  std::vector<std::function<void()>> events_;

  std::mutex runningMu_;
  bool running_;
};

} // namespace airball

#endif // AIRBALL_FRAMEWORK_APPLICATION_H
