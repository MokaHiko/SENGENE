#include "Physics.h"

namespace flg
{
	// Bodies
	std::vector<Body *> PhysicsWorld::m_Bodies{};
	PhysicsWorldProperties PhysicsWorld::m_Properties{};

	// Default Callbacks
	std::function<void(CollisionPoints &col, uint32_t entityA, uint32_t entityB)> PhysicsWorld::m_CollisionEnterCallback = [](CollisionPoints, uint32_t, uint32_t) {};
	std::function<void(CollisionPoints &col, uint32_t entityA, uint32_t entityB)> PhysicsWorld::m_CollisionExitCallback = [](CollisionPoints, uint32_t, uint32_t) {};

	PhysicsWorld::~PhysicsWorld()
	{
		m_Bodies.clear();
	}

	void PhysicsWorld::Step(float dt)
	{
		for (Body *body : m_Bodies)
		{
			if (body->BodyCollider)
			{
				for (Body *body2 : m_Bodies)
				{
					if (body == body2)
						continue;

					auto collisionPoints = body->BodyCollider->TestCollision(&body->BodyTransform, body2->BodyCollider, &body2->BodyTransform);
					if (collisionPoints.DidCollide)
					{
						m_CollisionEnterCallback(collisionPoints, body->GetEntityOwnerID(), body2->GetEntityOwnerID());
						// TODO: Set WithinCollisionFlag to true
					}
				}

				// TODO: Resolve Positions
			}

			if (body->Type != BodyType::Static)
				body->Force += body->Mass * m_Properties.Gravity;

			body->Velocity += (body->Force / body->Mass) * dt;
			body->BodyTransform.Position += body->Velocity * dt;

			body->Force = glm::vec3{0.0f};
		}
	}

	PhysicsWorld::Raycasthit PhysicsWorld::Raycast(const Ray *ray, float distance)
	{
		Raycasthit hit = Raycasthit();

		// TODO: Optimize with accelerated structures
		for (Body *body : m_Bodies)
		{
			if (body->BodyCollider != nullptr)
			{ // Check Collision
				Transform *t = &body->BodyTransform;
				CollisionPoints col = body->BodyCollider->TestCollision(t, ray, nullptr);

				if (col.DidCollide)
				{
					hit.CollisionPoint = col.A;
					hit.body = body;

					// TODO: CHANGE FROM FIRST HIT TO ALL HIT
					break;
				}
			}
		}

		return hit;
	}

	void PhysicsWorld::AddBody(Body *body)
	{
		m_Bodies.push_back(body);
	}

	void PhysicsWorld::RemoveBody(Body *body)
	{
		// TODO: - Remove Body
	}

	void PhysicsWorld::Clear()
	{
		m_Bodies.clear();
	}

	void PhysicsWorld::SetOnCollisionEnterCallBack(CollisionCallbackFn onEnterFn)
	{
		m_CollisionEnterCallback = onEnterFn;
	}

	void PhysicsWorld::SetOnCollisionExitCallBack(CollisionCallbackFn onExit)
	{
		m_CollisionExitCallback = onExit;
	}

}