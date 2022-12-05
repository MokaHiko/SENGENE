#include "Scene.h"
#include "Renderer/Renderer.h"
#include "SkinnedMeshRenderer/SkinnedMeshRenderer.h"
#include "Systems.h"

#include "Core/TimeStep.h"
namespace SGE {
	Scene::Scene(const std::string& sceneName)
		:m_Name(sceneName) 
	{
	}

	Scene::~Scene()
	{
		flg::PhysicsWorld::Clear();
	}

	void Scene::OnScenePlay()
	{
		// Rebuild Physics World
		auto group = m_Registry.group<RigidBodyComponent>(entt::get<TransformComponent>);
		for (auto e : group)
		{
			Entity entity = {e, this};

			// Get Assigned Transform
			auto& transform = entity.GetComponent<TransformComponent>();

			auto& rb = entity.GetComponent<RigidBodyComponent>();
			
			// Assign Transform Position To Physics Body
			rb.Body.BodyTransform.Position = transform.Position;
			rb.Body.BodyTransform.Rotation = transform.Rotation;

			// Configure Rb Properties
			rb.Body.Type = flg::BodyType::Dynamic;

			// Pass to Physics System
			flg::PhysicsWorld::AddBody(&rb.Body);
		}

		m_SceneState = SCENE_STATE::PLAY;
	}

	void Scene::OnSceneStop()
	{
		// Clear Physics World
		flg::PhysicsWorld::Clear();

		m_SceneState = SCENE_STATE::PAUSE;
	}

	void Scene::Update(TimeStep timestep)
	{
		if (m_SceneState == SCENE_STATE::PLAY)
		{
			// Update Scripts
			{
				auto view = m_Registry.view<NativeScriptComponent>();

				// TODO: Move to On Scene Play
				for(auto entity : view)
				{
					auto& nsc = view.get<NativeScriptComponent>(entity);

					// instantiate script if not yet made
					if(!nsc.ScriptInstance)
					{
						nsc.ScriptInstance = nsc.InstantiateScript();
						nsc.ScriptInstance->m_Entity = Entity{entity, this};
						nsc.ScriptInstance->OnCreate();
						nsc.ScriptInstance->OnStart();
					}
					
					// Implement scripts
					nsc.ScriptInstance->OnUpdate(timestep);
				}
			}

			// Update Physics
			{
				flg::PhysicsWorld::Step(timestep);
				auto group = m_Registry.group<RigidBodyComponent>(entt::get<TransformComponent>);
				for(auto entity : group)
				{
					auto& transform= group.get<TransformComponent>(entity);
					auto& rb = group.get<RigidBodyComponent>(entity);
					transform.Position = rb.Body.GetPosition();
				}
			}
		}
		
		{
			// Update Camera View Matrices
			auto group = m_Registry.group<Camera3DComponent>(entt::get<TransformComponent>);
			for(auto entity : group)
			{
				auto& camera = group.get<Camera3DComponent>(entity);
				if(!camera.IsActive)
					continue;

				auto& transform = group.get<TransformComponent>(entity);
				Camera3DSystem(camera, transform);
			}
		}

		{
			// Draw Meshes
			{
				auto group = m_Registry.group<MeshRendererComponent, TransformComponent>();

				for(auto entity : group)
				{
					auto& model = group.get<MeshRendererComponent>(entity);
					auto& transform = group.get<TransformComponent>(entity);
					Renderer::Draw(model.Model, transform.Position, transform.Rotation, transform.Scale);
				}
			}

			// Draw Animated Meshes
			{
				auto group = m_Registry.group<SkinnedMeshRendererComponent>(entt::get<TransformComponent>);

				for(auto entity : group)
				{
					auto& model = group.get<SkinnedMeshRendererComponent>(entity);
					auto& transform = group.get<TransformComponent>(entity);
					SkinnedMeshRenderer::Draw(model.AnimatedModel, transform.Position, transform.Rotation, transform.Scale);
				}
			}
		}
	}
}