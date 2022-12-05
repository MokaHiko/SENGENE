#include "Physics.h"
#include <iostream>

namespace flg {
	std::vector<Body*> PhysicsWorld::m_Bodies{};
	PhysicsWorldProperties PhysicsWorld::m_Properties{};

	PhysicsWorld::~PhysicsWorld()
	{
		m_Bodies.clear();
	}

	void PhysicsWorld::Step(float dt)
	{
		for(Body* body : m_Bodies)
		{
			body->Force += body->Mass * m_Properties.Gravity;

			body->Velocity += (body->Force / body->Mass) * dt;
			body->BodyTransform.Position += body->Velocity * dt;

			body->Force = glm::vec3{0.0f};
		}
	}
	
	void PhysicsWorld::AddBody(Body* body)
	{
		m_Bodies.push_back(body);
	}
	
	void PhysicsWorld::RemoveBody(Body* body)
	{
		// TODO: --- 
	}
	
	void PhysicsWorld::Clear()
	{
		m_Bodies.clear();
	}

}