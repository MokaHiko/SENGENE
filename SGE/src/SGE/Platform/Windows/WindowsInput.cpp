#include "Core/Input.h"
#include <GLFW/glfw3.h>
#include "Renderer/Renderer.h"

namespace SGE  {
	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

#ifdef SGE_RELEASE_MODE
	float Input::GetMouseX()
	{
		auto[x,y] =	GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto[x,y] =	GetMousePosition();
		return y;
	}
	
	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return {(float)xpos, (float)ypos};
	}
#endif

}