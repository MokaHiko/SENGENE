#include "Collider.h"
#include "Algorithms.h"

namespace flg {
	PlaneCollider::PlaneCollider(glm::vec3 origin, glm::vec3 normal, glm::vec3 bounds)
		:Origin(origin), Normal(normal), Bounds(bounds) {}
	PlaneCollider::PlaneCollider()
		:Origin(0.0f), Normal(0.0f), Bounds(0.0f) {}

	CollisionPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform
	) const 
	{
		// Double dispatch to resolve both collider types. 
		// TODO: Remove Comment: i.e collider->specific_collider->TestCollision(transform, PlaneCollider, transform);
		return collider->TestCollision(colliderTransform, this, transform);
	};

	CollisionPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const SphereCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const PlaneCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints PlaneCollider::TestCollision(
		const Transform* transform,
		const Ray* ray,
		const Transform* rayTransform
	) const
	{
		return algo::FindRayPlaneCollisionPoints(ray, this);
	}
	
	SphereCollider::SphereCollider(glm::vec3 center, float radius)
		:Center(center), Radius(radius) {}
	SphereCollider::SphereCollider()
		:Center(0.0f), Radius(0.0f) {}

	CollisionPoints SphereCollider::TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform
	) const 
	{
		return collider->TestCollision(colliderTransform, this, transform);
	};

	CollisionPoints SphereCollider::TestCollision(
		const Transform* transform,
		const SphereCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints SphereCollider::TestCollision(
		const Transform* transform,
		const PlaneCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints SphereCollider::TestCollision(
		const Transform* transform,
		const Ray* ray,
		const Transform* rayTransform
	) const
	{
		return algo::FindRaySphereCollisionPoints(ray, this);
	};

	Ray::Ray(const glm::vec3 origin, const glm::vec3 direction)
		:Origin(origin), Direction(direction) {}

	CollisionPoints Ray::TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform
	) const 
	{
		return collider->TestCollision(colliderTransform, this, transform);
	};

	CollisionPoints Ray::TestCollision(
		const Transform* transform,
		const SphereCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints Ray::TestCollision(
		const Transform* transform,
		const PlaneCollider* collider,
		const Transform* colliderTransform
	) const 
	{
		return CollisionPoints{};
	};

	CollisionPoints Ray::TestCollision(
		const Transform* transform,
		const Ray* ray,
		const Transform* rayTransform
	) const
	{
		// TODO: Ray Intersection
		return CollisionPoints{};
	};
}