#pragma once

#include <functional>

inline void actionArrayInvoke(std::function<void(int)> invoke, int size) {
  for (int i = 0; i < size; i++) {
    invoke(i);  // Call the function with the parameter
  }
}