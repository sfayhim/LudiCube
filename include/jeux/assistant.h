#pragma once
#include <Arduino.h>

namespace Assistant {
  void begin();
  void step();
  bool isCompleted();
}
