#ifndef INPUT_H
#define INPUT_H

#pragma once

#include "Core/Application.h"

namespace SGE
{
    class Input
    {
    public:
        static bool Input::IsKeyPressed(int keycode);

        static bool Input::IsMouseButtonPressed(int button);

        static float Input::GetMouseX();

        static float Input::GetMouseY();

        static std::pair<float, float> Input::GetMousePosition();
    };
}

#endif