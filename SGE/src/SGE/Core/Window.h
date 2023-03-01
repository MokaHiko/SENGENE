#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include "Core/Core.h"
#include "Events/Event.h"
namespace SGE
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string &title = "The Selfish Gene",
					unsigned int width = 1280,
					unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		using EventCallBackFn = std::function<void(Event &)>;

		~Window() = default;
		virtual void OnUpdate() = 0;

		inline virtual unsigned int GetWidth() const = 0;
		inline virtual unsigned int GetHeight() const = 0;

		virtual void SetWindowTitle(const std::string &title) = 0;
		virtual void *GetNativeWindow() const = 0;
		virtual void SetEventCallBack(EventCallBackFn callBackFn) = 0;

		virtual void SetVSync(bool isVSync) = 0;

		static Window *CreateWindow(const WindowProps &props = WindowProps());
	};
}

#endif