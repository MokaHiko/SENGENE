#include "Scene.h"
#include "Renderer/Renderer.h"
#include "SkinnedMeshRenderer/SkinnedMeshRenderer.h"
#include "Systems.h"

#include "Core/TimeStep.h"
#include <functional>

namespace SGE
{
	Scene::Scene(const std::string &sceneName)
		: m_Name(sceneName)
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
			RegisterToPhysicsWorld(entity);
		}

		// Bind OnCollisionEnterCallback
		flg::PhysicsWorld::SetOnCollisionEnterCallBack(std::bind(&Scene::CollisionEnterCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		// Implement script OnStart methods
		{
			auto view = m_Registry.view<NativeScriptComponent>();

			for (auto entity : view)
			{
				auto &nsc = view.get<NativeScriptComponent>(entity);

				// register and instantiate script if not yet made
				if (!nsc.ScriptInstance)
				{
					nsc.ScriptInstance = nsc.InstantiateScript();
					nsc.ScriptInstance->m_Entity = Entity{entity, this};
					nsc.ScriptInstance->OnCreate();
				}
				nsc.ScriptInstance->OnStart();
			}
		}

		m_SceneState = SCENE_STATE::PLAY;
	}

	void Scene::OnSceneStop()
	{
		// Clear Physics World
		flg::PhysicsWorld::Clear();

		// Change scene state
		m_SceneState = SCENE_STATE::PAUSE;
	}

	void Scene::Update(TimeStep timestep)
	{
		if (m_SceneState == SCENE_STATE::PLAY)
		{
			// Run OnUpdate script overrides
			{
				auto view = m_Registry.view<NativeScriptComponent>();

				for (auto entity : view)
				{
					auto &nsc = view.get<NativeScriptComponent>(entity);

					// instantiate script if not yet made
					if (!nsc.ScriptInstance)
					{
						nsc.ScriptInstance = nsc.InstantiateScript();
						nsc.ScriptInstance->m_Entity = Entity{entity, this};
						nsc.ScriptInstance->OnCreate();
						nsc.ScriptInstance->OnStart();
					}

					if (nsc.ScriptInstance)
					{
						nsc.ScriptInstance->OnUpdate(timestep);
					}
				}
			}

			// Update Physics
			{
				flg::PhysicsWorld::Step(timestep);
				auto group = m_Registry.group<RigidBodyComponent>(entt::get<TransformComponent>);
				for (auto entity : group)
				{
					auto &transform = group.get<TransformComponent>(entity);
					auto &rb = group.get<RigidBodyComponent>(entity);

					// Register all post play created entities
					if (!rb.Registered)
						RegisterToPhysicsWorld({entity, this});

					transform.Position = rb.Body.GetPosition();
				}
			}
		}

		{
			// Update Camera View Matrices
			auto group = m_Registry.group<Camera3DComponent>(entt::get<TransformComponent>);
			for (auto entity : group)
			{
				auto &camera = group.get<Camera3DComponent>(entity);
				if (!camera.IsActive)
					continue;

				auto &transform = group.get<TransformComponent>(entity);
				Camera3DSystem(camera, transform);
			}
		}

		{
			// Draw Meshes
			auto group = m_Registry.group<MeshRendererComponent, TransformComponent>();

			for (auto entity : group)
			{
				auto &model = group.get<MeshRendererComponent>(entity);
				auto &transform = group.get<TransformComponent>(entity);
				Renderer::Draw(model.Model, transform.Position, transform.Rotation, transform.Scale);
			}
		}

		// Draw Animated Meshes
		{
			auto group = m_Registry.group<SkinnedMeshRendererComponent>(entt::get<TransformComponent>);

			for (auto entity : group)
			{
				auto &model = group.get<SkinnedMeshRendererComponent>(entity);
				auto &transform = group.get<TransformComponent>(entity);
				SkinnedMeshRenderer::Draw(model.AnimatedModel, transform.Position, transform.Rotation, transform.Scale);
			}
		}

		// Clean Up
		{
			for (Entity entity : m_EntitiesToDestroy)
			{
				// Remove From Physics World
				if (entity.HasComponent<RigidBodyComponent>())
				{
					flg::PhysicsWorld::RemoveBody(&entity.GetComponent<RigidBodyComponent>().Body);
				}

				// Call On Destroy If Scriptable
				if (entity.HasComponent<NativeScriptComponent>())
				{
					auto &nsc = entity.GetComponent<NativeScriptComponent>();
					nsc.ScriptInstance->OnDestroy();
					// entity.GetComponent<NativeScriptComponent>().DestroyScript(&nsc);
				}

				m_Registry.destroy(entity.m_EntityHandle);
			}
			m_EntitiesToDestroy.clear();
		}
	}

	void Scene::RegisterToPhysicsWorld(Entity e)
	{
		// Get Assigned Transform
		auto &transform = e.GetComponent<TransformComponent>();
		auto &rb = e.GetComponent<RigidBodyComponent>();

		// Assign Owner Entity ID & Transform Position To Physics Body
		rb.Body.SetEntityOwnerID(e.Id());
		rb.Body.BodyTransform.Position = transform.Position;
		rb.Body.BodyTransform.Rotation = transform.Rotation;
		rb.Body.BodyTransform.Scale = transform.Scale;

		// Calculate Values for Colliders
		if (e.HasComponent<PlaneColliderComponent>())
		{
			auto &collider = e.GetComponent<PlaneColliderComponent>();

			// TODO: calculate normal for any orientation
			collider.planeCollider.Normal = {0.0, 1.0f, 0.0};

			rb.Body.BodyCollider = &collider.planeCollider;
		}

		if (e.HasComponent<SphereColliderComponent>())
		{
			auto &collider = e.GetComponent<SphereColliderComponent>();

			rb.Body.BodyCollider = &collider.sphereCollider;
		}

		// Register Rigid Body and Add to Physics System
		rb.Registered = true;
		flg::PhysicsWorld::AddBody(&rb.Body);
	}

	void Scene::CollisionEnterCallback(flg::CollisionPoints &col, uint32_t entityA, uint32_t entityB)
	{
		Entity e1{entityA, this};
		Entity e2{entityB, this};

		if (e1.HasComponent<SGE::NativeScriptComponent>())
		{
			if (e1.GetComponent<SGE::NativeScriptComponent>().ScriptInstance->OnCollisionEnter(col, e2))
				return;
		}

		if (e2.HasComponent<SGE::NativeScriptComponent>())
		{
			if (e2.GetComponent<SGE::NativeScriptComponent>().ScriptInstance->OnCollisionEnter(col, e1))
				return;
		}
	};
	void Scene::CollisionExitCallback(flg::CollisionPoints &col, uint32_t entityA, uint32_t entityB){

	};
}