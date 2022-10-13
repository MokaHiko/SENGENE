#include "Application.h"

#include <GLFW/glfw3.h>
#include <cassert>

#include "Renderer/Renderer.h"
#include "ImGui/ImGuiLayer.h"

#include "Core/TimeStep.h"
#include "Core/Input.h"

namespace SGE{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		assert(!s_Instance);
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::CreateWindow());
		m_Window->SetEventCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Renderer::Init();
		m_LastFrameTime = (float)glfwGetTime();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		
	}
	
	void Application::Update(TimeStep timestep)
	{
		for(Layer* layer : m_LayerStack)
			layer->OnUpdate(timestep);

		m_ImGuiLayer->Begin();
		for(Layer* layer : m_LayerStack) // TODO: Only Update ImGuiLayers in Debug
			layer->OnImGuiRender();
		m_ImGuiLayer->End();
	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	
	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	
	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowResizeEvent>(std::bind(&Application::OnWindowResize, this, std::placeholders::_1));

		for(auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(event);
			if(event.m_Handled)
				break;
		}
	}
	
	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
        return true;
	}
	
	void Application::Run()
	{
		while(m_Running)
		{
			float time = (float)glfwGetTime();
			TimeStep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			m_Window->SetWindowTitle(std::to_string(timestep.GetMilliSeconds()));
			
			Update(timestep);

			m_Window->OnUpdate();
		}
	}
}