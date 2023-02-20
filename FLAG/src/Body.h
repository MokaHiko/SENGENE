#ifndef BODY_H
#define BODY_H

#pragma once

#include <glm/glm.hpp>

namespace flg {
    enum BodyType
    {
        Static = 0,
        Dynamic = 1,
        Kinematic = 2,
    };

    struct Transform
    {
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Scale;

        Transform()
            :Position(0.0f), Rotation(0.0f), Scale(1.0f) {}
    };

    struct Collider;
    struct Body
    {
        Transform BodyTransform;
        glm::vec3 Velocity;
        glm::vec3 Force;
        float Mass;

        BodyType Type;
        Collider* BodyCollider;
        bool UseGravity;

        Body(uint32_t ownerEntityID = -1)
            :OwnerEntityID(ownerEntityID), BodyTransform({}), Velocity(0.0f), Force(0.0f), Mass(1.0f), Type(BodyType::Static), BodyCollider(nullptr)
        {}

        const glm::vec3& GetPosition() {return BodyTransform.Position;};

        void SetEntityOwnerID(uint32_t id) {OwnerEntityID = id;};
        uint32_t GetEntityOwnerID() {return OwnerEntityID;};
    private:
        uint32_t OwnerEntityID;
    };
} 
#endif