#ifndef AIRBALL_SCREEN_ST7789VI_SCREEN_H
#define AIRBALL_SCREEN_ST7789VI_SCREEN_H

#include <condition_variable>
#include <mutex>
#include <thread>

#include "AbstractScreen.h"

#include "st7789vi_frame_writer_smi.h"

namespace airball {

class ST7789VIScreen : public AbstractScreen {
public:
  ST7789VIScreen();

  ~ST7789VIScreen() override;

  virtual void flush() override;

private:
  st7789vi_frame_writer_smi w_;
  unsigned char *data_;
  std::thread paint_;
  std::mutex paint_mu_;
  std::condition_variable paint_cond_;
};

}  // namespace airball

#endif // AIRBALL_SCREEN_ST7789VI_SCREEN_H