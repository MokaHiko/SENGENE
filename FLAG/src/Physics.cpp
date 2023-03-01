#include "Physics.h"

namespace flg
{
	std::vector<Body *> PhysicsWorld::m_Bodies{};
	PhysicsWorldProperties PhysicsWorld::m_Properties{};

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
				// TODO: Check Collisions

				// Update Collider Positions
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
			{
				// Check Collision
				Transform *t = &body->BodyTransform;
				CollisionPoints col = body->BodyCollider->TestCollision(t, ray, nullptr);

				if (col.DidCollide)
				{
					hit.CollisionPoint = col.A;
					hit.body = body;
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

}