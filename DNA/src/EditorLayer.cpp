#include "EditorLayer.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scripts/Board.h"
#include "Scripts/CameraController.h"
#include "Scripts/Unit.h"

#include <Core/Application.h>
#include <imgui/imgui.h>

#include "Utils/PlatformUtils.h"

#include <GrassRenderer/GrassRenderer.h>
#include <SkinnedMeshRenderer/SkinnedMeshRenderer.h>

// Define Inputs relative to viewport in edit mode
glm::vec2 EditorLayer::editorMouseInput = glm::vec2{0.0f};

namespace SGE
{
#ifndef SGE_RELEASE_MODE
  float Input::GetMouseX()
  {
    return EditorLayer::editorMouseInput.x;
  }
  float Input::GetMouseY() { return EditorLayer::editorMouseInput.y; }
  std::pair<float, float> Input::GetMousePosition()
  {
    return std::pair<float, float>(EditorLayer::editorMouseInput.x,
                                   EditorLayer::editorMouseInput.y);
  }
#endif
} // namespace SGE

EditorLayer::~EditorLayer() {}

void EditorLayer::OnAttach()
{
  // Configure App Settings
  {
    SGE::Application::Get().GetWindow().SetWindowTitle("DNA Editor");
  }

  // Load Engine Resources
  {
    // Load Default Shaders
    SGE::Shader::CreateShader("assets/shaders/phong_instanced_shader.vert",
                              "assets/shaders/phong_instanced_shader.frag");
    SGE::Shader::CreateShader(
        "assets/shaders/phong_instanced_shader_animated.vert",
        "assets/shaders/phong_instanced_shader_animated.frag");

    // Init Used Renderers
    SGE::Renderer::Init();
    SGE::SkinnedMeshRenderer::Init();
    SGE::GrassRenderer::Init(SGE::ResourceManager::CreateModel(
                                 "assets/models/grass/blade.fbx", true, 100000),
                             SGE::Shader::CreateShader(
                                 "assets/shaders/grass_instanced_shader.vert",
                                 "assets/shaders/grass_instanced_shader.frag"));

    // Load Basic Shapes
    SGE::Model::CreateModel("assets/models/cube/cube.obj");
    LoadScene("assets/scenes/chess.selfish");

    m_DebugConsolePanel.Log<std::string>("Resources Loaded!");
  }

  // Load Chess Game Demo
  {
    // Load Resources
    SGE::Model::CreateModel("assets/models/Mecha/Mecha.fbx");
    m_Scene->CreateEntity("MainBoard").AddNativeScriptComponent<Board>();
  }
}

void EditorLayer::OnDetach() {}

void EditorLayer::OnEvent(SGE::Event &event)
{
  if (event.GetEventType() == SGE::EventType::MouseMove)
  {
    auto view = m_Scene->Registry()
                    .view<SGE::EventWatcherComponent<SGE::MouseMoveEvent>>();
    for (auto entity : view)
    {
      auto &eventWatcher =
          view.get<SGE::EventWatcherComponent<SGE::MouseMoveEvent>>(entity);
      eventWatcher.EntityCallBack(*(SGE::MouseMoveEvent *)&event);
    }
  }
  if (event.GetEventType() == SGE::EventType::WindowResize)
  {
    SGE::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<SGE::WindowResizeEvent>(
        std::bind(&EditorLayer::OnWindowResize, this, std::placeholders::_1));
  }
}

