#include "WindowsWindow.h"
#include <glad/glad.h>

#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

namespace SGE { 
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if(!glfwInit())
			throw std::runtime_error("Failed to initialize GLFW");

		m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), NULL , NULL);

		if(!m_Window)
			throw std::runtime_error("Failed to initialize GLFW");

		glfwMakeContextCurrent(m_Window);

		// init glad after making WindowsWindow current context
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			throw std::runtime_error("Failed to load glad");

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event{};
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMoveEvent event(xpos, ypos);
			data.EventCallback(event);
		});

	}

	Window* Window::CreateWindow(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	
	void WindowsWindow::SetWindowTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
	}
	
	void WindowsWindow::OnUpdate()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
	
	void WindowsWindow::SetEventCallBack(std::function<void(Event&)> callBackFn)
	{
		m_Data.EventCallback = callBackFn;
	}
	
	void WindowsWindow::SetVSync(bool isVSync)
	{
		glfwSwapInterval(isVSync ? 1 : 0);
	}
	
	void* WindowsWindow::GetNativeWindow() const
	{
		return m_Window;
	}
}

