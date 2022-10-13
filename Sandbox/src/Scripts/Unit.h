#ifndef UNIT_H
#define UNIT_H

#pragma once

#include "SGE/SGE.h"
#include <random>

class Unit : public SGE::ScriptableEntity
{
public:
    ~Unit(){}
    virtual void OnUpdate(SGE::TimeStep timestep)
    {
        auto& transform = GetComponent<SGE::TransformComponent>();
        m_Time += timestep.GetMilliSeconds();
        transform.Rotation = glm::vec3(0,glm::radians(sin(m_Time * 0.00025) * 360), 0);
    };
private:
    float m_Time = 0.0f;
};

#endif