#ifndef INPUT_H
#define INPUT_H

#pragma once

#include "Core/Application.h"

namespace SGE {
class Input {
public:
  static bool IsKeyPressed(int keycode);

  static bool IsMouseButtonPressed(int button);

  static float GetMouseX();

  static float GetMouseY();

  static std::pair<float, float> GetMousePosition();
};
} // namespace SGE

#endif