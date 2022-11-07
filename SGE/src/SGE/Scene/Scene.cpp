#include "Scene.h"
#include "Renderer/Renderer.h"
#include "Systems.h"

#include "Core/TimeStep.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
namespace SGE {
	Scene::Scene(const std::string& sceneName)
		:m_Name(sceneName) {}

	Scene::~Scene()
	{
		delete m_PhysicsWorld;
	}

	b2BodyType SGERigidbody2DTypeToBox2D(RigidBody2DComponent::BodyType type)
	{
		switch (type)
		{
		case RigidBody2DComponent::BodyType::Static:
			return b2BodyType::b2_staticBody;
		case RigidBody2DComponent::BodyType::Dynamic:
			return b2BodyType::b2_dynamicBody;
		case RigidBody2DComponent::BodyType::Kinematic:
			return b2BodyType::b2_kinematicBody;
		}

		printf("Uknown RigidBody Type: ");
		return b2BodyType::b2_staticBody; 
	}
	void Scene::OnScenePlay()
	{
		m_PhysicsWorld = new b2World({0.0f, -20.f} );

		auto view = m_Registry.view<RigidBody2DComponent>();
		for (auto e : view)
		{
			auto& entity = Entity{ e, this };
			TransformComponent& transform = entity.GetComponent<TransformComponent>();
			RigidBody2DComponent& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();

			b2BodyDef def{};
			def.type = SGERigidbody2DTypeToBox2D(rigidBody2D.Type);
			def.angle = transform.Rotation.z;
			def.position = { transform.Position.x, transform.Position.y };
			def.linearVelocity.Set(0.0f, 0.0f);
			def.angularVelocity = 0.0f;
			def.linearDamping = 0.0f;
			def.angularDamping = 0.0f;
			def.allowSleep = true;
			def.awake = true;
			def.fixedRotation = false;
			def.bullet = false;
			def.enabled = true;
			def.gravityScale = 1.0f;

			// TODO: possibly change from void* to map
			b2Body* body = m_PhysicsWorld->CreateBody(&def);
			rigidBody2D.RuntimeBody = (void*)body;

			// Configure Collider
			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				BoxCollider2DComponent& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape colliderShape;
				colliderShape.SetAsBox(bc2d.scale.x * transform.Scale.x, bc2d.scale.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &colliderShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}

		m_SceneState = SCENE_STATE::PLAY;
	}

	void Scene::OnSceneStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
		
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
					
					// implment scripts
					nsc.ScriptInstance->OnUpdate(timestep);
				}
			}

			// Update Physics
			{
				const int32_t velocityIterations = 8;
				const int32_t positionIterations = 3;
				m_PhysicsWorld->Step(timestep, velocityIterations, positionIterations);

				auto group = m_Registry.group<RigidBody2DComponent>(entt::get<TransformComponent>);
				for (auto entity : group)
				{
					auto& transform = group.get<TransformComponent>(entity);
					auto& rigidBody2D = group.get<RigidBody2DComponent>(entity);

					b2Body* body = (b2Body*)(rigidBody2D.RuntimeBody);
					
					// Update Properties
					body->SetType(SGERigidbody2DTypeToBox2D(rigidBody2D.Type));

					const b2Vec2& position2D = body->GetPosition();

					transform.Position.x = position2D.x;
					transform.Position.y = position2D.y;
					transform.Rotation.z = body->GetAngle();
				}
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

		{
			auto group = m_Registry.group<MeshRendererComponent, TransformComponent>();

			for(auto entity : group)
			{
				auto& model = group.get<MeshRendererComponent>(entity);
				auto& transform = group.get<TransformComponent>(entity);
				Renderer::Draw(model.Model, transform.Position, transform.Rotation, transform.Scale);
			}
		}
	}
}