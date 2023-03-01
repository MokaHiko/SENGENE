#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include <glm/glm.hpp>

namespace flg
{
    struct CollisionPoints
    {
        glm::vec3 A;              // Furthest point of A into B or Point of Collisionjjj
        glm::vec3 B;              // Furthest point of B into A
        glm::vec3 NormalDelta;    // B - A Normalized
        float MagnitudeDelta = 0; // Magnitude of B - A
        bool DidCollide = false;

        CollisionPoints() = default;

        // Ray cast Constructor
        CollisionPoints(const glm::vec3 &a, bool didCollide)
            : A(a), DidCollide(didCollide) {}
    };

    struct Collision
    {
        glm::vec3 Point;
        bool DidCollide = false;
    };

    // Forward declare colliders/collidable objects
    struct SphereCollider;
    struct PlaneCollider;
    struct Ray;
    struct Transform;

    struct Collider
    {
        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Collider *collider,
            const Transform *colliderTransform) const { return CollisionPoints{}; }

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const SphereCollider *collider,
            const Transform *colliderTransform) const { return CollisionPoints{}; }

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const PlaneCollider *collider,
            const Transform *colliderTransform) const { return CollisionPoints{}; }

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Ray *ray,
            const Transform *rayTransform) const { return CollisionPoints{}; }
    };

    struct PlaneCollider : public Collider
    {
    public:
        glm::vec3 Origin;
        glm::vec3 Normal;
        glm::vec2 Bounds;

        PlaneCollider(glm::vec3 origin, glm::vec3 normal, glm::vec3 bounds);
        PlaneCollider();

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Collider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const SphereCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const PlaneCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Ray *ray,
            const Transform *rayTransform) const override;
    };

    struct SphereCollider : public Collider
    {
    public:
        glm::vec3 Center;
        float Radius;

        SphereCollider(glm::vec3 center, float radius);
        SphereCollider();

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Collider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const SphereCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const PlaneCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Ray *ray,
            const Transform *rayTransform) const override;
    };

    struct Ray : public Collider
    {
    public:
        glm::vec3 Direction;
        glm::vec3 Origin;
        Ray(const glm::vec3 origin, const glm::vec3 direction);

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Collider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const SphereCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const PlaneCollider *collider,
            const Transform *colliderTransform) const override;

        virtual CollisionPoints TestCollision(
            const Transform *transform,
            const Ray *ray,
            const Transform *rayTransform) const override;
    };
}

#endif