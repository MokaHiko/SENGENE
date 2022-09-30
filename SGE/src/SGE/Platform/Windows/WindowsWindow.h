#ifndef WINDOWSWINDOW_H
#define WINDOWSWINDOW_H

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Core/Window.h"
namespace SGE
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WindowProps& props);
        ~WindowsWindow();

        virtual void SetWindowTitle(const std::string& title) override;
        virtual void OnUpdate() override;
        virtual void SetEventCallBack(EventCallBackFn callBackFn) override;

		inline virtual unsigned int GetWidth() const {return m_Data.Width;}
		inline virtual unsigned int GetHeight() const {return m_Data.Height;}

        virtual void SetVSync(bool isVSync) override;
        virtual void* GetNativeWindow() const;
    private:
        GLFWwindow* m_Window;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallBackFn EventCallback;
        };

        WindowData m_Data;
    };
}

#endif