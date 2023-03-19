#pragma once

#include "Body.h"
#include "Collider.h"

namespace flg
{
	namespace algo
	{
		// --- Ray/Raycasts ---
		static CollisionPoints FindRayPlaneCollisionPoints(const Ray *ray, const Collider *collider)
		{
			// return collider->TestCollision((Collider*)(nullptr), ray, ray->Origin);
			return CollisionPoints{};
		}
		static CollisionPoints FindRayPlaneCollisionPoints(const Ray *ray, const PlaneCollider *plane)
		{
			// Rays' origin is a point on the plane.
			if (ray->Origin == plane->Origin)
				return CollisionPoints{ray->Origin, true};

			// Ray and plane are perpendicular.
			float rayDirectionDotPlaneNormal = glm::dot(ray->Direction, plane->Normal);
			if (rayDirectionDotPlaneNormal == 0)
				return CollisionPoints{glm::vec3{0.0f}, false};

			float t = glm::dot(plane->Origin - ray->Origin, plane->Normal) / rayDirectionDotPlaneNormal;
			return CollisionPoints{ray->Origin + (t * ray->Direction), t > 0 ? true : false};
		}

		static CollisionPoints FindRaySphereCollisionPoints(const Ray *ray, const SphereCollider *sphere, const Transform *sphereTransform)
		{
			glm::vec3 sphereWorldCenter = sphereTransform->Position + sphere->Center;
			float b = glm::dot(ray->Direction, ray->Origin - sphereWorldCenter);
			float c = glm::dot(ray->Origin - sphereWorldCenter, ray->Origin - sphereWorldCenter) - (sphere->Radius * sphere->Radius);

			float discriminant = b * b - c;

			// No collision
			if (discriminant < 0)
				return CollisionPoints{};

			// TODO: Optional calculate Point of collision
			float discriminantRoot = glm::sqrt(discriminant);
			float t1 = -b + discriminantRoot;
			float t2 = -b - discriminantRoot;

			float a = glm::dot(ray->Direction, ray->Direction);
			t1 /= a;
			t2 /= a;

			return CollisionPoints{ray->Origin + (ray->Direction * t1), true};
		}

		// --- Physics Colliders Fns ---
		static CollisionPoints FindSphereSphereColissionPoints(
			const SphereCollider *a, const Transform *ta,
			const SphereCollider *b, const Transform *tb)
		{
			glm::vec3 difference = (ta->Position + a->Center) - (tb->Position + b->Center);
			float differenceMagnitude = glm::length(difference);
			float sumRadius = a->Radius + b->Radius;

			if (differenceMagnitude < sumRadius)
			{
				return CollisionPoints{difference / 2.0f, true};
			}

			return {};
		}

		static CollisionPoints FindSpherePlaneCollissionPoints(
			const SphereCollider *a, const Transform *ta,
			const PlaneCollider *b, const Transform *tb)
		{
			return {};
		}

		static CollisionPoints FindPlaneSphereCollissionPoints(
			const PlaneCollider *a, const Transform *ta,
			const SphereCollider *b, const Transform *tb)
		{
			return {};
		}
	}
}