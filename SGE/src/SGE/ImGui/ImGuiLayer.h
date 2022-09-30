#ifndef IMGUILAYER_H
#define IMGUILAYER_H

#pragma once

#include "Core/Layer.h"

namespace SGE {
    class ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        void Begin();
        void End();
    };
}

#endif