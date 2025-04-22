#pragma once

class KioskInstance {
public:
  virtual void begin() = 0;
  virtual void task() = 0;
};