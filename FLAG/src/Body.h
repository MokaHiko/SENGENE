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

        Transform()
            :Position(0.0f), Rotation(0.0f){}
    };

    struct Body
    {
        Transform BodyTransform;
        glm::vec3 Velocity;
        glm::vec3 Force;
        float Mass;

        BodyType Type;

        Body()
            :BodyTransform({}), Velocity(0.0f), Force(0.0f), Mass(1.0f), Type(BodyType::Static)
        {}

        const glm::vec3& GetPosition() {return BodyTransform.Position;};
        void BodyType(const BodyType bodyType) {Type = bodyType;};
    };


} // namespace flg


#endif