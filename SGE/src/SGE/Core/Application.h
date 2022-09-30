#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once
#include "Core/Core.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Core/Layer.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;
namespace SGE{
    class Application
    {
    public:
        Application();
        virtual ~Application();

        void Update(TimeStep timestep);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        static Application& Get() {return *s_Instance;}
        inline Window& GetWindow() {return *m_Window;}
        void Run();
    private:
        void OnEvent(Event& event);
        bool OnWindowResize(WindowResizeEvent& event);
        bool OnWindowClose(WindowCloseEvent& event);

    private:
        Scope<Window> m_Window;
        LayerStack m_LayerStack;

        static Application* s_Instance;

        float m_LastFrameTime = 0.0f;
        bool m_Running = true;

        ImGuiLayer* m_ImGuiLayer;
    };

    SGE::Application* CreateApplication();
}

#endif