void EditorLayer::OnUpdate(SGE::TimeStep ts)
{
  m_Framebuffer->Bind();

  // --------- Scene Update ---------

  // Update editor relative input
  auto [mx, my] = ImGui::GetMousePos();
  int x = (int)(mx - m_SceneData.ViewPortBounds[0].x);
  int y = (int)(my - m_SceneData.ViewPortBounds[0].y);
  EditorLayer::editorMouseInput = {x, y};

  m_Scene->Update(ts);

  // --------- Scene Update ---------

  // --------- Renderering ---------

  // Common Render Commands
  // glClearColor(0.1f, 0.1, 0.1, 1.0f);
  glClearColor(1.0f, 1.0f, 1.0f, 0.75f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  SGE::Renderer::Begin(); // TODO: Allow Renderers To Have Unique Cameras
  SGE::Renderer::End();

  SGE::SkinnedMeshRenderer::Begin();
  SGE::SkinnedMeshRenderer::End();

  SGE::GrassRenderer::Begin();
  SGE::GrassRenderer::End();

  // --------- Renderering ---------

  // --------- Debug Widgets ---------
  if (m_EditorState == EditorState::GamePaused)
  {
    if (SGE::Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
      SGE::Entity camera = SGE::Renderer::GetSceneData().MainCamera;
      CameraController *cameraController =
          camera.GetNativeScriptComponent<CameraController>();
      if (!cameraController)
        return;

      glm::vec3 rayDir = cameraController->MouseToWorldCoordinates();
      auto ray = flg::Ray(
          camera.GetComponent<SGE::TransformComponent>().Position, rayDir);
      auto hit = flg::PhysicsWorld::Raycast(&ray, 10000);
      if (hit.DidHit())
      {
        glm::vec3 colPoint = hit.CollisionPoint;
        SGE::Entity hitEntity = {hit.body->GetEntityOwnerID(), m_Scene.get()};

        printf("Hit: %s\n",
               hitEntity.GetComponent<SGE::TagComponent>().Tag.c_str());
        m_SceneHierarchyPanel.SetSelectedEntity(hitEntity);
      }
    }
  }

  m_FrameTime = ts.GetMilliSeconds();

  // --------- Debug Widghets ---------
  m_Framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
  static bool opt_isOpen = true;
  static bool opt_fullscreen = true;
  static bool opt_padding = false;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window
  // not dockable into, because it would be confusing to have two docking
  // targets within each others.
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen)
  {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }
  else
  {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  if (!opt_padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", &opt_isOpen, window_flags);
  if (!opt_padding)
    ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  // Submit the DockSpace
  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
  else
  {
    std::cout << "Docking Disabled" << std::endl;
  }

  // Menus
  if (ImGui::BeginMainMenuBar())
  {
    // Application Menus
    if (ImGui::BeginMenu("File"))
    {
      ShowFileMenuHierarchy();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
      if (ImGui::MenuItem("Undo", "CTRL+Z"))
      {
      }
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
      {
      } // Disabled item
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X"))
      {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C"))
      {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V"))
      {
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        // Disabling fullscreen would allow the window to be moved to the front
        // of other windows, which we can't undo at the moment without finer
        // window depth/z control.
        ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
        ImGui::MenuItem("Padding", NULL, &opt_padding);
        ImGui::Separator();

        if (ImGui::MenuItem("Flag: NoSplit", "",
                            (dockspace_flags & ImGuiDockNodeFlags_NoSplit) !=
                                0))
        {
          dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
        }
        if (ImGui::MenuItem("Flag: NoResize", "",
                            (dockspace_flags & ImGuiDockNodeFlags_NoResize) !=
                                0))
        {
          dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
        }
        if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
                            (dockspace_flags &
                             ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
        {
          dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
        }
        if (ImGui::MenuItem(
                "Flag: AutoHideTabBar", "",
                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
        {
          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
        }
        if (ImGui::MenuItem(
                "Flag: PassthruCentralNode", "",
                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                opt_fullscreen))
        {
          dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
        }
        ImGui::Separator();

        if (ImGui::MenuItem("Close", NULL, false, &opt_isOpen != nullptr))
          opt_isOpen = false;
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }

    // Panel Menus
    m_SceneHierarchyPanel.ShowMenu();

    ImGui::EndMainMenuBar();
  }

  // Panels
  m_SceneHierarchyPanel.OnImGuiRender();
  m_DebugConsolePanel.OnImGuiRender();
  ImGui::End();

  // Application Viewport of Dockspace
  ShowGameViewPort();

  ImGui::Begin("Gizmos Control");
  ImGui::Text("FPS: %0.2f fps", 1.0f / m_FrameTime * 1000.0);
  ImGui::Text("FrameTime: %0.2f ms", m_FrameTime);
  ImGui::Text("Selected Entity: %s", m_SceneHierarchyPanel.GetSelectedEntity()
                                         .GetComponent<SGE::TagComponent>()
                                         .Tag.c_str());

  ImGui::End();
}

void EditorLayer::LoadScene(const std::string &filePath)
{
  // App/Settings Configuration
  m_Scene = SGE::CreateRef<SGE::Scene>();
  SGE::FramebufferSpecification spec{};
  if (!m_Framebuffer)
  {
    spec.Width = 2560;
    spec.Height = 1080;
  }
  else
  {
    spec = m_Framebuffer->GetFrameBufferSpecification();
  }

  m_Framebuffer = SGE::Framebuffer::CreateFramebuffer(spec);
  m_ViewPortSize = {spec.Width, spec.Height};

  // Set UI Panels SceneContext
  m_SceneHierarchyPanel.SetContext(m_Scene);
  m_DebugConsolePanel.SetContext(m_Scene);

  SGE::SceneSerializer serializer(m_Scene);
  serializer.Deserialize(filePath);

  ResetScene();
  m_ViewPortSize = {spec.Width, spec.Height};
}

void EditorLayer::ResetScene()
{
  m_SceneData.ProjectionMatrix = glm::perspective(
      glm::radians(90.0f),
      (float)m_SceneData.SceneWidth / (float)m_SceneData.SceneHeight, 0.1f,
      1000.0f);
  // cameras
  {
    // TODO: Add Script Serialization
    auto view = m_Scene->Registry().view<SGE::Camera3DComponent>();
    for (auto e : view)
    {
      SGE::Entity entity{e, m_Scene.get()};
      m_SceneData.MainCamera = entity;
      entity.AddNativeScriptComponent<CameraController>();
      break;
    }
  }

  // directional lights
  {
    auto view = m_Scene->Registry().view<SGE::DirectionalLightComponent>();
    for (auto e : view)
    {
      SGE::Entity entity{e, m_Scene.get()};
      m_SceneData.DirectionalLight = entity;
      break;
    }
  }

  // Attach scene data to renderers
  SGE::Renderer::Configure(m_SceneData);
  SGE::SkinnedMeshRenderer::Configure(m_SceneData);
  SGE::GrassRenderer::Configure(m_SceneData);
}

bool EditorLayer::OnWindowResize(SGE::WindowResizeEvent &event)
{
  m_Framebuffer->Resize(event.GetWidth(), event.GetHeight());
  m_ViewPortSize = {event.GetWidth(), event.GetHeight()};
  return false;
}

void EditorLayer::ShowFileMenuHierarchy()
{
  if (ImGui::MenuItem("New"))
  {
  }
  if (ImGui::MenuItem("Open", "Ctrl+O"))
  {
    std::string filePath =
        SGE::FileDialogs::OpenFile("SENGINE Scene (*.selfish)\0*.selfish\0");
    if (!filePath.empty())
      LoadScene(filePath);
  }
  if (ImGui::BeginMenu("Open Recent"))
  {
    ImGui::MenuItem("fish_hat.c");
    ImGui::MenuItem("fish_hat.inl");
    ImGui::MenuItem("fish_hat.h");
    ImGui::EndMenu();
  }
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
    SGE::SceneSerializer serializer(m_Scene);
    serializer.Serialize("assets/scenes/example.selfish");
  }

  if (ImGui::MenuItem("Save As.."))
  {
    std::string filePath =
        SGE::FileDialogs::SaveFile("SENGINE Scene (*.selfish)\0*.selfish\0");
    if (!filePath.empty())
    {
      SGE::SceneSerializer serializer(m_Scene);
      serializer.Serialize(filePath);
    }
  }

  ImGui::Separator();
  if (ImGui::BeginMenu("Options"))
  {
    static bool enabled = true;
    ImGui::MenuItem("Enabled", "", &enabled);
    ImGui::BeginChild("child", ImVec2(0, 60), true);
    for (int i = 0; i < 10; i++)
      ImGui::Text("Scrolling Text %d", i);
    ImGui::EndChild();
    static float f = 0.5f;
    static int n = 0;
    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
    ImGui::InputFloat("Input", &f, 0.1f);
    ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Colors"))
  {
    float sz = ImGui::GetTextLineHeight();
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
      const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddRectFilled(
          p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
      ImGui::Dummy(ImVec2(sz, sz));
      ImGui::SameLine();
      ImGui::MenuItem(name);
    }
    ImGui::EndMenu();
  }

  // Here we demonstrate appending again to the "Options" menu (which we already
  // created above) Of course in this demo it is a little bit silly that this
  // function calls BeginMenu("Options") twice. In a real code-base using it
  // would make senses to use this feature from very different code locations.
  if (ImGui::BeginMenu("Options")) // <-- Append!
  {
    static bool b = true;
    ImGui::Checkbox("SomeOption", &b);
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Disabled", false)) // Disabled
  {
    IM_ASSERT(0);
  }
  if (ImGui::MenuItem("Checked", NULL, true))
  {
  }
  if (ImGui::MenuItem("Quit", "Alt+F4"))
  {
    SGE::Application::Get().ShutDown();
  }
}

void EditorLayer::ShowGameViewPort()
{
  ImGui::Begin("View Port Menu");
  static std::string runLabel = "Play";
  if (ImGui::Button(runLabel.c_str()))
  {
    if (m_Scene->m_SceneState == SGE::SCENE_STATE::PAUSE)
    {
      m_Scene->OnScenePlay();
      runLabel = "Pause";
      m_EditorState = EditorState::GameRunning;
    }
    else
    {
      m_Scene->OnSceneStop();
      runLabel = "Play";
      m_EditorState = EditorState::GamePaused;
    }
  }
  ImGui::End();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
  static bool isOpen = true;

  ImGui::Begin("View Port", &isOpen);
  auto dockSize = ImGui::GetWindowSize();

  auto viewPortOffset = ImGui::GetCursorPos();
  uint32_t textureID = m_Framebuffer->GetColorAttachment();
  ImGui::Image((void *)textureID, ImVec2(dockSize.x, dockSize.y), ImVec2(0, 1),
               ImVec2(1, 0));

  if (dockSize.x != m_Framebuffer->GetFrameBufferSpecification().Width ||
      dockSize.y != m_Framebuffer->GetFrameBufferSpecification().Height)
  {
    // Rebuild framebuffer when viewport changes
    m_Framebuffer->Resize(dockSize.x, dockSize.y);

    // Recalculate game bounds for editor relative input
    auto windowSize = ImGui::GetWindowSize();
    ImVec2 minBound = ImGui::GetWindowPos();
    minBound.x += viewPortOffset.x;
    minBound.y += viewPortOffset.y;
    ImVec2 maxBound = {minBound.x + windowSize.x, minBound.y + windowSize.y};

    m_SceneData.SceneWidth = dockSize.x;
    m_SceneData.SceneHeight = dockSize.y;
    m_SceneData.ViewPortBounds[0] = glm::vec2{minBound.x, minBound.y};
    m_SceneData.ViewPortBounds[1] = glm::vec2{maxBound.x, maxBound.y};
    m_SceneData.ProjectionMatrix = glm::perspective(
        glm::radians(90.0f),
        (float)m_SceneData.SceneWidth / (float)m_SceneData.SceneHeight, 0.1f,
        1000.0f);

    SGE::Renderer::Configure(m_SceneData);
    SGE::SkinnedMeshRenderer::Configure(m_SceneData);
    SGE::GrassRenderer::Configure(m_SceneData);
  }

  ImGui::End();
  ImGui::PopStyleVar();
}