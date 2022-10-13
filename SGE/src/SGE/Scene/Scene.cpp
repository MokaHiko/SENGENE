#include "Scene.h"
#include "Renderer/Renderer.h"
#include "Systems.h"

#include "Core/TimeStep.h"
namespace SGE {
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}
	
	void Scene::Update(TimeStep timestep)
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
				
				// implment script
				nsc.ScriptInstance->OnUpdate(timestep);
			}
		}
		{
			auto group = m_Registry.group<MeshRendererComponent, TransformComponent>();

			for(auto entity : group)
			{
				auto& model = group.get<MeshRendererComponent>(entity);
				auto& transform = group.get<TransformComponent>(entity);
				Renderer::Draw(model.Model, transform.Position, transform.Rotation, transform.Scale);
			}
		}
		{
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
	}
}