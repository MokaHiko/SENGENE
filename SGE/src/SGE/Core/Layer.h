#ifndef LAYER_H
#define LAYER_H

#pragma once

#include "Core/TimeStep.h"
#include "Events/Event.h"

namespace SGE
{
    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer(){};

        virtual void OnAttach(){};
        virtual void OnDetach(){};
        virtual void OnEvent(Event &event){};

        virtual void OnUpdate(TimeStep ts){};
        virtual void OnImGuiRender(){};
    };

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void PushLayer(Layer *layer);
        void PopLayer(Layer *layer);

        void PushOverlay(Layer *layer);
        void PopOverlay(Layer *layer);

        std::vector<Layer *>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer *>::iterator end() { return m_Layers.end(); }

    private:
        std::vector<Layer *> m_Layers;
        uint32_t m_LayerInsertIndex = 0;
    };
}

#endif