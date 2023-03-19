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
		ResolveCollision(dt);

		for (Body *body : m_Bodies)
		{
			if (body->Type == BodyType::Static)
				continue;

			if (body->Type == BodyType::Dynamic)
				body->Force += body->Mass * m_Properties.Gravity;

			body->Velocity += (body->Force / body->Mass) * dt;
			body->BodyTransform.Position += body->Velocity * dt;

			// TODO: Add World Floor In Properties
			body->BodyTransform.Position.y = std::max(body->BodyTransform.Position.y, 1.4f);

			body->Force = glm::vec3{0.0f};
		}
	}

	PhysicsWorld::Raycasthit PhysicsWorld::Raycast(const Ray *ray, float distance)
	{
		Raycasthit hit = Raycasthit();

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

	void PhysicsWorld::ResolveCollision(float dt)
	{
		for (Body *body : m_Bodies)
		{
			if (body->BodyCollider)
			{
				for (Body *body2 : m_Bodies)
				{
					// Break So Collisions are not Doubled
					if (body == body2)
						break;

					CollisionPoints collisionPoints = body->BodyCollider->TestCollision(&body->BodyTransform, body2->BodyCollider, &body2->BodyTransform);
					if (collisionPoints.DidCollide)
					{
						// TODO: Set WithinCollisionFlag to true
						m_CollisionEnterCallback(collisionPoints, body->GetEntityOwnerID(), body2->GetEntityOwnerID());

						// TODO: Resolve Collision
						// ResolveCollision(body, body2, collisionPoints);
					}
				}
			}
		}
	}

	void PhysicsWorld::AddBody(Body *body)
	{
		m_Bodies.push_back(body);
	}

	void PhysicsWorld::RemoveBody(Body *body)
	{
		m_Bodies.erase(remove(m_Bodies.begin(), m_Bodies.end(), body), m_Bodies.end());
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